#include "ModelsAndMeshes.h"

ModelManager::ModelManager()
{
	// Additional light information
	CVector3 gLightsPosition2[NUM_LIGHTS] = { {30,30,0},{-20,30,30},{20,30,130},{ 50.0f,60.0f, -30.0f },{50,30,0},{ 50.0f,40.0f, -100.0f } };
	float gLightStrengths2[NUM_LIGHTS] = {10.0f,40.0f,40.0f,20.0f,40.0f,20.0f};
	CVector3 gLightsColours2[NUM_LIGHTS] = { { 0.8f, 0.8f, 1.0f } , { 1.0f, 0.8f, 0.2f },{ 0.5f, 0.8f, 0.2f },{ 1.0f, 0.8f, 0.2f }, { 1.0f, 0.8f, 0.2f },{ 1.0f, 0.8f, 0.2f } };
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		gLightsPosition[i] = gLightsPosition2[i];
		gLightStrengths[i] = gLightStrengths2[i];
		gLightsColours[i] = gLightsColours2[i];
	}
	
	gColourSwitch = false;
	gColourCount = 1;
	gRed = 0.5f;
	gGreen = 0.8f;
	gBlue = 0.2f;
	gColourRate = 2.0f;

	gLightPulsation = false;
	gPulseRate = 10.0f;
	gMinLightStrength = 10.0f;
	gMaxLightStrength = 30.0f;
	gTextureCount = false;
	gSpotlightConeAngle = 90.0f;
}

ModelManager::~ModelManager()
{
	delete gCamera;        gCamera = nullptr;
	delete gPortalCamera;  gPortalCamera = nullptr;

	delete gPortal;  gPortal = nullptr;
	
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		delete gLights[i].model;
		gLights[i].model=nullptr;

	}
	delete gSky; gSky = nullptr;
	delete gTroll;   gTroll = nullptr;;
	delete gGround;  gGround = nullptr;
	delete gSphere;  gSphere = nullptr;
	delete gCrate;   gCrate = nullptr;
	delete gDecal;   gDecal = nullptr;
	delete *gCube;    for (int i = 0; i < kCubeNum; i++)gCube[i] = nullptr;
	delete gTeapot;  gTeapot = nullptr;
	delete gFloor;   gFloor = nullptr;
	delete gWaterHouse; gWaterHouse = nullptr;
	delete gWater;		gWater = nullptr;

	delete gSkyMesh;	gSkyMesh = nullptr;
	delete gPortalMesh;  gPortalMesh = nullptr;
	delete gLightMesh;   gLightMesh = nullptr;
	delete gGroundMesh;  gGroundMesh = nullptr;
	delete gSphereMesh;  gSphereMesh = nullptr;
	delete gCrateMesh;   gCrateMesh = nullptr;
	delete gDecalMesh;   gDecalMesh = nullptr;
	delete gCubeMesh;    gCubeMesh = nullptr;
	delete gFloorMesh;   gFloorMesh = nullptr;
	delete gTeapotMesh;  gTeapotMesh = nullptr;
	delete gTrollMesh; gTrollMesh = nullptr;
	delete gWaterHouseMesh; gWaterHouseMesh = nullptr;
	delete gMainHouseMesh; gMainHouseMesh = nullptr;
	delete gWaterMesh; gWaterMesh = nullptr;
}

