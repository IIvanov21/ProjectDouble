//--------------------------------------------------------------------------------------
//	Water rendering
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Water Constants
//--------------------------------------------------------------------------------------

static const float  RefractionDistortion  = 20.0f; // How distorted the refractions are
static const float  ReflectionDistortion  = 16.0f; // How distorted the reflections are
static const float  MaxDistortionDistance = 40;    // Depth/height at which maximum distortion is reached
static const float  SpecularStrength      = 2.0f;  // Strength of specular lighting added to reflection, reduce to make water less shiny
static const float  RefractionStrength    = 0.9f;  // Maximum level of refraction (0-1), use to help define water edge but should remain high, use WaterExtinction
static const float  ReflectionStrength    = 0.95f; // Maximum level of reflection (0-1), reduce to make water less reflective, but it will also get darker
static const float  WaterRefractiveIndex  = 1.5f;  // Refractive index of clean water is 1.33. Impurities increase this value and values up to about 7.0 are sensible
                                                   // Affects the blending of reflection and refraction. Higher values give more reflection
static const float3 WaterExtinction   = float3(15,75,300); // How far red, green and blue light can travel in the water. Values for clear tropical water
//static const float3 WaterExtinction   = float3(15,16,25); // Values for unclear sea water
//static const float3 WaterExtinction   = float3(9,7,3); // Values for flood water
static const float  WaterDiffuseLevel = 0.5f;  // Brightness of particulates in water (change brightness of dirty water)

// Water normal map/height map is sampled four times at different sizes, then overlaid to give a complex wave pattern
static const float WaterSize1  = 0.5f;
static const float WaterSize2  = 1.0f;
static const float WaterSize3  = 2.0f;
static const float WaterSize4  = 4.0f;

// Each wave layer moves at different speeds so it animates in a varying manner
static const float WaterSpeed1 = 0.5f;
static const float WaterSpeed2 = 1.0f;
static const float WaterSpeed3 = 1.7f;
static const float WaterSpeed4 = 2.6f;

// To get the correct wave height, must specify the height/normal map dimensions exactly. Assuming square normal maps
static const float HeightMapHeightOverWidth = 1 / 32.0f; // Maximum height of height map compared to its width, this is effectively embedded in the normals
                                                         // The normal maps for this lab have been created at this level. Used free AwesomeBump software
static const float WaterWidth = 400.0f; // World space width of water surface (size of grid created when creating model in cpp file)
static const float MaxWaveHeight = WaterWidth * HeightMapHeightOverWidth; // The above two values determine the basic wave height in world units


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// All these variables are created & manipulated in the C++ code and passed into the shader here

// Viewport size in pixels
float2 ViewportSize;

// The matrices for transforming from 3D mesh to 2D projection
float4x4 WorldMatrix;
float4x4 ViewMatrix;
float4x4 ProjectionMatrix;
float4x4 CameraMatrix; // Camera matrix, used to correctly distort the reflection and refraction relative to the screen

// Light data
float3 Light1Position;
float3 Light2Position;
float3 Light1Colour;
float3 Light2Colour;
float3 AmbientColour;
float  SpecularPower;
float3 CameraPosition; // used for specular lighting

// Variable used to tint each light model to show the colour that it emits
float3 TintColour;

// Miscellaneous water variables
float  WaterPlaneY;   // Y coordinate of the water plane (before adding the height map)
float2 WaterMovement; // An offset added to the water height map UVs to make the water surface move
float  WaveScale;     // How tall the waves are (rescales weight heights and normals)


// Diffuse texture map & normal/height map
Texture2D DiffuseMap;
Texture2D NormalHeightMap;

// Water depth buffer, reflection and refraction texture maps
Texture2D WaterHeightMap;
Texture2D RefractionMap;
Texture2D ReflectionMap;

// Samplers to use with the above texture maps
// Standard tri-linear filtering (bilinear filtering + linear filtered mip-maps)
SamplerState TrilinearWrap
{
  Filter = MIN_MAG_MIP_LINEAR;
  AddressU = Wrap;
  AddressV = Wrap;
};

