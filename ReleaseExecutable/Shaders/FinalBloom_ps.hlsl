#include "common.hlsli"

// The scene has been rendered to a texture, these variables allow access to that texture
Texture2D SceneTexture : register(t13);
SamplerState PointSample : register(s5); // We don't usually want to filter (bilinear, trilinear etc.) the scene texture when
										  // post-processing so this sampler will use "point sampling" - no filtering
Texture2D ATexture : register(t10);
Texture2D BTexture : register(t11);
Texture2D CTexture : register(t12);




float4 main(BlurInput input) : SV_TARGET
{
    float3 finalColour=0;
    float3 colourMain = SceneTexture.Sample(PointSample, input.uv);
    float3 colourA = ATexture.Sample(PointSample, input.uv);
    float3 colourB = BTexture.Sample(PointSample, input.uv);

    
    finalColour = colourMain.rgb + colourA.rgb + colourB.rgb;
    return float4(finalColour, 1.0f);
}