bool ModelManager::LoadMeshes()
{
	try
	{
		gCubeMesh = new Mesh(MeshesMediaFolder +"Cube.x");
		gTrollMesh = new Mesh(MeshesMediaFolder + "Troll.x");
		gDecalMesh = new Mesh(MeshesMediaFolder + "Decal.x");
		gCrateMesh = new Mesh(MeshesMediaFolder + "CargoContainer.x");
		gSphereMesh = new Mesh(MeshesMediaFolder + "Sphere.x");
		gGroundMesh = new Mesh(MeshesMediaFolder + "Hills.x");
		gLightMesh = new Mesh(MeshesMediaFolder + "Light.x");
		gPortalMesh = new Mesh(MeshesMediaFolder + "Portal.x");
		gTeapotMesh = new Mesh(MeshesMediaFolder + "Teapot.x");
		gFloorMesh = new Mesh(MeshesMediaFolder + "mount.obj");
		gWaterHouseMesh = new Mesh(MeshesMediaFolder + "waterWheelHouse.fbx");
		gMainHouseMesh = new Mesh(MeshesMediaFolder + "mainHouse.obj");
		gSkyMesh = new Mesh(MeshesMediaFolder + "Skybox.x");
		gWaterMesh = new Mesh(CVector3(-200, 0, -200), CVector3(200, 0, 200), 400, 400, true);
	}
	catch (std::runtime_error e)  
	{
		gLastError = e.what(); 
		return false;
	}
	return true;
}

void ModelManager::CreateModels()
{
	gSky = new Model(gSkyMesh);
	gCube[0] = new Model(gCubeMesh);
	gCube[1] = new Model(gCubeMesh);
	gDecal = new Model(gDecalMesh);
	gCrate = new Model(gCrateMesh);
	gSphere = new Model(gSphereMesh);
	gGround = new Model(gGroundMesh);
	gWaterHouse = new Model(gWaterHouseMesh);
	gMainHouse = new Model(gMainHouseMesh);
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		gLights[i].model = new Model(gLightMesh);
	}
	
	gPortal = new Model(gCubeMesh);
	gPortal2 = new Model(gCubeMesh);
	gTeapot = new Model(gTeapotMesh);
	gFloor = new Model(gFloorMesh);
	gTroll = new Model(gTrollMesh);
	gWater = new Model(gWaterMesh);

}

void ModelManager::InitialSceneSetup()
{
	gSky->SetRotation({ 0, ToRadians(90.0f), 0 });
	gSky->SetScale(10);

	gTeapot->SetPosition({ 10,20,0 });
	gCube[0]->SetPosition({ 0, 15,  0.05f });
	gCube[1]->SetPosition({ 60,5,180 });
	gTroll->SetPosition({ 50,22,20 });
	gTroll->SetScale(10.0f);
	
	//House render
	gWaterHouse->SetPosition({ 140,18,135 });
	gWaterHouse->SetRotation({ 0.0f,ToRadians(150.0f),0.0f });
	gWaterHouse->SetScale(2.0f);
	gMainHouse->SetScale(2.0f);
	gMainHouse->SetPosition({ 50.0f,22.0f, -30.0f });


	gFloor->SetPosition({0.0f, -50.0f, 0.0f});
	gFloor->SetScale(50.0f);
	gDecal->SetPosition({ 0, 15,  0 });
	gSphere->SetPosition({ -10,15,220 });
	gCrate->SetPosition({ -10,  22.0f, 70 });
	gCrate->SetScale(6.0f);
	gCrate->SetRotation({ 0.0f, ToRadians(40.0f), 0.0f });
	gPortal->SetPosition({ 40, 25.0f, 40 });
	gPortal->SetRotation({ 0.0f, ToRadians(-130.0f), 0.0f });
	gPortal2->SetPosition({ 30, 25.0f, 30 });
	gPortal2->SetRotation({ 0.0f, ToRadians(-130.0f), 0.0f });
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		gLights[i].model->SetPosition(gLightsPosition[i]);
		gLights[i].strength = gLightStrengths[i];
		gLights[i].colour = gLightsColours[i];
		gLights[i].model->SetScale(pow(gLightStrengths[i],0.7f));
	}
	gLights[4].model->FaceTarget(gTroll->Position());
	gLights[5].model->FaceTarget(gTroll->Position());

	gWater->SetPosition({ 0, 10, 0 });

}