// Bilinear filtering, no mip-maps and mirrored at the edges.
// Used for reflection and refraction maps, mip-maps would be costly to make
// at run time, and we don't want to wrap around when near the edge of the screen
SamplerState BilinearMirror
{
  Filter = MIN_MAG_LINEAR_MIP_POINT;
  MaxLOD = 0; // Switches off mip-maps (although the water textures weren't created with them in any case)
  AddressU = Mirror;
  AddressV = Mirror;
};


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

// Standard vertex data to be sent into vertex shaders - the mesh data
struct VS_INPUT
{
  float3 Position : POSITION;
  float3 Normal   : NORMAL;
	float2 UV       : TEXCOORD0;
};


// Data sent to pixel shaders that only do texturing - output from the related vertex shader
struct PS_TEX_INPUT
{
  float4 ProjPosition : SV_POSITION; // 2D "projected" position + depth for pixel rendering
  float2 UV           : TEXCOORD0;
};

// The data sent to pixel shaders that perform lighting and texturing - output from the related vertex shader
struct PS_LIGHTING_TEX_INPUT
{
  float4 ProjPosition  : SV_POSITION; // 2D "projected" position + depth for pixel rendering
	float3 WorldPosition : POSITION;    // World position for lighting calculations
	float3 WorldNormal   : NORMAL;
  float2 UV            : TEXCOORD0;
};

// The data sent to the water surface pixel shaders
struct PS_WATER_INPUT
{
  float4 ProjPosition  : SV_POSITION;
	float3 WorldPosition : POSITION;
  float2 UV            : TEXCOORD0;
};



//**************************************************************************************
// Standard shaders
//**************************************************************************************

//--------------------------------------------------------------------------------------
// Standard Vertex Shaders
//--------------------------------------------------------------------------------------

// Basic vertex shader to transform 3D model vertices to 2D and pass UVs to the pixel shader
PS_TEX_INPUT BasicVS( VS_INPUT input )
{
	PS_TEX_INPUT output;
	
	// Transform the input model vertex position into world, then view then projection space
	float4 modelPosition = float4(input.Position, 1.0f); // Add 4th element to position
	float4 worldPosition = mul( modelPosition, WorldMatrix      );
	float4 viewPosition  = mul( worldPosition, ViewMatrix       );
	output.ProjPosition  = mul( viewPosition,  ProjectionMatrix );
	
	// Pass texture coordinates (UVs) on to the pixel shader
	output.UV = input.UV;

	return output;
}


// Standard vertex shader for models that use per-pixel lighting in the pixel shader
PS_LIGHTING_TEX_INPUT PixelLightingVS( VS_INPUT input )
{
	PS_LIGHTING_TEX_INPUT output;

	// Add 4th element to position and normal
	float4 modelPosition = float4(input.Position, 1.0f);
	float4 modelNormal   = float4(input.Normal,   0.0f);

  // Transform model vertex position and normal to world space
	float4 worldPosition = mul( modelPosition, WorldMatrix );
	float3 worldNormal   = mul( modelNormal,   WorldMatrix );

	// Pass world space position & normal to pixel shader for lighting calculations
  output.WorldPosition = worldPosition.xyz;
	output.WorldNormal   = worldNormal;

	// Use camera matrices to further transform the vertex from world space into view space (camera's point of view) and finally into 2D "projection" space for rendering
	float4 viewPosition  = mul( worldPosition, ViewMatrix       );
	output.ProjPosition  = mul( viewPosition,  ProjectionMatrix );

	// Pass texture coordinates (UVs) on to the pixel shader
	output.UV = input.UV;

	return output;
}


//--------------------------------------------------------------------------------------
// Standard Pixel Shaders
//--------------------------------------------------------------------------------------

// A pixel shader that only outputs a texture
float4 TexOnlyPS( PS_TEX_INPUT input ) : SV_Target
{
	// Extract diffuse material colour for this pixel from a texture
	float4 diffuseMaterial = DiffuseMap.Sample( TrilinearWrap, input.UV );

	return diffuseMaterial;
}


