#include "Common.hlsli" 


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------


LightingPixelShaderInput main(BasicVertex modelVertex)
{
	LightingPixelShaderInput output; 

	float4 modelPosition = float4(modelVertex.position, 1);

	float4 worldPosition = mul(gWorldMatrix, modelPosition);
	float4 normalVector = float4(modelVertex.position, 0);
	float4 worldNormal = mul(normalVector, gWorldMatrix);
	worldNormal = normalize(worldNormal); 

	worldPosition.x += sin(modelPosition.y + wiggle) * 0.1f;
	worldPosition.y += sin(modelPosition.x + wiggle) * 0.1f;

	float4 viewPosition = mul(gViewMatrix, worldPosition);
	output.projectedPosition = mul(gProjectionMatrix, viewPosition);

	
	float4 modelNormal = float4(modelVertex.normal, 0);      
	output.worldNormal = mul(gWorldMatrix, modelNormal).xyz; 

															
	output.worldPosition = worldPosition.xyz; 
	output.uv = modelVertex.uv;

	return output; 
}


