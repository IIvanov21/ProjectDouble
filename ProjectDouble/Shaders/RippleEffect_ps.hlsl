#include "Common.hlsli"
Texture2D DiffuseMap : register(t0);
SamplerState InputSampler:register(s0);

float4 main(WorldPositionPixelShaderInput input) : SV_Target
{
    float2 wave=0.0f;
    float2 toPixel = input.projectedPosition.xy - center;
    
    float distance = length(toPixel) * input.uv.x/*z*/;
    float2 direction = normalize(toPixel);
    
    sincos(frequency * distance + phase, wave.x, wave.y);
    //Clamp the distance between 0 and 1 and squares the value
    float falloff = saturate(1 - distance);
    falloff = pow(falloff, 1.0f / spread);
    
    //Calculate new mapping coordinates based on the frequency, center, and amplitude
    float2 uv2 = input.uv.xy + (wave.x * falloff * amplitude) * direction * input.uv.x/*zw*/;
    
    float lighting = lerp(1.0f, 1.0f + wave.x * falloff * 0.2f, saturate(amplitude / 20.0f));
    float3 finalColour = DiffuseMap.Sample(InputSampler, uv2);
    finalColour.rgb *= lighting;
    
    //Place holder
    return float4(finalColour, 1.0f);
}