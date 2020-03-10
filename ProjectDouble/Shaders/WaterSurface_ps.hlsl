//--------------------------------------------------------------------------------------
// Water height pixel shader
//--------------------------------------------------------------------------------------
// Water surface pixel shader, combines refraction, reflection and specular lighting

#include "Common.hlsli"


//--------------------------------------------------------------------------------------
// Textures maps
//--------------------------------------------------------------------------------------

// Note that the texture register numbers are important - slot 0 is not used here, but is used for the diffuse texture in
// other shaders, so the first texture goes to slot 1. Similarly there is a water height map used for other shaders in slot 2
// We make sure each map gets a unique slot across all the shaders in use at any given point
Texture2D NormalHeightMap : register(t1); // Normal/height map for the water waves

// Water reflection and refraction texture maps (a rendering of the reflected scene and of the scene below the water)
Texture2D RefractionMap : register(t3);
Texture2D ReflectionMap : register(t4);

SamplerState StandardFilter : register(s0); // Filtering used on most textures (trilinear or anisotropic - chosen on the C++ side)
SamplerState BilinearMirror : register(s1); // We use mirror mode for the reflection and refraction because pixels off screen might come
                                            // into view due to the water wiggling. Mirror mode will put some vaguely sensible colours there
                                            // although it is a bit of a cheat. An alternative solution is to render the reflection / refraction
                                            // maps larger than they need to be but that adds complexity for little gain

//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

