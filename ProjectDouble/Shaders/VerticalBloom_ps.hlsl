#include "common.hlsli"

// The scene has been rendered to a texture, these variables allow access to that texture
Texture2D SceneTexture : register(t13);
SamplerState PointSample : register(s5); // We don't usually want to filter (bilinear, trilinear etc.) the scene texture when
										  // post-processing so this sampler will use "point sampling" - no filtering

//To understand how the blur works I have followed Raztek's tutorial:http://www.rastertek.com/dx11tut36.html
//To further improve it and get more efficient Gaussian Blur I used Pascal Triangle which already 
//contained calculated kernel values:http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/ 

float4 main(BlurInput input) : SV_TARGET
{
    float weight0, weight1, weight2, weight3, weight4;
    float normalisation;
    float3 finalColour;
	
    weight0 = 924.0f / 4070.0f;
    weight1 = 792.0f / 4070.0f;
    weight2 = 495.0f / 4070.0f;
    weight3 = 220.0f / 4070.0f;
    weight4 = 66.0f / 4070.0f;

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
    //Get only the bright values by multiplying each colour and adding all together
    //This can be calculated in different ways such averaging them or doing a dot product
    float brightness = (SceneColour.r * 0.2126f) + (SceneColour.g * 0.7152) + (SceneColour.b * 0.0722);
    //Pixel size
    float tSize = (1.0f / gViewportHeight);
    
    
    if (brightness > 0.2f)//Select bright values only and apply blur on them
    {
        //Pixels blured in negative Y
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * -4.0f)) * weight4;
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * -3.0f)) * weight3;
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * -2.0f)) * weight2;
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * -1.0f)) * weight1;
        //Main pixel being blured
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * 0.0f)) * weight0 ;
        //Pixels blured in positive Y                                                                    
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * 1.0f)) * weight1 ;
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * 2.0f)) * weight2 ;
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * 3.0f)) * weight3 ;
        finalColour += SceneTexture.Sample(PointSample, input.uv + float2(0.0f, tSize * 4.0f)) * weight4;
    }
    finalColour *= gBlurIncerement;
	//Motion blur effect
    return float4(finalColour, 0.3);
}