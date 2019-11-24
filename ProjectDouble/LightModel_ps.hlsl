

#include "Common.hlsli" // Shaders can also use include files - note the extension


Texture2D    DiffuseMap : register(t0); // A diffuse map is the main texture for a model.
                                        // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic



float4 main(SimplePixelShaderInput input) : SV_Target
{
	
    float3 diffuseMapColour = DiffuseMap.Sample(TexSampler, input.uv).rgb;
	float3 finalColour = gObjectColour * diffuseMapColour;

    return float4(finalColour, 1.0f); // Always use 1.0f for alpha - no alpha blending in this lab
}