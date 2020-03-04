//--------------------------------------------------------------------------------------
// Scene geometry and layout preparation
// Scene rendering & update
//--------------------------------------------------------------------------------------

#include "Definitions.h"
#include "ModelsAndMeshes.h"
#include "TextureManager.h"
#include <d3d11.h>

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

PerFrameConstants gPerFrameConstants;
ID3D11Buffer*     gPerFrameConstantBuffer;

PerModelConstants gPerModelConstants;
ID3D11Buffer*     gPerModelConstantBuffer;
const float ROTATION_SPEED = 2.0f;
const float MOVEMENT_SPEED = 50.0f;
//--------------------------------------------------------------------------------------
// Scene Data
//--------------------------------------------------------------------------------------




CVector3 gAmbientColour = { 0.2f, 0.2f, 0.3f }; 
float    gSpecularPower = 256; 
ColourRGBA gBackgroundColor = { 0.2f, 0.2f, 0.3f , 1.0f };

//Objects declaration
ModelManager* ModelCreator = new ModelManager();//Models and Meshes controller 
TextureManager* TextureCreator = new TextureManager();//Texture Manager
CMatrix4x4 CalculateLightViewMatrix(Model* light)
{
	return InverseAffine(light->WorldMatrix());
}

CMatrix4x4 CalculateLightProjectionMatrix(Model* light)
{
	return MakeProjectionMatrix(1.0f, ToRadians(ModelCreator->gSpotlightConeAngle)); // Helper function in Utility\GraphicsHelpers.cpp
}



bool InitGeometry()
{
    // Load mesh geometry data, just like TL-Engine this doesn't create anything in the scene. Create a Model for that.
    // IMPORTANT NOTE: Will only keep the first object from the mesh - multipart objects will have parts missing - see later lab for more robust loader
	ModelCreator->LoadMeshes();
	

    // Load the shaders required for the geometry we will use (see Shader.cpp / .h)
    if (!LoadShaders())
    {
        gLastError = "Error loading shaders";
        return false;
    }


    // Create GPU-side constant buffers to receive the gPerFrameConstants and gPerModelConstants structures above
    // These allow us to pass data from CPU to shaders such as lighting information or matrices
    // See the comments above where these variable are declared and also the UpdateScene function
    gPerFrameConstantBuffer = CreateConstantBuffer(sizeof(gPerFrameConstants));
    gPerModelConstantBuffer = CreateConstantBuffer(sizeof(gPerModelConstants));
    if (gPerFrameConstantBuffer == nullptr || gPerModelConstantBuffer == nullptr)
    {
        gLastError = "Error creating constant buffers";
        return false;
    }


    //// Load / prepare textures on the GPU ////

    // Load textures and create DirectX objects for them
    // The LoadTexture function requires you to pass a ID3D11Resource* (e.g. &gCubeDiffuseMap), which manages the GPU memory for the
    // texture and also a ID3D11ShaderResourceView* (e.g. &gCubeDiffuseMapSRV), which allows us to use the texture in shaders
    // The function will fill in these pointers with usable data. The variables used here are globals found near the top of the file.
   

	TextureCreator->LoadTextures();


    //**** Create Portal Texture ****//
	TextureCreator->CreateTextures(gHWnd);
	
	

    
    //*****************************//
 

  	// Create all filtering modes, blending modes etc. used by the app (see State.cpp/.h)
	if (!CreateStates())
	{
		gLastError = "Error creating states";
		return false;
	}

	return true;
}


// Prepare the scene
// Returns true on success
bool InitScene()
{
    //// Set up scene ////

	ModelCreator->CreateModels();
	// Initial positions
	
	////Creating 2D texture////
	ModelCreator->InitialSceneSetup();
	gPerFrameConstants.lerpCount = 0.0f;



	ModelCreator->CreateCameras();

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    ReleaseStates();
	TextureCreator->ReleaseTextures();
    
    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();

    ReleaseShaders();

    // See note in InitGeometry about why we're not using unique_ptr and having to manually delete
	ModelCreator->~ModelManager();
}



//--------------------------------------------------------------------------------------
// Scene Rendering
//--------------------------------------------------------------------------------------