void ModelManager::CreateCameras()
{
	//// Set up cameras ////

	gCamera = new Camera();
	gCamera->SetPosition({ 40, 30, -90 });
	gCamera->SetRotation({ ToRadians(8.0f), ToRadians(-18.0f), 0.0f });
	gCamera->SetNearClip(0.7f);
	gCamera->SetFarClip(100000.0f);


	//**** Portal camera is the view shown in the portal object's texture ****//
	gPortalCamera = new Camera();
	gPortalCamera->SetPosition({ 45, 45, 85 });
	gPortalCamera->SetRotation({ ToRadians(20.0f), ToRadians(215.0f), 0 });

	gPortalCamera2 = new Camera();
	gPortalCamera2->SetPosition({ 35, 35, 75 });
	gPortalCamera2->SetRotation({ ToRadians(20.0f), ToRadians(215.0f), 0 });
}

void ModelManager::RenderDefaultModels(TextureManager* GetTexture)
{
	//// Render other lit models ////
		// Using a pixel shader that tints the texture - don't need a tint on the sky so set it to white
	gPerModelConstants.objectColour = { 1, 1, 1 };

	// Sky points inwards
	gD3DContext->RSSetState(gCullNoneState);

	// Render sky
	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gSkyDiffuseSpecularMapSRV);
	gSky->Render();
	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gShadowMappingPixelShader, nullptr, 0);
	//Note to self:Add house shaders after textures have been complete
	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gGreyDiffuseSpecularMapSRV);
	//gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
	gWaterHouse->Render();
	gMainHouse->Render();
	//Ground Texture Selection
	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gGroundDiffuseSpecularMapSRV);
	gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

	gFloor->Render();

	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gStoneDiffuseSpecularMapSRV);
	gCube[0]->Render();

	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gCrateDiffuseSpecularMapSRV);
	gCrate->Render();


	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gTrollSpecularDiffuseMapSRV);
	gTroll->Render();


	gD3DContext->VSSetShader(gLerpVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gLerpPixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gStoneDiffuseSpecularMapSRV);
	gD3DContext->PSSetShaderResources(1, 1, &GetTexture->gBrickDiffuseSpecularMapSRV);
	gCube[1]->Render();



	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gShadowMappingPixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gTeapotSpecularDiffuseMapSRV);
	gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
	gTeapot->Render();


	//gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gPortalTextureSRV);
	//gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

	//gPortal->Render();

	gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
	gD3DContext->RSSetState(gCullNoneState);

	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gDecalDiffuseSpecularMapSRV);

	gDecal->Render();
	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
	gD3DContext->RSSetState(gCullBackState);

	/*gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gPixelLightingPixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gTVDiffuseSpecularMapSRV);
	gD3DContext->PSSetShaderResources(1, 1, &GetTexture->gPortalTextureSRV);
	gPortal2->Render();*/
}

void ModelManager::GetCamera(Camera* cameraIn)
{
	// Set camera matrices in the constant buffer and send over to GPU
	gPerFrameConstants.cameraMatrix = cameraIn->WorldMatrix();
	gPerFrameConstants.viewMatrix = cameraIn->ViewMatrix();
	gPerFrameConstants.projectionMatrix = cameraIn->ProjectionMatrix();
	gPerFrameConstants.viewProjectionMatrix = cameraIn->ViewProjectionMatrix();
	UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
	gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);
}

void ModelManager::RenderLights(TextureManager* GetTexture)
{
	//// Render lights ////

	gD3DContext->VSSetShader(gLightModelVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gLightModelPixelShader, nullptr, 0);

	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gLightDiffuseMapSRV);
	gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

	gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
	gD3DContext->RSSetState(gCullNoneState);

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		gPerModelConstants.objectColour = gLights[i].colour;
		gLights[i].model->Render();
	}
	// Restore standard states
	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
	gD3DContext->RSSetState(gCullBackState);
}

