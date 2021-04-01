//--------------------------------------------------------------------------------------
// Pixel shader for non-lit geometry (the lights) in refraction
//--------------------------------------------------------------------------------------
// Simple tinted texture on refracted objects - only renders below water
// The equivalent of the TintedTexture_ps file but with additional work for refraction

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
	float objectDepth = waterHeight - input.worldPosition.y;
	clip(objectDepth); // Remove pixels with negative depth - i.e. above the water

	// Extract diffuse material colour for this pixel from a texture and tint
	float3 diffuseMaterial = DiffuseMap.Sample(StandardFilter, input.uv).rgb;
	clip(dot(diffuseMaterial, diffuseMaterial) - 0.1f); // Refracted blended pixels have issues especially when completely see through (makes what is behind them distort too little) 
														// so clip pixels that are close to transparent in reflections/refractions
	diffuseMaterial *= gObjectColour;

	// Darken the colour based on the depth underwater
	float3 depthDarken = saturate(objectDepth / WaterExtinction);
	float3 refractionColour = lerp(diffuseMaterial, normalize(WaterExtinction) * WaterDiffuseLevel, depthDarken);

	// Store the darkened colour in rgb and a value representing how deep the pixel is in alpha (used for refraction distortion)
	// This is written to a 8-bit RGBA texture, so this alpha depth value has limited accuracy
	return float4(refractionColour, saturate(objectDepth / MaxDistortionDistance)); // Alpha ranges 0->1 for depths of 0 to MaxDistortionDistance
}
