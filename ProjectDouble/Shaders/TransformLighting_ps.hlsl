#include "Common.hlsli" // Shaders can also use include files - note the extension

Texture2D    DiffuseMap : register(t0); 
SamplerState TexSampler : register(s0); 



float4 main(LightingPixelShaderInput input) : SV_Target
{
	input.uv += wiggle;//Texture scrolling
	input.worldNormal = normalize(input.worldNormal); 
	float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);
	

	// Light 3
	float3 light4Vector = gLight4Position - input.worldPosition;
	float3 light4Dist = length(light4Vector);
	float3 light4Direction = light4Vector / light4Dist;
	float3 diffuseLight4 = gLight4Colour * max(dot(input.worldNormal, light4Direction), 0) / light4Dist;

	float3 halfway = normalize(light4Direction + cameraDirection);
	float3 specularLight4 = diffuseLight4 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

	float4 textureColour = DiffuseMap.Sample(TexSampler, input.uv);
	float tintColour = 0.2f;
	float3 diffuseMaterialColour = textureColour.g - tintColour; //Tinting to fixed single colour
	float specularMaterialColour = textureColour.a;
	float3 finalColour = gObjectColour *  (gAmbientColour + diffuseLight4 * diffuseMaterialColour + specularLight4 * diffuseMaterialColour );

	return float4(finalColour, 1.0f); 
}