// Render the scene from the given light's point of view. Only renders depth buffer
void RenderDepthBufferFromLight(Model*Light)
{
	// Get camera-like matrices from the spotlight, seet in the constant buffer and send over to GPU
	gPerFrameConstants.viewMatrix = CalculateLightViewMatrix(Light);
	gPerFrameConstants.projectionMatrix = CalculateLightProjectionMatrix(Light);
	gPerFrameConstants.viewProjectionMatrix = gPerFrameConstants.viewMatrix * gPerFrameConstants.projectionMatrix;
	UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
	gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


	//// Only render models that cast shadows ////

	// Use special depth-only rendering shaders
	gD3DContext->VSSetShader(gLightModelVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gDepthOnlyPixelShader, nullptr, 0);

	// States - no blending, normal depth buffer and culling
	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
	gD3DContext->RSSetState(gCullBackState);

	// Render models - no state changes required between each object in this situation (no textures used in this step)
	ModelCreator->gFloor->Render();
	ModelCreator->gTroll->Render();
	ModelCreator->gCrate->Render();
	ModelCreator->gSphere->Render();
	ModelCreator->gCube[0]->Render();
	ModelCreator->gCube[1]->Render();
	ModelCreator->gTeapot->Render();

}
// Render everything in the scene from the given camera
// This code is common between rendering the main scene and rendering the scene in the portal
// See RenderScene function below
void RenderSceneFromCamera(Camera* camera)
{
    // Set camera matrices in the constant buffer and send over to GPU
    gPerFrameConstants.viewMatrix           = camera->ViewMatrix();
    gPerFrameConstants.projectionMatrix     = camera->ProjectionMatrix();
    gPerFrameConstants.viewProjectionMatrix = camera->ViewProjectionMatrix();
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


    //// Render lit models - ground first ////

 

	ModelCreator->PrepareRenderModels(gD3DContext, TextureCreator);
}




