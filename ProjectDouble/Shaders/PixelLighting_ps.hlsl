//--------------------------------------------------------------------------------------
// Per-Pixel Lighting Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader receives position and normal from the vertex shader and uses them to calculate
// lighting per pixel. Also samples a samples a diffuse + specular texture map and combines with light colour.

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D    DiffuseSpecularMap : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular
Texture2D    DiffuseSpecularMapTwo : register(t1);                                               // map (shininess level) in their alpha channel. Repurposing the alpha channel means we can't use alpha blending
                                                // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(LightingPixelShaderInput input) : SV_Target
{
    // Lighting equations
    input.worldNormal = normalize(input.worldNormal); // Normal might have been scaled by model scaling or interpolation so renormalise
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

    // Light 1
    float3 light1Vector = gLight1Position - input.worldPosition;
    float3 light1Dist = length(light1Vector);
    float3 light1Direction = light1Vector / light1Dist;
    float3 diffuseLight1 = gLight1Colour * max(dot(input.worldNormal, light1Direction), 0) / light1Dist; // Equations from lighting lecture

    float3 halfway = normalize(light1Direction + cameraDirection);
    float3 specularLight1 =  diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference


    // Light 2
    float3 light2Vector = gLight2Position - input.worldPosition;
    float3 light2Dist = length(light2Vector);
    float3 light2Direction = light2Vector / light2Dist;
    float3 diffuseLight2 = gLight2Colour * max(dot(input.worldNormal, light2Direction), 0) / light2Dist;

    halfway = normalize(light2Direction + cameraDirection);
    float3 specularLight2 =  diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);



	

	//Teapot's light
	// Light 3
	float3 light3Vector = gLight3Position - input.worldPosition;
	float3 light3Dist = length(light3Vector);
	float3 light3Direction = light3Vector / light3Dist;
	float3 diffuseLight3 = gLight3Colour * max(dot(input.worldNormal, light3Direction), 0) / light3Dist;

	halfway = normalize(light3Direction + cameraDirection);
	float3 specularLight3 = diffuseLight3 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
	// Light 4
	float3 light4Vector = gLight4Position - input.worldPosition;
	float3 light4Dist = length(light4Vector);
	float3 light4Direction = light4Vector / light4Dist;
	float3 diffuseLight4 = gLight4Colour * max(dot(input.worldNormal, light4Direction), 0) / light4Dist; // Equations from lighting lecture

	halfway = normalize(light4Direction + cameraDirection);
	float3 specularLight4 = diffuseLight4 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); 

	//Light 5
	float3 light5Vector = gLight5Position - input.worldPosition;
	float3 light5Dist = length(light5Vector);
	float3 light5Direction = light5Vector / light5Dist;
	float3 diffuseLight5 = gLight5Colour * max(dot(input.worldNormal, light5Direction), 0) / light5Dist; // Equations from lighting lecture

	halfway = normalize(light5Direction + cameraDirection);
	float3 specularLight5 = diffuseLight5 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

	
	// Light 6
	float3 light6Vector = gLight6Position - input.worldPosition;
	float3 light6Dist = length(light6Vector);
	float3 light6Direction = light6Vector / light6Dist;
	float3 diffuseLight6 = gLight6Colour * max(dot(input.worldNormal, light6Direction), 0) / light6Dist; // Equations from lighting lecture

	halfway = normalize(light6Direction + cameraDirection);
	float3 specularLight6 = diffuseLight6 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

    // Sample diffuse material and specular material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
    float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
    float specularMaterialColour = textureColour.a;   // Specular material colour in texture A (shininess of the surface)

	float3 diffuseMapColourOne = DiffuseSpecularMap.Sample(TexSampler, input.uv).rgb;
	float3 diffuseMapColourTwo = DiffuseSpecularMapTwo.Sample(TexSampler, input.uv).rgb;

    // Combine lighting with texture colours
    float3 finalColour =lerp(diffuseMapColourOne,diffuseMapColourTwo ,0.15f) + (gAmbientColour + diffuseLight1 + diffuseLight2 + diffuseLight3+ diffuseLight4 + diffuseLight5 + diffuseLight6) * diffuseMaterialColour +
                         (specularLight1 + specularLight2 + specularLight3 + specularLight4 + specularLight5 + specularLight6) * specularMaterialColour;
	

    return float4(finalColour, 1.0f); // Always use 1.0f for output alpha - no alpha blending in this lab
}