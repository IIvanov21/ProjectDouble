#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------


SimplePixelShaderInput main(BasicVertex modelVertex)
{
	SimplePixelShaderInput output; 


	float4 modelPosition = float4(modelVertex.position, 1);

	
	float4 worldPosition = mul(gWorldMatrix, modelPosition);
	float4 viewPosition = mul(gViewMatrix, worldPosition);
	output.projectedPosition = mul(gProjectionMatrix, viewPosition);

	
	output.uv = modelVertex.uv;

	return output; 
}