void ModelManager::PrepareRenderModels(ID3D11DeviceContext* gD3DContext, TextureManager *GetTexture)
{
	

	gD3DContext->PSSetShaderResources(1, 1, &GetTexture-> gWaterNormalMapSRV);
	gD3DContext->VSSetShaderResources(1, 1, &GetTexture->gWaterNormalMapSRV);
	
	gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
	gD3DContext->VSSetSamplers(0, 1, &gAnisotropic4xSampler);
	gD3DContext->PSSetSamplers(1, 1, &gBilinearMirrorSampler);

	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
	gD3DContext->RSSetState(gCullBackState);

	//***************************
	// Render water height
	//***************************

	// Target the water height texture for rendering
	gD3DContext->OMSetRenderTargets(1, &GetTexture->gWaterHeightRenderTarget, gDepthStencil);

	// Clear the water depth texture and depth buffer
	// Note we reuse the same depth buffer for all the rendering passes, clearing it each time
	float Zero[4] = { 0,0,0,0 };
	gD3DContext->ClearRenderTargetView(GetTexture->gWaterHeightRenderTarget, Zero);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Select shaders
	gD3DContext->VSSetShader(gWaterSurfaceVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gWaterHeightPixelShader, nullptr, 0);
	gD3DContext->GSSetShader(nullptr, nullptr, 0);  // Switch off geometry shader when not using it (pass nullptr for first parameter)

	// Render heights of water surface
	gWater->Render();


	//***************************
	// Render refracted scene
	//***************************

	// Target the refraction texture for rendering and clear depth buffer
	gD3DContext->OMSetRenderTargets(1, &GetTexture->gRefractionRenderTarget, gDepthStencil);
	gD3DContext->ClearRenderTargetView(GetTexture->gRefractionRenderTarget, &gBackgroundColor.r);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Select the water height map (rendered in the last step) as a texture, so the refraction shader can tell what is underwater
	gD3DContext->PSSetShaderResources(2, 1, &GetTexture->gWaterHeightSRV); // First parameter must match texture slot number in the shader

	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gRefractedPixelLightingPixelShader, nullptr, 0);

	RenderDefaultModels(GetTexture);

	gD3DContext->VSSetShader(gBasicTransformWorldPosVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gRefractedTintedTexturePixelShader, nullptr, 0);

	RenderLights(GetTexture);
	
	//***************************
	// Render reflected scene
	//***************************

	// Reflect the camera's matrix in the water plane - to show what is seen in the reflection.
	// Will assume the water is horizontal in the xz plane, which makes the reflection simple:
	// - Negate the y component of the x,y and z axes of the reflected camera matrix
	// - Put the reflected camera y position on the opposite side of the water y position
	CMatrix4x4 originalMatrix = gCamera->WorldMatrix();
	gCamera->XAxis().y *= -1; // Negate y component of each axis of the matrix
	gCamera->YAxis().y *= -1;
	gCamera->ZAxis().y *= -1;

	// Camera distance above water = Camera.y - Water.y
	// Reflected camera is same distance below water = Water.y - (Camera.y - Water.y) = 2*Water.y - Camera.y
	// (Position is on bottom row (row 3) of matrix so Camera.y is matrix element e31)
	gCamera->Position().y = gWater->Position().y * 2 - gCamera->Position().y;

	// Use camera with reflected matrix for rendering
	GetCamera(gCamera);

	// IMPORTANT: when rendering in a mirror must switch from back face culling to front face culling (because clockwise / anti-clockwise order of points will be reversed)
	gD3DContext->RSSetState(gCullFrontState);

	// Target the reflection texture for rendering and clear depth buffer
	gD3DContext->OMSetRenderTargets(1, &GetTexture->gReflectionRenderTarget, gDepthStencil);
	gD3DContext->ClearRenderTargetView(GetTexture->gReflectionRenderTarget, &gBackgroundColor.r);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Note that water height map is still selected as a texture (from the previous step) and will be used here to tell what is above the water

	////// Render lit models

	// Select shaders for reflection rendering of lit models
	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gReflectedPixelLightingPixelShader, nullptr, 0);

	RenderDefaultModels(GetTexture);


	// Select shaders for reflection rendering of non-lit models
	gD3DContext->VSSetShader(gBasicTransformWorldPosVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gReflectedTintedTexturePixelShader, nullptr, 0);

	RenderLights(GetTexture);
	
	// Restore original camera and culling state
	gCamera->WorldMatrix() = originalMatrix;
	GetCamera(gCamera);
	gD3DContext->RSSetState(gCullBackState);

	// Detach the water height map from being a source texture so it can be used as a render target again next frame (if you don't do this DX emits lots of warnings)
	ID3D11ShaderResourceView* gNullSRV = nullptr;
	gD3DContext->PSSetShaderResources(2, 1, &gNullSRV);


	//***************************
	// Render main scene
	//***************************

	// Finally target the back buffer for rendering, clear depth buffer
	gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, gDepthStencil);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	////// Render lit models
	
	// Select shaders for ordinary rendering of lit models
	gD3DContext->PSSetShaderResources(1, 1, &GetTexture->gShadowMap1SRV);
	gD3DContext->PSSetShaderResources(2, 1, &GetTexture->gShadowMap2SRV);
	gD3DContext->PSSetSamplers(1, 1, &gPointSampler);
	
	RenderDefaultModels(GetTexture);


	////// Render water surface - combining reflection and refraction
	// Render water before transparent objects or it will draw over them

	// Select the reflection and refraction textures (rendered in the previous steps)
	gD3DContext->PSSetShaderResources(3, 1, &GetTexture->gRefractionSRV); // First parameter must match texture slot number in the shader
	gD3DContext->PSSetShaderResources(4, 1, &GetTexture->gReflectionSRV);

	gD3DContext->VSSetShader(gWaterSurfaceVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gWaterSurfacePixelShader, nullptr, 0);
	gWater->Render();

	// Detach the reflection/refraction maps from being source textures so they can be used as a render target again next frame (if you don't do this DX emits lots of warnings)
	gD3DContext->PSSetShaderResources(3, 1, &gNullSRV);
	gD3DContext->PSSetShaderResources(4, 1, &gNullSRV);


	////// Render sky and lights

	// Select shaders for ordinary rendering of non-lit models
	gD3DContext->VSSetShader(gBasicTransformVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gTintedTexturePixelShader, nullptr, 0);
	RenderLights(GetTexture);
}