// A pixel shader that just tints a (diffuse) texture with a fixed colour
float4 TintTexPS( PS_TEX_INPUT input ) : SV_Target
{
	// Extract diffuse material colour for this pixel from a texture
	float4 diffuseMaterial = DiffuseMap.Sample( TrilinearWrap, input.UV );

	// Tint by a fixed colour
	diffuseMaterial.rgb *= TintColour;

	return diffuseMaterial;
}

// Per-pixel lighting pixel shader with diffuse and specular maps
float4 PixelLightingPS( PS_LIGHTING_TEX_INPUT input ) : SV_Target
{
	// By this point normals have been transformed by the world matrix, which might contain scaling, and interpolated by the
	// rasterizer stage, which also might introduce scaling. In other words they are not likely length 1 any more, so renormalise
	float3 worldNormal = normalize(input.WorldNormal); 

	///////////////////////
	// Calculate lighting

	// Calculate vector to camera from world position of current pixel
	float3 normalToCamera = normalize(CameraPosition - input.WorldPosition);
	
	// Light 1
	float3 vectorToLight = Light1Position - input.WorldPosition; // Vector to light from current pixel
  float3 normalToLight = normalize(vectorToLight);
	float3 halfwayVector = normalize(normalToLight + normalToCamera);
	float3 diffuseLight1  = Light1Colour  * max( dot(worldNormal, normalToLight), 0 ) / length(vectorToLight);
	float3 specularLight1 = diffuseLight1 * pow( max( dot(worldNormal, halfwayVector), 0 ), SpecularPower );

	// Light 2
	vectorToLight = Light2Position - input.WorldPosition;
  normalToLight = normalize(vectorToLight);
	halfwayVector = normalize(normalToLight + normalToCamera);
	float3 diffuseLight2  = Light2Colour  * max( dot(worldNormal, normalToLight), 0 ) / length(vectorToLight);
	float3 specularLight2 = diffuseLight2 * pow( max( dot(worldNormal, halfwayVector), 0 ), SpecularPower );

	// Sum the effect of the two lights and add an ambient light level (to the diffuse)
	float3 diffuseLight  = AmbientColour + diffuseLight1 + diffuseLight2;
	float3 specularLight = specularLight1 + specularLight2;


	////////////////////
	// Sample texture

	// Extract diffuse material colour for this pixel from a texture
	float4 diffuseMaterial = DiffuseMap.Sample( TrilinearWrap, input.UV );
	
	// Get specular material colour from texture alpha
	float3 specularMaterial = diffuseMaterial.a;

	
	////////////////////
	// Combine colours 
	
	// Combine maps and lighting for final pixel colour
	float4 combinedColour;
	combinedColour.rgb = diffuseMaterial * diffuseLight + specularMaterial * specularLight;
	combinedColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

	return combinedColour;
}



//**************************************************************************************
//**************************************************************************************
// Water shaders
//**************************************************************************************
//**************************************************************************************

//--------------------------------------------------------------------------------------
// Water Vertex Shaders
//--------------------------------------------------------------------------------------

