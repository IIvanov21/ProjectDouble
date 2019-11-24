//--------------------------------------------------------------------------------------
// Common include file for all shaders
//--------------------------------------------------------------------------------------
// Using include files to define the type of data passed between the shaders


//--------------------------------------------------------------------------------------
// Shader input / output
//--------------------------------------------------------------------------------------

// The structure below describes the vertex data to be sent into the vertex shader.
struct BasicVertex
{
    float3 position : position;
    float3 normal   : normal;
    float2 uv       : uv;
};


// This structure describes what data the lighting pixel shader receives from the vertex shader.
// The projected position is a required output from all vertex shaders - where the vertex is on the screen
// The world position and normal at the vertex are sent to the pixel shader for the lighting equations.
// The texture coordinates (uv) are passed from vertex shader to pixel shader unchanged to allow textures to be sampled
struct LightingPixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    
    float3 worldPosition : worldPosition;   // The world position and normal of each vertex is passed to the pixel...
    float3 worldNormal   : worldNormal;     //...shader to calculate per-pixel lighting. These will be interpolated
                                            // automatically by the GPU (rasterizer stage) so each pixel will know
                                            // its position and normal in the world - required for lighting equations
    
    float2 uv : uv; // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned" to that vertex.
};


struct PixelShaderInput
{
	float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
											// to the pixel shader and so it uses the special semantic "SV_Position"
											// because the shader needs to identify this important information
	float3 diffuseLight  : diffuseLight;
	float3 specularLight : specularLight;
	float2 uv            : uv;
};

// This structure is similar to the one above but for the light models, which aren't themselves lit
struct SimplePixelShaderInput
{
    float4 projectedPosition : SV_Position;
    float2 uv : uv;
};


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

// These structures are "constant buffers" - a way of passing variables over from C++ to the GPU
// They are called constants but that only means they are constant for the duration of a single GPU draw call.
// These "constants" correspond to variables in C++ that we will change per-model, or per-frame etc.

// In this exercise the matrices used to position the camera are updated from C++ to GPU every frame along with lighting information
// These variables must match exactly the gPerFrameConstants structure in Scene.cpp
cbuffer PerFrameConstants : register(b0) // The b0 gives this constant buffer the number 0 - used in the C++ code
{
    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
    float4x4 gViewProjectionMatrix; // The above two matrices multiplied together to combine their effects

    float3   gLight1Position; 
    float    padding1;        
                              
                              
    float3   gLight1Colour;
    float    padding2;



    float3   gLight2Position;
    float    padding3;

    float3   gLight2Colour;
    float    padding4;


    float3   gAmbientColour;
    float    gSpecularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4

    float3   gCameraPosition;
    float    padding5;

	float3	gLight3Position;
	float	padding7;
	float3  gLight3Colour;
	float   padding8;

	float3   gLight4Position;
	float	 padding9;
	float3   gLight4Colour;
	float	 padding10;

	

	float3   gLight5Position;
	float    padding11;


	float3   gLight5Colour;
	float    padding12;

	float3   gLight5Facing;
	float    gLight5CosHalfAngle;
	float4x4 gLight5ViewMatrix;
	float4x4 gLight5ProjectionMatrix;

	float3   gLight6Position;
	float    padding13;

	float3   gLight6Colour;
	float    padding14;

	float3   gLight6Facing;
	float    gLight6CosHalfAngle;
	float4x4 gLight6ViewMatrix;
	float4x4 gLight6ProjectionMatrix;

	float wiggle;
	float lerpCount;
}
// Note constant buffers are not structs: we don't use the name of the constant buffer, these are really just a collection of global variables (hence the 'g')


// If we have multiple models then we need to update the world matrix from C++ to GPU multiple times per frame because we
// only have one world matrix here. Because this data is updated more frequently it is kept in a different buffer for better performance.
// We also keep other data that changes per-model here
// These variables must match exactly the gPerModelConstants structure in Scene.cpp
cbuffer PerModelConstants : register(b1) // The b1 gives this constant buffer the number 1 - used in the C++ code
{
    float4x4  gWorldMatrix;

    float3   gObjectColour;
    float    padding6;  // See notes on padding in structure above
}