float4 main(WorldPositionPixelShaderInput input) : SV_Target
{
	// Sample the normal at this point on the water's surface. Sample at four different sizes and combine to give complex waves
	// All UVs are moving, different speeds for each size. Normals are stored in rgb 0->1 range, so they are changed to xyz -1->1 range (* 2 - 1)
	float2 waterUV = input.uv;
	float3 normal1 = NormalHeightMap.Sample(StandardFilter, WaterSize1 * (waterUV + gWaterMovement * WaterSpeed1)).xyz * 2.0f - 1.0f;
	float3 normal2 = NormalHeightMap.Sample(StandardFilter, WaterSize2 * (waterUV + gWaterMovement * WaterSpeed2)).xyz * 2.0f - 1.0f;
	float3 normal3 = NormalHeightMap.Sample(StandardFilter, WaterSize3 * (waterUV + gWaterMovement * WaterSpeed3)).xyz * 2.0f - 1.0f;
	float3 normal4 = NormalHeightMap.Sample(StandardFilter, WaterSize4 * (waterUV + gWaterMovement * WaterSpeed4)).xyz * 2.0f - 1.0f;
  
	// When sampling the water at different sizes, the normals change because we are not changing the height at each size, so correct the
	// normals for that. Alternative is to leave this out and scale the heights used in the vertex shader. This approach gives choppier waves.
	normal1.y *= WaterSize1;
	normal2.y *= WaterSize2;
	normal3.y *= WaterSize3;
	normal4.y *= WaterSize4;

	// Have to renormalize all the normals because of the y scaling above, also swap the y and z axes because normal maps point down the z axis, water points up the y axis.
	float3 waterNormal = (normalize(normal1) + normalize(normal2) + normalize(normal3) + normalize(normal4)).xzy;
	waterNormal.y /= (gWaveScale + 0.001f); // User control of wave height also affects normals, +0.001 to avoid divide by 0 if user chooses 0 height waves
	waterNormal = normalize(waterNormal);  // Final normalization for above line

	// Doing correct reflection and refraction from a bumpy surface is difficult without going into full raytracing-like solutions. The
	// usual approximation is to use the xz from the water surface normal to distort the UVs into the reflection and refractions (wiggle!).
	// The idea is that flat water has a normal that points straight up, i.e. x and z are 0, which gives you 0 distortion. All the code
	// examples and discussions I could find only use first line of code below. But that's incomplete, the water normal is in world space,
	// but the reflection and refraction textures are in camera space. The second line of code corrects for that, it's a 2D matrix multiply.
	// The reason people get away without the correct version is because there are so many approximations going on here and the distortion 
	// due to water so complex anyway, that one missing step doesn't make a noticable difference...
	// - Try setting offsetDir = waterNormal2D;  instead of the complex multiplication to see the effect of doing it wrong
	float2 waterNormal2D = waterNormal.xz;
	float2 offsetDir = float2(dot(waterNormal2D, normalize(gCameraMatrix[0].xz)), dot(waterNormal2D, normalize(gCameraMatrix[2].xz)));

	// Sample the depth to the refracted scene at this pixel and the height to the refected scene (the textures rendered in earlier passes)
	float2 screenUV = input.projectedPosition.xy / float2(gViewportWidth, gViewportHeight);
	float refractionDepth  = RefractionMap.Sample(BilinearMirror, screenUV).a;
	float reflectionHeight = ReflectionMap.Sample(BilinearMirror, screenUV).a;

	// Distort the UVs in screen space based on the distance travelled by the light and the the offset direction from the surface
	// normal. This is an approximation, not physically accurate. When light is bent due to reflection/refraction then the further
	// it travels, the more offset the object we end up seeing at that point. Most games don't account for this and just have a
	// fixed maximum offset. The advantage of this method is that objects deep underwater are much more distorted. The disadvantage
	// is that it is more likely to try and sample offscreen.
	float2 refractionUV = screenUV + RefractionDistortion * refractionDepth  * offsetDir / input.projectedPosition.w;
	float2 reflectionUV = screenUV + ReflectionDistortion * reflectionHeight * offsetDir / input.projectedPosition.w;
	float4 refractColour = RefractionMap.Sample(BilinearMirror, refractionUV) * RefractionStrength; 
	float4 reflectColour = ReflectionMap.Sample(BilinearMirror, reflectionUV) * ReflectionStrength;

	// Fade out reflections at water's edge to avoid errors from using a planar approximation to a bumpy surface
	reflectColour = lerp(refractColour, reflectColour, saturate(refractionDepth * MaxDistortionDistance / (0.5f * MaxWaveHeight * gWaveScale)));

	// Specular lighting calculation. As the water reflects the sky and the lights, then strictly speaking specular light is not
	// needed, because it is just an approximation for the reflection of the light. However, without implementing HDR lighting (high 
	// dynamic range) the reflected lights are quite dim, so use the specular lighting equations to get a stronger effect.
	float3 normalToCamera = normalize(gCameraPosition - input.worldPosition);

	// Light 1
	float3 vectorToLight = gLight1Position - input.worldPosition; 
	float3 normalToLight = normalize(vectorToLight);
	float3 halfwayVector = normalize(normalToLight + normalToCamera);
	float3 specularLight1 = gLight1Colour * pow( max( dot(waterNormal, halfwayVector), 0 ), gSpecularPower ) / length(vectorToLight);

	// Light 2
	vectorToLight = gLight2Position - input.worldPosition;
	normalToLight = normalize(vectorToLight);
	halfwayVector = normalize(normalToLight + normalToCamera);
	float3 specularLight2 = gLight2Colour * pow( max( dot(waterNormal, halfwayVector), 0 ), gSpecularPower ) / length(vectorToLight);

	// Add the effect of the two lights into the reflected colour
	reflectColour.rgb += SpecularStrength * (specularLight1 + specularLight2);

	// Fresnel effect: the reflected and refracted light is blended based on angle of viewer to surface normal. A glancing angle
	// gives more reflection, straight down into water gives more refraction  
	float n1 = 1.0; // Refractive index of air
	float n2 = WaterRefractiveIndex; 
	float r0 = ((n1 - n2) / (n1 + n2));
	r0 *= r0;
	float exp = pow(1 - saturate(dot(waterNormal, normalToCamera)), 5);
	float fresnel = saturate(lerp(r0, 1, exp));
	return lerp(refractColour, reflectColour, fresnel);
}

