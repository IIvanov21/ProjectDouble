//--------------------------------------------------------------------------------------
// Light Model Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader simply samples a diffuse texture map and tints with a fixed colour sent over from the CPU via a constant buffer

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D    DiffuseMap : register(t0); // A diffuse map is the main texture for a model.
                                        // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState StandardFilter : register(s0); // Filtering used on most textures (trilinear or anisotropic - chosen on the C++ side)


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point
// This shader just samples a diffuse texture map and tints it to a fixed colour
float4 main(SimplePixelShaderInput input) : SV_Target
{
    // Sample diffuse material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    // Ignoring any alpha in the texture, just reading RGB
    float3 diffuseMapColour = DiffuseMap.Sample(StandardFilter, input.uv).rgb;

    // Blend texture colour with fixed per-object colour
    float3 finalColour = (gObjectColour * diffuseMapColour)*gDayCycle;

    return float4(finalColour, 1.0f); // Always use 1.0f for alpha - no alpha blending in this lab
}