void ModelManager::UpdateModels(float &frameTime, PerFrameConstants &gPerFrameConstants)
{
	static float rotate2 = 0.0f;
	static float go = true;
	gLights[4].model->SetPosition(gTroll->Position() + CVector3{ cos(rotate2) * gLightOrbit, 10, sin(rotate2) * gLightOrbit });
	gLights[4].model->FaceTarget(gTroll->Position());

	gLights[5].model->FaceTarget(gTroll->Position());

	if (go)  rotate2 -= gLightOrbitSpeed * frameTime;
	if (KeyHit(Key_1))  go = !go;

	// Control sphere (will update its world matrix)
	if(KeyHeld(Key_Z))gSphere->Control(gWholeMesh,frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
	//Light pulsation 
	if (gLightPulsation)
	{
		gLights[3].strength += gPulseRate * frameTime;
		if (gLights[3].strength >= gMaxLightStrength)gLightPulsation = false;
	}
	else if (!gLightPulsation)
	{
		gLights[3].strength -= gPulseRate * frameTime;
		if (gLights[3].strength <= gMinLightStrength)gLightPulsation = true;
	}
	//Light colours changing
	//Increment only 1 colour out of the 3 each frame
	if (!gColourSwitch)
	{
		if (gRed < ColourMax&&gColourCount == Red)
		{
			gRed += gColourRate * frameTime;
			gColourCount = Blue;
			gLights[2].colour = { gRed, gGreen, gBlue };
		}
		else if (gBlue < ColourMax && gColourCount == Blue)
		{
			gBlue += gColourRate * frameTime;
			gColourCount = Green;
			gLights[2].colour = { gRed, gGreen, gBlue };
		}
		else if (gGreen < ColourMax && gColourCount == Green)
		{
			gGreen += gColourRate * frameTime;
			gColourCount = Red;
			gLights[2].colour = { gRed, gGreen, gBlue };
		}

		if (gRed >= ColourMax)gColourSwitch = true;
		if (gBlue >= ColourMax)gColourSwitch = true;
		if (gGreen >= ColourMax)gColourSwitch = true;

	}
	else if (gColourSwitch)
	{
		if (gRed > 0.0f && gColourCount == Red)
		{
			gRed -= gColourRate * frameTime;
			gColourCount = Blue;
			gLights[2].colour = { gRed, gGreen, gBlue };
		}
		else if (gBlue > 0.0f && gColourCount == Blue)
		{
			gBlue -= gColourRate * frameTime;
			gColourCount = Green;
			gLights[2].colour = { gRed, gGreen, gBlue };
		}
		else if (gGreen > 0.0f && gColourCount == Green)
		{
			gGreen -= gColourRate * frameTime;
			gColourCount = Red;
			gLights[2].colour = { gRed, gGreen, gBlue };
		}

		if (gRed <= 0.0f)gColourSwitch = false;
		if (gBlue <= 0.0f)gColourSwitch = false;
		if (gGreen <= 0.0f)gColourSwitch = false;

	}

	gPerFrameConstants.wiggle += 6 * frameTime;
	//Transition from Texture A to Texture B
	if (gPerFrameConstants.lerpCount <= ColourMax && !gTextureCount)
	{
		gPerFrameConstants.lerpCount += 0.2f*frameTime;
		if (gPerFrameConstants.lerpCount >= ColourMax)gTextureCount = true;

	}

	//Transition from Texture B to Texture A
	if (gPerFrameConstants.lerpCount >= 0.0f && gTextureCount)
	{
		gPerFrameConstants.lerpCount -= 0.2f*frameTime;
		if (gPerFrameConstants.lerpCount <= 0.0f)gTextureCount = false;
	}
	// Control water height
	gPerFrameConstants.waterPlaneY = gWater->Position().y;
	if (KeyHeld(Key_Period))  gPerFrameConstants.waterPlaneY += 5.0f * frameTime;
	if (KeyHeld(Key_Comma))  gPerFrameConstants.waterPlaneY -= 5.0f * frameTime;
	gWater->SetPosition({ gWater->Position().x, gPerFrameConstants.waterPlaneY, gWater->Position().z });

	// Control wave height
	static float waveScale = 0.6f;
	if (KeyHeld(Key_Plus))  waveScale += 0.1f * frameTime;
	if (KeyHeld(Key_Minus))  waveScale -= 0.1f * frameTime;
	if (waveScale < 0)  waveScale = 0;
	gPerFrameConstants.waveScale = waveScale;

	// Move water
	static CVector2 waterPos = { 0, 0 };
	const float waterSpeed = 1.0f;
	waterPos += frameTime * waterSpeed * CVector2(0.01f, 0.015f);
	gPerFrameConstants.waterMovement = waterPos;
	// Orbit the light
	static float rotate = 0.0f;
	gLights[0].model->SetPosition(gCube[0]->Position() + CVector3{ cos(rotate) * gLightOrbit, 0.0f, sin(rotate) * gLightOrbit });
	rotate -= gLightOrbitSpeed * frameTime;
	if (KeyHeld(Key_X))gTeapot->Control(gWholeMesh,frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
	if (KeyHeld(Key_C))gTroll->Control(gWholeMesh,frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
	// Control camera (will update its view matrix)
	gCamera->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D);
	gPortalCamera->Control(frameTime, Key_T, Key_G, Key_F, Key_H, Key_B, Key_N, Key_J, Key_M);
}