// Vertex shader that distorts the water surface (which is a fine grid - tessellation would be better)
// Also sends data for pixel lighting as the water uses specular lighting
PS_WATER_INPUT WaterVS( VS_INPUT input )
{
	PS_WATER_INPUT output;

	// Add 4th element to position
	float4 modelPosition = float4(input.Position, 1.0f);

  // Sample the height at this point on the water's surface. Sample at four different sizes and combine to give complex waves
  // All UVs are moving, different speeds for each size
  // TODO - STAGE 6: Add vertex displacement to get bumpy water
  //                 You need to sample the alpha channel of NormalHeightMap four times and add them all together. Look at how the
  //                 normals are sampled in the WaterSurfacePS pixel shader just below. Use the same process here except:
  //                 - Use SampleLevel instead of Sample, and add an extra final parameter of 0 (this is needed to access textures in a vertex shader)
  //                 - Grab the alpha channel, not the rgb (normals and heights stored in same texture - same as parallax mapping)
  //                 - Don't do the * 2 - 1 part, that converts rgb to xyz for normals, here the 0->1 range of alpha is already OK
  //                 - Add all the alphas together, the line after will do the averaging and scaling to the final height
  float2 waterUV = input.UV;
  float3 normal1 = NormalHeightMap.SampleLevel(TrilinearWrap, WaterSize1 * (waterUV + WaterMovement * WaterSpeed1),0).a;
  float3 normal2 = NormalHeightMap.SampleLevel(TrilinearWrap, WaterSize2 * (waterUV + WaterMovement * WaterSpeed2),0).a;
  float3 normal3 = NormalHeightMap.SampleLevel(TrilinearWrap, WaterSize3 * (waterUV + WaterMovement * WaterSpeed3),0).a;
  float3 normal4 = NormalHeightMap.SampleLevel(TrilinearWrap, WaterSize4 * (waterUV + WaterMovement * WaterSpeed4),0).a;
  float height = (normal1 + normal2 + normal3 + normal4); // Not 2, read comment above
  // Average heights and add to water y-coordinate
  modelPosition.y += (0.25f * height - 0.5f) * MaxWaveHeight * WaveScale; // -0.5 makes wave movement an equal amount up or down from basic water height

  // Transform new water vertex position to world space and send to pixel shader
	float4 worldPosition = mul( modelPosition, WorldMatrix );
  output.WorldPosition = worldPosition.xyz;

	// Use camera matrices to further transform the vertex from world space into view space and finally into 2D "projection" space
	float4 viewPosition  = mul( worldPosition, ViewMatrix       );
	output.ProjPosition  = mul( viewPosition,  ProjectionMatrix );

	// Pass texture coordinates (UVs) on to the pixel shader
	output.UV = input.UV;

	return output;
}


//--------------------------------------------------------------------------------------
// Water Pixel Shaders
//--------------------------------------------------------------------------------------

// Water height pixel shader, stores for each screen pixel the world y-coordinate of the water at that point
float WaterHeightPS( PS_WATER_INPUT input ) : SV_Target
{
  // Calculation was done in vertex shader already
  return input.WorldPosition.y;
}


