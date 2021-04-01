#include "Common.hlsli" 

float4 main(SimplePixelShaderInput input) : SV_Target
{
		return pow(input.projectedPosition.z, 20);
}