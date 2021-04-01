//--------------------------------------------------------------------------------------
// Pixel shader for lit geometry being reflected
//--------------------------------------------------------------------------------------
// Per-pixel lighting for reflected objects

#include "Common.hlsli"

// Actually include the pixel lighting shader so the code can be shared. Only need the additional work for refraction here. Unusual but works fine. 
#include "PixelLighting_ps.hlsl" 


//--------------------------------------------------------------------------------------
// Texture maps
//--------------------------------------------------------------------------------------

// Note that the texture register numbers are important - slots 0-5 are not used here, but are used in other shaders
// We make sure each map gets a unique slot across all the shaders in use at any given point

// Water height map - each pixel contains the y coordinate of the surface of the water at that point - rendered in an earlier pass
Texture2D WaterHeightMap : register(t2);

SamplerState BilinearMirror : register(s1); // We use mirror mode for the reflection and refraction because pixels off screen might come
                                            // into view due to the water wiggling. Mirror mode will put some vaguely sensible colours there
                                            // although it is a bit of a cheat. An alternative solution is to render the reflection / refraction
                                            // maps larger than they need to be but that adds complexity for little gain


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

float4 main(LightingPixelShaderInput input) : SV_Target
{
  // Sample the height map of the water to find if this pixel is underwater
  float2 screenUV = input.projectedPosition.xy / float2(gViewportWidth, gViewportHeight);
  float waterHeight = WaterHeightMap.Sample(BilinearMirror, screenUV).x;
  float objectHeight = input.worldPosition.y - (2 * gWaterPlaneY - waterHeight); // Invert the bumps on the water water surface when calculating effective
                                                                                 // height (downwards!) of this pixel in the reflection. This is a cheat
                                                                                 // to enable us to use a simple planar reflection on a bumpy surface
  clip(objectHeight); // Remove pixels with negative height - i.e. below the water

  // Get the basic colour for this pixel by calling the standard pixel-lighting shader (included at the top)
  float3 sceneColour = PixelLighting(input).rgb;

  // Store the (reflected) scene colour in rgb and a value representing how high the pixel is in alpha (used for
  // reflection distortion). Written to a 8-bit RGBA texture, so this alpha height value has limited accuracy
  return float4(sceneColour, saturate(objectHeight / MaxDistortionDistance)); // Alpha ranges 0->1 for heights of 0 to MaxDistortionDistance
}