// Water surface pixel shader, combines refraction, reflection and specular lighting
float4 WaterSurfacePS( PS_WATER_INPUT input ) : SV_Target
{
  // Sample the normal at this point on the water's surface. Sample at four different sizes and combine to give complex waves
  // All UVs are moving, different speeds for each size. Normals are stored in rgb 0->1 range, so they are changed to xyz -1->1 range (* 2 - 1)
  float2 waterUV = input.UV;
  float3 normal1 = NormalHeightMap.Sample( TrilinearWrap, WaterSize1 * (waterUV + WaterMovement * WaterSpeed1) ).rgb * 2.0f - 1.0f;
  float3 normal2 = NormalHeightMap.Sample( TrilinearWrap, WaterSize2 * (waterUV + WaterMovement * WaterSpeed2) ).rgb * 2.0f - 1.0f;
  float3 normal3 = NormalHeightMap.Sample( TrilinearWrap, WaterSize3 * (waterUV + WaterMovement * WaterSpeed3) ).rgb * 2.0f - 1.0f;
  float3 normal4 = NormalHeightMap.Sample( TrilinearWrap, WaterSize4 * (waterUV + WaterMovement * WaterSpeed4) ).rgb * 2.0f - 1.0f;
  
  // When sampling the water at different sizes, the normals change because we are not changing the height at each size, so correct the
  // normals for that. Alternative is to leave this out and scale the heights used in the vertex shader. This approach gives choppier waves.
  normal1.y *= WaterSize1;
  normal2.y *= WaterSize2;
  normal3.y *= WaterSize3;
  normal4.y *= WaterSize4;

  normal1 = normalize(normal1);
  normal2 = normalize(normal2);
  normal3 = normalize(normal3);
  normal4 = normalize(normal4);


  // Have to renormalize all the normals because of the y scaling above, also swap the y and z axes because normal maps point down the z axis, water points up the y axis.
  // TODO - STAGE 2 - Blend four normals together to make complex waves
  //                  Look at the lines above and read the comments, four normals have been sampled from a normal map. Steps required:
  //                  - Each normal has been scaled differently, so each normal needs to be renormalised
  //                  - Average all the normals to a single normal "waterNormal"
  //                  - Swap the z and the y axes of waterNormal because normal maps point down z, but the water points up the y axis
  float3 waterNormal = (normal1+normal2+normal3+normal4)/4; // Not this, read comment above
  float tempHolder = waterNormal.y;
  waterNormal.y = waterNormal.z;
  waterNormal.z = tempHolder;
  waterNormal.y /= (WaveScale + 0.001f); // User control of wave height also affects normals
  waterNormal = normalize(waterNormal);  // Final normalization for above line

  // Doing correct reflection and refraction from a bumpy surface is difficult without going into full raytracing-like solutions. The
  // usual approximation is to use the xz from the water surface normal to distort the UVs into the reflection and refractions (wiggle!).
  // The idea is that flat water has a normal that points straight up, i.e. x and z are 0, which gives you 0 distortion. All the code
  // examples and discussions I could find only use first line of code below. But that's incomplete, the water normal is in world space,
  // but the reflection and refraction textures are in camera space. The second line of code corrects for that, it's a 2D matrix multiply.
  // The reason people get away without the correct version is because there are so many approximations going on here and the distortion 
  // due to water so complex anyway, that one missing step doesn't make a noticable difference...
  // - Try setting offsetDir = waterNormal2D;  instead of the complex multiplication
  float2 waterNormal2D = waterNormal.xz;
  float2 offsetDir = float2(dot(waterNormal2D, normalize(CameraMatrix[0].xz)), dot(waterNormal2D, normalize(CameraMatrix[2].xz)));

  // Sample the depth to the refracted scene at this pixel and the height to the refected scene (the textures rendered in earlier passes)
  float2 screenUV = input.ProjPosition.xy / ViewportSize;
  float4 refractionDepth  = RefractionMap.Sample( BilinearMirror, screenUV ).a;
  float4 reflectionHeight = ReflectionMap.Sample( BilinearMirror, screenUV ).a;

  // Distort the UVs in screen space based on the distance travelled by the light and the the offset direction from the surface
  // normal. This is an approximation, not physically accurate. When light is bent due to reflection/refraction then the further
  // it travels, the more offset the object we end up seeing at that point. Most games don't account for this and just have a
  // fixed maximum offset. The advantage of this method is that objects deep underwater are much more distorted. The disadvantage
  // is that it is more likely to try and sample offscreen.
  float2 refractionUV = screenUV + RefractionDistortion * refractionDepth  * offsetDir / input.ProjPosition.w;
  // TODO - STAGE 3: Get reflection distortion working
  //                 The normals sampled in the previous stage allow us to distort the relflection and refraction. It's working
  //                 for refraction, but the line below needs to be written to make reflection distortion work. A simple task,
  //                 the process is exactly the same as the refraction line. Check it is working when you're done
  float2 reflectionUV = screenUV + ReflectionDistortion * reflectionHeight * offsetDir / input.ProjPosition.w; // Needs more code on this line, see comment above
  float4 refractColour = RefractionMap.Sample( BilinearMirror, refractionUV ) * RefractionStrength; 
  float4 reflectColour = ReflectionMap.Sample( BilinearMirror, reflectionUV ) * ReflectionStrength;

  // Fade out reflections at water's edge to avoid errors from using a planar approximation to a bumpy surface
  reflectColour = lerp(refractColour, reflectColour, saturate(refractionDepth * MaxDistortionDistance / (0.5f * MaxWaveHeight * WaveScale)));

  // Specular lighting calculation. As the water reflects the sky and the lights, then strictly speaking specular light is not
  // needed, because it is just an approximation for the reflection of the light. However, without implementing HDR (high dynamic
  // range) lighting the reflected lights are quite dim, so use the specular lighting equations to get a stronger effect.
  float3 normalToCamera = normalize(CameraPosition - input.WorldPosition);

  // Light 1
	float3 vectorToLight = Light1Position - input.WorldPosition; 
  float3 normalToLight = normalize(vectorToLight);
	float3 halfwayVector = normalize(normalToLight + normalToCamera);
	float3 specularLight1 = Light1Colour * pow( max( dot(waterNormal, halfwayVector), 0 ), SpecularPower ) / length(vectorToLight);

	// Light 2
	// TODO - STAGE 5: Add specular light to enhance the reflected lights
	//                 Two lights in this scene, specular from first big light is calculated above. Do the same specular
	//                 calculation for the small second light here. Ensure you can see the result
	float3 specularLight2 = Light2Colour * pow(max(dot(waterNormal, halfwayVector), 0), SpecularPower) / length(vectorToLight); // Not 0, read comment above

	// Add the effect of the two lights into the reflected colour
	reflectColour.rgb += SpecularStrength * (specularLight1 + specularLight2);

  // Fresnel effect: the reflected and refracted light is blended based on angle of viewer to surface normal. A glancing angle
  // gives more reflection, straight down into water gives more refraction  
	// TODO - STAGE 4: Implement the Fresnel equation
	//                 Find the Fresnel approximation in the lecture notes and implement here. You will need waterNormal and the
	//                 normal to the camera. Result should go in the "fresnel" variable below. When correct you should get strong
	//                 reflection at a glancing angle and little reflection when viewing straight down
  float n1 = 1.0; // Refractive index of air
  float n2 = WaterRefractiveIndex; 
  //...
  float F0 = pow((n1 - n2) / (n1 + n2),2);
  float fresnel = F0+(1.0f-F0)*pow(1-dot(waterNormal, normalToCamera),5); // Not 0.25, read the comment above
  return lerp(refractColour, reflectColour, fresnel);
}


