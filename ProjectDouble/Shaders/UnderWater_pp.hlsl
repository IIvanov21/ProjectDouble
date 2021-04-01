//--------------------------------------------------------------------------------------
// Under Water Post-Processing Pixel Shader
//--------------------------------------------------------------------------------------
// Just samples a pixel from the scene texture and applies wiggle effect to create underwater feeling to the scene

#include "Common.hlsli"


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// The scene has been rendered to a texture, these variables allow access to that texture
Texture2D    SceneTexture : register(t13);
SamplerState PointSample  : register(s5); // We don't usually want to filter (bilinear, trilinear etc.) the scene texture when
										  // post-processing so this sampler will use "point sampling" - no filtering


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Post-processing shader that tints the scene texture to a given colour
float4 main(PostProcessingInput input) : SV_Target
{
	// Sample a pixel from the scene texture and multiply it with the tint colour (comes from a constant buffer defined in Common.hlsli)
	float3 tint = float3( 0.0f, 0.3f, 1.0f );

    input.sceneUV.y += cos(input.sceneUV.x * 25.0f) * 0.009f * cos(wiggle);
    input.sceneUV.x += sin(input.sceneUV.y * 25.0f) * 0.009f * sin(wiggle);
    float3 finalColour = SceneTexture.Sample(PointSample, input.sceneUV).rgb * tint;
	// Got the RGB from the scene texture, set alpha to 1 for final output
	return float4(finalColour, 1.0f);
}