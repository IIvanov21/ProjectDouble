#include "common.hlsli"

// The scene has been rendered to a texture, these variables allow access to that texture
Texture2D    SceneTexture : register(t13);
SamplerState PointSample  : register(s5); // We don't usually want to filter (bilinear, trilinear etc.) the scene texture when
										  // post-processing so this sampler will use "point sampling" - no filtering

//To understand how the blur works I have followed Raztek's tutorial:http://www.rastertek.com/dx11tut36.html
//To further improve it and get more efficient Gaussian Blur I used Pascal Triangle which already 
//contained calculated kernel values:http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/ 

float4 main(BlurInput input) : SV_TARGET
{
	float weight0, weight1, weight2, weight3, weight4;
	float normalisation;
	float3 finalColour;
	
	weight0 = 924.0f/4070.0f;
	weight1 = 792.0f/4070.0f;
	weight2 = 495.0f/4070.0f;
	weight3 = 220.0f/4070.0f;
	weight4 = 66.0f/4070.0f;

	//Normalised value to average the weights out
	normalisation = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4));

	//Normalise the weights
	weight0 /= normalisation;
	weight1 /= normalisation;
	weight2 /= normalisation;
	weight3 /= normalisation;
	weight4 /= normalisation;

    float3 SceneColour = SceneTexture.Sample(PointSample, input.uv);
    finalColour = float3(0.0f, 0.0f, 0.0f);
    float tSize = (1.0f / gViewportWidth) * gBlurIncerement;
	//Pixels blured in Negative X
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * -4.0f, 0.0f)) * weight4;
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * -3.0f, 0.0f)) * weight3;
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * -2.0f, 0.0f)) * weight2;
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * -1.0f, 0.0f)) * weight1;
	//Main being blured Pixel
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * 0.0f, 0.0f)) * weight0;
	//Pixels blured in Positive X
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * 1.0f, 0.0f)) * weight1;
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * 2.0f, 0.0f)) * weight2;
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * 3.0f, 0.0f)) * weight3;
    finalColour += SceneTexture.Sample(PointSample, input.uv + float2(tSize * 4.0f, 0.0f)) * weight4;
    
	//Motion blur effect applied by decreasing alpha
	return float4(finalColour, 0.3264);
}