//--------------------------------------------------------------------------------------
// Reflected and refracted versions of standard shaders
//--------------------------------------------------------------------------------------

// Basic vertex shader that also outputs world position needed for reflection/refraction depth calculations
PS_WATER_INPUT BasicReflectRefractVS( VS_INPUT input )
{
	PS_WATER_INPUT output;
	
	// Transform the input model vertex position into world, then view then projection space
	float4 modelPosition = float4(input.Position, 1.0f); // Add 4th element to position
	float4 worldPosition = mul( modelPosition, WorldMatrix      );
	output.WorldPosition = worldPosition;
	float4 viewPosition  = mul( worldPosition, ViewMatrix       );
	output.ProjPosition  = mul( viewPosition,  ProjectionMatrix );
	
	// Pass texture coordinates (UVs) on to the pixel shader
	output.UV = input.UV;

	return output;
}


// Per-pixel lighting for refracted objects - only renders below water
float4 PixelLightingRefractionPS( PS_LIGHTING_TEX_INPUT input ) : SV_Target
{
  // Sample the height map of the water to find if this pixel is underwater
  float2 screenUV = input.ProjPosition.xy / ViewportSize;
  float waterHeight = WaterHeightMap.Sample( BilinearMirror, screenUV );
  float objectDepth = waterHeight - input.WorldPosition.y;
  clip(objectDepth); // Remove pixels with negative depth - i.e. above the water

  // Get the basic colour for this pixel by calling the standard pixel-lighting shader
  float3 sceneColour = PixelLightingPS(input).rgb;

  // Darken the colour based on the depth underwater
  // TODO - STAGE 1: Darken deep water
  //                 Use the < and > keys to adjust the water height, we would like objects going deep underwater to darken and
  //                 become blue as happens will real water. The second line below (refractionColour = ...) does the colour tint, 
  //                 but first you need to calculate a factor of how much to darken. There is a constant "WaterExtinction" at the
  //                 top of the file. It sets how many metres red, blue and green light can travel in water. We also have the
  //                 depth underwater of the current pixel in the variable "objectDepth":
  //                 - Formula needed in line below is simple, object depth divided by water extinction level
  //                 - However, ensure the result does not exceed 1. There is a HLSL function to do this better than an if, but
  //                   use an "if" if you don't remember it.
  //                 When finished ensure objects underwater darken to blue
  float3 depthDarken = saturate(objectDepth / WaterExtinction);
 
  float3 refractionColour = lerp(sceneColour, normalize(WaterExtinction) * WaterDiffuseLevel, depthDarken);

  // Store the darkened colour in rgb and a value representing how deep the pixel is in alpha (used for refraction distortion)
  // This is written to a 8-bit RGBA texture, so this alpha depth value has limited accuracy
  return float4(refractionColour, saturate(objectDepth / MaxDistortionDistance)); // Alpha ranges 0->1 for depths of 0 to MaxDistortionDistance
}


