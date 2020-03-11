#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D DiffuseSpecularMap : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular map (shininess) in the a channel
SamplerState TexSampler      : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic - this is the sampler used for the texture above

Texture2D ShadowMapLight1 : register(t1); // Texture holding the view of the scene from a light
Texture2D ShadowMapLight2 : register(t2);
SamplerState PointClamp   : register(s1); // No filtering for shadow maps (you might think you could use trilinear or similar, but it will filter light depths not the shadows cast...)


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(LightingPixelShaderInput input) : SV_Target
{
		// Slight adjustment to calculated depth of pixels so they don't shadow themselves
		const float DepthAdjust = 0.001f;

	// Normal might have been scaled by model scaling or interpolation so renormalise
	input.worldNormal = normalize(input.worldNormal);
	// Direction from pixel to camera
	float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

	///////////////////////
	// Calculate lighting
	 // Light 1
	float3 light1Vector = gLight1Position - input.worldPosition;
	float3 light1Dist = length(light1Vector);
	float3 light1Direction = light1Vector / light1Dist;
	float3 diffuseLight1 = gLight1Colour * max(dot(input.worldNormal, light1Direction), 0) / light1Dist; // Equations from lighting lecture

	float3 halfway = normalize(light1Direction + cameraDirection);
	float3 specularLight1 = diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference


	// Light 2
	float3 light2Vector = gLight2Position - input.worldPosition;
	float3 light2Dist = length(light2Vector);
	float3 light2Direction = light2Vector / light2Dist;
	float3 diffuseLight2 = gLight2Colour * max(dot(input.worldNormal, light2Direction), 0) / light2Dist;

	halfway = normalize(light2Direction + cameraDirection);
	float3 specularLight2 = diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);





	//Teapot's light
	// Light 3
	float3 light3Vector = gLight3Position - input.worldPosition;
	float3 light3Dist = length(light3Vector);
	float3 light3Direction = light3Vector / light3Dist;
	float3 diffuseLight3 = gLight3Colour * max(dot(input.worldNormal, light3Direction), 0) / light3Dist;

	halfway = normalize(light3Direction + cameraDirection);
	float3 specularLight3 = diffuseLight3 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
	// Light 4
	float3 light4Vector = gLight4Position - input.worldPosition;
	float3 light4Dist = length(light4Vector);
	float3 light4Direction = light4Vector / light4Dist;
	float3 diffuseLight4 = gLight4Colour * max(dot(input.worldNormal, light4Direction), 0) / light4Dist; // Equations from lighting lecture

	halfway = normalize(light4Direction + cameraDirection);
	float3 specularLight4 = diffuseLight4 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

	

	//----------
	// LIGHT 5

	float3 diffuseLight5 = 0; // Initialy assume no contribution from this light
	float3 specularLight5 = 0;

	// Direction from pixel to light
	float3 light5Direction = normalize(gLight5Position - input.worldPosition);

	// Check if pixel is within light cone
	if (dot(gLight5Facing, -light5Direction) > cos(gLight5CosHalfAngle)) //**** TODO: This condition needs to be written as the first exercise to get spotlights working
		   //           As well as the variables above, you also will need values from the constant buffers in "common.hlsli"
	{
		// Using the world position of the current pixel and the matrices of the light (as a camera), find the 2D position of the
		// pixel *as seen from the light*. Will use this to find which part of the shadow map to look at.
		// These are the same as the view / projection matrix multiplies in a vertex shader (can improve performance by putting these lines in vertex shader)
		float4 light5ViewPosition = mul(gLight5ViewMatrix,       float4(input.worldPosition, 1.0f));
		float4 light5Projection = mul(gLight5ProjectionMatrix, light5ViewPosition);

		// Convert 2D pixel position as viewed from light into texture coordinates for shadow map - an advanced topic related to the projection step
		// Detail: 2D position x & y get perspective divide, then converted from range -1->1 to UV range 0->1. Also flip V axis
		float2 shadowMapUV = 0.5f * light5Projection.xy / light5Projection.w + float2(0.5f, 0.5f);
		shadowMapUV.y = 1.0f - shadowMapUV.y;	// Check if pixel is within light cone

		// Get depth of this pixel if it were visible from the light (another advanced projection step)
		float depthFromLight = light5Projection.z / light5Projection.w - DepthAdjust; //*** Adjustment so polygons don't shadow themselves

		// Compare pixel depth from light with depth held in shadow map of the light. If shadow map depth is less than something is nearer
		// to the light than this pixel - so the pixel gets no effect from this light
		if (depthFromLight < ShadowMapLight1.Sample(PointClamp, shadowMapUV).r)
		{
			float3 light5Dist = length(gLight5Position - input.worldPosition);
			diffuseLight5 = gLight5Colour * max(dot(input.worldNormal, light5Direction), 0) / light5Dist; // Equations from lighting lecture
			float3 halfway = normalize(light5Direction + cameraDirection);
			specularLight5 = diffuseLight5 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference
		}
	}
	//Light 2

	float3 diffuseLight6 = 0; // Initialy assume no contribution from this light
	float3 specularLight6 = 0;

	// Direction from pixel to light
	float3 light6Direction = normalize(gLight6Position - input.worldPosition);

	if (dot(gLight6Facing, -light6Direction) > cos(gLight6CosHalfAngle)) //**** TODO: This condition needs to be written as the first exercise to get spotlights working
																	   //           As well as the variables above, you also will need values from the constant buffers in "common.hlsli"
	{
		// Using the world position of the current pixel and the matrices of the light (as a camera), find the 2D position of the
		// pixel *as seen from the light*. Will use this to find which part of the shadow map to look at.
		// These are the same as the view / projection matrix multiplies in a vertex shader (can improve performance by putting these lines in vertex shader)
		float4 light6ViewPosition = mul(gLight6ViewMatrix, float4(input.worldPosition, 1.0f));
		float4 light6Projection = mul(gLight6ProjectionMatrix, light6ViewPosition);

		// Convert 2D pixel position as viewed from light into texture coordinates for shadow map - an advanced topic related to the projection step
		// Detail: 2D position x & y get perspective divide, then converted from range -2->2 to UV range 0->2. Also flip V axis
		float2 shadowMapUV = 0.5f * light6Projection.xy / light6Projection.w + float2(0.5f, 0.5f);
		shadowMapUV.y = 1.0f - shadowMapUV.y;	// Check if pixel is within light cone

												// Get depth of this pixel if it were visible from the light (another advanced projection step)
		float depthFromLight = light6Projection.z / light6Projection.w - DepthAdjust; //*** Adjustment so polygons don't shadow themselves

																					  // Compare pixel depth from light with depth held in shadow map of the light. If shadow map depth is less than something is nearer
																					  // to the light than this pixel - so the pixel gets no effect from this light
		if (depthFromLight < ShadowMapLight2.Sample(PointClamp, shadowMapUV).r)
		{
			float3 light6Dist = length(gLight6Position - input.worldPosition);
			diffuseLight6 = gLight6Colour * max(dot(input.worldNormal, light6Direction), 0) / light6Dist; // Equations from lighting lecture
			float3 halfway = normalize(light6Direction + cameraDirection);
			specularLight6 = diffuseLight6 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference
		}
	}

	// Sum the effect of the lights - add the ambient at this stage rather than for each light (or we will get too much ambient)
	float3 diffuseLight = gAmbientColour + diffuseLight1 + diffuseLight2 + diffuseLight3 + diffuseLight4 + diffuseLight5 + diffuseLight6;
	float3 specularLight = specularLight1 + specularLight2 + specularLight3 + specularLight4 + specularLight5 + specularLight6;


	////////////////////
	// Combine lighting and textures

	// Sample diffuse material and specular material colour for this pixel from a texture using a given sampler that you set up in the C++ code
	float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
	float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
	float specularMaterialColour = textureColour.a;   // Specular material colour in texture A (shininess of the surface)

	// Combine lighting with texture colours
	float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour;

	return float4(finalColour, 1.0f); // Always use 1.0f for output alpha 
}