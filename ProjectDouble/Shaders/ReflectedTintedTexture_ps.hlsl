//--------------------------------------------------------------------------------------
// Pixel shader for non-lit geometry (the lights) in reflection
//--------------------------------------------------------------------------------------
// Simple tinted texture on reflected objects
// The equivalent of the TintedTexture_ps file but with additional work for reflection

#include "Common.hlsli"


//--------------------------------------------------------------------------------------
// Textures maps
//--------------------------------------------------------------------------------------

// Note that the texture register numbers are important - slots 1-5 are not used here, but are used in other shaders
// We make sure each map gets a unique slot across all the shaders in use at any given point

Texture2D DiffuseMap : register(t0);
Texture2D WaterHeightMap : register(t2); // Water height map - each pixel contains the y coordinate of the surface of the water at that point - rendered in an earlier pass

SamplerState StandardFilter : register(s0); // Filtering used on most textures (trilinear or anisotropic - chosen on the C++ side)
SamplerState BilinearMirror : register(s1); // We use mirror mode for the reflection and refraction because pixels off screen might come
                                            // into view due to the water wiggling. Mirror mode will put some vaguely sensible colours there
                                            // although it is a bit of a cheat. An alternative solution is to render the reflection / refraction
                                            // maps larger than they need to be but that adds complexity for little gain

//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

float4 main(WorldPositionPixelShaderInput input) : SV_Target
{
	// Sample the height map of the water to find if this pixel is underwater
	float2 screenUV = input.projectedPosition.xy / float2(gViewportWidth, gViewportHeight);
	float waterHeight = WaterHeightMap.Sample(BilinearMirror, screenUV).x;
	float objectHeight = input.worldPosition.y - (2 * gWaterPlaneY - waterHeight);
	clip(objectHeight); // Remove pixels with negative height - i.e. below the water (see ReflectedPixelLighting_ps for more detailed comments)

	// Extract diffuse material colour for this pixel from a texture and tint
	float3 diffuseMaterial = DiffuseMap.Sample(StandardFilter, input.uv).rgb;
	clip(dot(diffuseMaterial, diffuseMaterial) - 0.1f); // Reflected blended pixels have issues especially when completely see through (makes what is behind them distort too little) 
														// so clip pixels that are close to transparent in reflections/refractions
	diffuseMaterial *= gObjectColour;

	// Return colour and height of reflected pixel
	return float4(diffuseMaterial, saturate(objectHeight / MaxDistortionDistance));
}