// Per-pixel lighting for reflection - only renders above water
float4 PixelLightingReflectionPS( PS_LIGHTING_TEX_INPUT input ) : SV_Target
{
  // Sample the height map of the water to find if this pixel is above the water
  float2 screenUV = input.ProjPosition.xy / ViewportSize;
  float waterHeight = WaterHeightMap.Sample( BilinearMirror, screenUV );
  float objectHeight = input.WorldPosition.y - (2 * WaterPlaneY - waterHeight); // Invert the bumps on the water water surface when calculating effective
                                                                                // height (downwards!) of this pixel in the reflection. This is a cheat
                                                                                // to enable us to use a simple planar reflection on a bumpy surface
  clip(objectHeight); // Remove pixels with negative height - i.e. below the water

  // Store the (reflected) scene colour in rgb and a value representing how high the pixel is in alpha (used for
  // reflection distortion). Written to a 8-bit RGBA texture, so this alpha height value has limited accuracy
  return float4(PixelLightingPS(input).rgb, saturate(objectHeight / MaxDistortionDistance)); // Alpha ranges 0->1 for heights of 0 to MaxDistortionDistance
}


// Tinted blended texture shader for refraction - only renders below water - see PixelLightingRefractionPS above for detail
float4 TintTexRefractionPS( PS_WATER_INPUT input ) : SV_Target
{
  float2 screenUV = input.ProjPosition.xy / ViewportSize;
  float waterHeight = WaterHeightMap.Sample( BilinearMirror, screenUV );
  float objectDepth = waterHeight - input.WorldPosition.y;
  clip(objectDepth); // Remove pixels with negative depth - i.e. above the water

 	// Extract diffuse material colour for this pixel from a texture and tint
	float3 diffuseMaterial = DiffuseMap.Sample( TrilinearWrap, input.UV );
	clip(dot(diffuseMaterial, diffuseMaterial) - 0.1f); // Refracted blended pixels have issues especially when completely see through (level of distortion is incorrect) 
                                                      // so clip pixels that are close to transparent in reflections/refractions
  diffuseMaterial *= TintColour;

  // Darken the colour based on the depth underwater
  float3 depthDarken = saturate(objectDepth / WaterExtinction);
  float3 refractionColour = lerp(diffuseMaterial, normalize(WaterExtinction) * WaterDiffuseLevel, depthDarken);

  // Return colour and depth of reflected pixel
  return float4(refractionColour, saturate(objectDepth / MaxDistortionDistance));
}