// Rendering the scene now renders everything twice. First it renders the scene for the portal into a texture.
// Then it renders the main scene using the portal texture on a model.
void RenderScene()
{

    // Set up the light information in the constant buffer 
    // Don't send to the GPU yet, the function RenderSceneFromCamera will do that
	//Basic information for lighting passed to GPU to calculate Diffuse Lighting, Specular Lighting and Light attenuation
	gPerFrameConstants.light1Colour = ModelCreator->gLights[0].colour * ModelCreator->gLights[0].strength;
	gPerFrameConstants.light1Position = ModelCreator->gLights[0].model->Position();
	gPerFrameConstants.light2Colour = ModelCreator->gLights[1].colour * ModelCreator->gLights[1].strength;
	gPerFrameConstants.light2Position = ModelCreator->gLights[1].model->Position();
	gPerFrameConstants.light3Colour = ModelCreator->gLights[2].colour * ModelCreator->gLights[2].strength;
	gPerFrameConstants.light3Position = ModelCreator->gLights[2].model->Position();
	gPerFrameConstants.light4Colour = ModelCreator->gLights[3].colour * ModelCreator->gLights[3].strength;
	gPerFrameConstants.light4Position = ModelCreator->gLights[3].model->Position();

   //Lights information that cast shadowing 
	gPerFrameConstants.light5Colour = ModelCreator->gLights[4].colour * ModelCreator->gLights[4].strength;
	gPerFrameConstants.light5Position = ModelCreator->gLights[4].model->Position();
	gPerFrameConstants.light5Facing = Normalise(ModelCreator->gLights[4].model->WorldMatrix().GetZAxis());  
	gPerFrameConstants.light5CosHalfAngle = cos(ToRadians(ModelCreator->gSpotlightConeAngle / 2));//It used for the size of the cone that the light creates
	gPerFrameConstants.light5ProjectionMatrix = CalculateLightProjectionMatrix(ModelCreator->gLights[4].model);
	gPerFrameConstants.light5ViewMatrix = CalculateLightViewMatrix(ModelCreator->gLights[4].model);

	gPerFrameConstants.light6Colour = ModelCreator->gLights[5].colour * ModelCreator->gLights[5].strength;
	gPerFrameConstants.light6Position = ModelCreator->gLights[5].model->Position();
	gPerFrameConstants.light6Facing = Normalise(ModelCreator->gLights[5].model->WorldMatrix().GetZAxis());
	gPerFrameConstants.light6CosHalfAngle = cos(ToRadians(ModelCreator->gSpotlightConeAngle / 2));//It used for the size of the cone that the light creates
	gPerFrameConstants.light6ViewMatrix = CalculateLightViewMatrix(ModelCreator->gLights[5].model);
	gPerFrameConstants.light6ProjectionMatrix = CalculateLightProjectionMatrix(ModelCreator->gLights[5].model);

	gPerFrameConstants.ambientColour = gAmbientColour;//Background lighting as static  
	gPerFrameConstants.specularPower = gSpecularPower;
	gPerFrameConstants.cameraPosition = ModelCreator->gCamera->Position();



    //-------------------------------------------------------------------------


    //// Portal scene rendering ////

    // Set the portal texture and portal depth buffer as the targets for rendering
    // The portal texture will later be used on models in the main scene
   gD3DContext->OMSetRenderTargets(1, &TextureCreator->gPortalRenderTarget, TextureCreator->gPortalDepthStencilView);

    // Clear the portal texture to a fixed colour and the portal depth buffer to the far distance
    gD3DContext->ClearRenderTargetView(TextureCreator->gPortalRenderTarget, &gBackgroundColor.r);
    gD3DContext->ClearDepthStencilView(TextureCreator->gPortalDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Setup the viewport for the portal texture size
    D3D11_VIEWPORT vp;
    vp.Width  = static_cast<FLOAT>(TextureCreator->gPortalWidth);
    vp.Height = static_cast<FLOAT>(TextureCreator->gPortalHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);

    // Render the scene for the portal
    RenderSceneFromCamera(ModelCreator->gPortalCamera);




	//***************************************//
	//// Render from light's point of view ////


	// Setup the viewport to the size of the shadow map texture
	

	vp.Width = static_cast<FLOAT>(TextureCreator->gShadowMapSize);
	vp.Height = static_cast<FLOAT>(TextureCreator->gShadowMapSize);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gD3DContext->RSSetViewports(1, &vp);

	
	gD3DContext->OMSetRenderTargets(0, nullptr, TextureCreator->gShadowMap1DepthStencil);
	gD3DContext->ClearDepthStencilView(TextureCreator->gShadowMap1DepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	RenderDepthBufferFromLight(ModelCreator->gLights[4].model);

	// Render the scene from the point of view of light 2 (only depth values written)

	gD3DContext->OMSetRenderTargets(0, nullptr, TextureCreator->gShadowMap2DepthStencil);
	gD3DContext->ClearDepthStencilView(TextureCreator->gShadowMap2DepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	RenderDepthBufferFromLight(ModelCreator->gLights[5].model);


	//**************************//

	//// Main scene rendering ////

	// Set the back buffer as the target for rendering and select the main depth buffer.
	// When finished the back buffer is sent to the "front buffer" - which is the monitor.
	gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, gDepthStencil);

	// Clear the back buffer to a fixed colour and the depth buffer to the far distance
	gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, &gBackgroundColor.r);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Setup the viewport to the size of the main window
	vp.Width = static_cast<FLOAT>(gViewportWidth);
	vp.Height = static_cast<FLOAT>(gViewportHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gD3DContext->RSSetViewports(1, &vp);

	
	gD3DContext->PSSetShaderResources(1, 1, &TextureCreator->gShadowMap1SRV);
	gD3DContext->PSSetShaderResources(2, 1, &TextureCreator->gShadowMap2SRV);
	gD3DContext->PSSetSamplers(1, 1, &gPointSampler);
    

    // Render the scene for the main window
    RenderSceneFromCamera(ModelCreator->gCamera);

	ID3D11ShaderResourceView* nullView = nullptr;
	gD3DContext->PSSetShaderResources(1, 1, &nullView);
    //-------------------------------------------------------------------------


    //// Scene completion ////

    // When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
    gSwapChain->Present(0, 0);
}


//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------

// Update models and camera. frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
	
	ModelCreator->UpdateModels(frameTime, gPerFrameConstants);
    // Show frame time / FPS in the window title //
    const float fpsUpdateTime = 0.5f; // How long between updates (in seconds)
    static float totalFrameTime = 0;
    static int frameCount = 0;
    totalFrameTime += frameTime;
    ++frameCount;
    if (totalFrameTime > fpsUpdateTime)
    {
        // Displays FPS rounded to nearest int, and frame time (more useful for developers) in milliseconds to 2 decimal places
        float avgFrameTime = totalFrameTime / frameCount;
        std::ostringstream frameTimeMs;
        frameTimeMs.precision(2);
        frameTimeMs << std::fixed << avgFrameTime * 1000;
        std::string windowTitle = "Ivaylo Ivanov Graphics Assignment: Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}
