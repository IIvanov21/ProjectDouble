//--------------------------------------------------------------------------------------
// Simple Vertex Shader
//--------------------------------------------------------------------------------------
// Basic vertex shader that also outputs world position needed for reflection/refraction depth calculations

#include "Common.hlsli"


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

WorldPositionPixelShaderInput main(BasicVertex input)
{
	WorldPositionPixelShaderInput output;
	
	// Transform the input model vertex position into world, then view then projection space
	float4 modelPosition = float4(input.position, 1); // Add 4th element to position

	// Multiply by world, then view, then projection matrix, output positions on the way
	float4 worldPosition = mul(gWorldMatrix, modelPosition);
	output.worldPosition = worldPosition.xyz;
	float4 viewPosition  = mul(gViewMatrix, worldPosition);
	output.projectedPosition  = mul(gProjectionMatrix, viewPosition);
	
	// Pass texture coordinates (UVs) on to the pixel shader
	output.uv = input.uv;

	return output;
}