// Tinted blended texture shader for reflection - only renders above water - see PixelLightingReflectionPS above for detail
float4 TintTexReflectionPS( PS_WATER_INPUT input ) : SV_Target
{
  float2 screenUV = input.ProjPosition.xy / ViewportSize;
  float waterHeight = WaterHeightMap.Sample( BilinearMirror, screenUV );
  float objectHeight = input.WorldPosition.y - (2 * WaterPlaneY - waterHeight);
  clip(objectHeight); // Remove pixels with negative height - i.e. below the water

 	// Extract diffuse material colour for this pixel from a texture and tint
	float3 diffuseMaterial = DiffuseMap.Sample( TrilinearWrap, input.UV );
	clip(dot(diffuseMaterial, diffuseMaterial) - 0.1f); // Reflected blended pixels have issues especially when completely see through (level of distortion is incorrect) 
                                                      // so clip pixels that are close to transparent in reflections/refractions
	diffuseMaterial *= TintColour;

  // Return colour and height of reflected pixel
  return float4(diffuseMaterial, saturate(objectHeight / MaxDistortionDistance));
}



//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------

RasterizerState CullNone  // Cull none of the polygons, i.e. show both sides
{
	CullMode = None;
};
RasterizerState CullBack  // Cull back side of polygon - normal behaviour, only show front of polygons
{
	CullMode = Back;
};
RasterizerState CullBackReflected  // Cull back side of polygon - in reflected view
{
	CullMode = Back;
  FrontCounterClockwise = true; // Front facing polygons have counter clockwise points when reflected
};

RasterizerState Wireframe
{
	CullMode = None;
	FillMode = WIREFRAME;
};



DepthStencilState DepthWritesOff // Don't write to the depth buffer - polygons rendered will not obscure other polygons
{
	DepthWriteMask = ZERO;
};
DepthStencilState DepthWritesOn  // Write to the depth buffer - normal behaviour 
{
	DepthWriteMask = ALL;
};


BlendState NoBlending // Switch off blending - pixels will be opaque
{
  BlendEnable[0] = FALSE;
};

BlendState AdditiveBlending // Additive blending is used for light models
{
  BlendEnable[0] = TRUE;
  SrcBlend = ONE;
  DestBlend = ONE;
  BlendOp = ADD;
};


//--------------------------------------------------------------------------------------
// Standard Techniques
//--------------------------------------------------------------------------------------

// Plain texture only, two-sided geometry (so it works reflected), no lighting
technique10 Skybox
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, BasicVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, TexOnlyPS() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOff, 0 );
  }
}

// Pixel lighting with textures
technique10 PixelLighting
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, PixelLightingVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, PixelLightingPS() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

// Additive blended texture. No lighting, but uses a global colour tint. Used for light models
technique10 AdditiveTintTex
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, BasicVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, TintTexPS() ) );

		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOff, 0 );
  }
}


//--------------------------------------------------------------------------------------
// Reflected and Refracted Versions of Standard Techniques
//--------------------------------------------------------------------------------------

// Pixel lighting with textures, viewed as refraction
technique10 PixelLightingRefraction
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, PixelLightingVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, PixelLightingRefractionPS() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


// Pixel lighting with textures, viewed as reflection
technique10 PixelLightingReflection
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, PixelLightingVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, PixelLightingReflectionPS() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBackReflected ); // Front facing polygons have counter clockwise points when reflected
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


// Tinted additive blended textures, viewed as refraction
technique10 AdditiveTintTexRefraction
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, BasicReflectRefractVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, TintTexRefractionPS() ) );

		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOff, 0 );
  }
}

// Tinted additive blended textures, viewed as reflection
technique10 AdditiveTintTexReflection
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, BasicReflectRefractVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, TintTexReflectionPS() ) );

		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone );  // No need to use CCW points as in the pixel lighting shader because this is two-sided geometry
		SetDepthStencilState( DepthWritesOff, 0 );
  }
}


//--------------------------------------------------------------------------------------
// Water Surface Techniques
//--------------------------------------------------------------------------------------

// Render heights for water surface (world y-coordinate at each pixel)
technique10 WaterHeight
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, WaterVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, WaterHeightPS() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

// Water surface rendering, reflection and refraction
technique10 WaterSurface
{
  pass P0
  {
    SetVertexShader( CompileShader( vs_4_0, WaterVS() ) );
    SetGeometryShader( NULL );                                   
    SetPixelShader( CompileShader( ps_4_0, WaterSurfacePS() ) );

		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}