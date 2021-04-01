#include "ModelManager.h"

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
	gCurrentCamera = CameraTypes::Free;
	gLightPulsation = false;
	gPulseRate = 10.0f;
	gMinLightStrength = 10.0f;
	gMaxLightStrength = 30.0f;
	gTextureCount = false;
	gSpotlightConeAngle = 90.0f;
}
//==================Release all model===========================//
ModelManager::~ModelManager()
{
	delete gCamera;        gCamera = nullptr;
	delete gPortalCamera;  gPortalCamera = nullptr;
	delete gPortalCamera2; gPortalCamera2 = nullptr;

	delete gPortal;  gPortal = nullptr;
	delete gPortal2;  gPortal2 = nullptr;
	
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
	for (int i = 0; i < kCubeNum; i++)
	{
		delete gCube[i];
		gCube[i] = nullptr;
	}
	delete gTeapot;  gTeapot = nullptr;
	delete gFloor;   gFloor = nullptr;
	delete gWaterHouse; gWaterHouse = nullptr;
	delete gWater;		gWater = nullptr;
	delete gMainHouse; gMainHouse = nullptr;
	for (unsigned int i = 0; i < kTreeNum; ++i)
	{
		delete gTree[i]; gTree[i] = nullptr;
		delete gTree2[i]; gTree2[i] = nullptr;
	}
	delete gDuck; gDuck = nullptr;

	delete gDummyMesh; gDummyMesh = nullptr;
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
	delete gNullSRV; gNullSRV = nullptr;
	delete gTreeMesh; gTreeMesh = nullptr;
	delete gTree2Mesh; gTree2Mesh = nullptr;

	
}
//==================Creating a new meshes===========================//
bool ModelManager::LoadMeshes()
{
	try
	{
		gDuckMesh = new Mesh(MeshesMediaFolder + "duck.obj");
		gHouseTwoMesh = new Mesh(MeshesMediaFolder + "House2.obj");
		gTreeMesh = new Mesh(MeshesMediaFolder + "Tree.obj");
		gTree2Mesh = new Mesh(MeshesMediaFolder + "Tree2.obj");
		gCubeMesh = new Mesh(MeshesMediaFolder + "Cube.x");
		gTrollMesh = new Mesh(MeshesMediaFolder + "Troll.x");
		gDecalMesh = new Mesh(MeshesMediaFolder + "Decal.x");
		gCrateMesh = new Mesh(MeshesMediaFolder + "CargoContainer.x");
		gSphereMesh = new Mesh(MeshesMediaFolder + "Sphere.x");
		gGroundMesh = new Mesh(MeshesMediaFolder + "Hills.x");
		gLightMesh = new Mesh(MeshesMediaFolder + "Light.x");
		gPortalMesh = new Mesh(MeshesMediaFolder + "Portal.x");
		gTeapotMesh = new Mesh(MeshesMediaFolder + "Teapot.x");
		gFloorMesh = new Mesh(MeshesMediaFolder + "mount.obj");
		gWaterHouseMesh = new Mesh(MeshesMediaFolder + "waterHouseTwo.obj");
		gMainHouseMesh = new Mesh(MeshesMediaFolder + "mainHouse.obj");
		gSkyMesh = new Mesh(MeshesMediaFolder + "Skybox.x");
		gDummyMesh = new Mesh(MeshesMediaFolder + "Dummy.x");
		gWaterMesh = new Mesh(CVector3(-200, 0, -200), CVector3(200, 0, 200), 400, 400, true);
	}
	catch (std::runtime_error e)
	{
		gLastError = e.what();
		return false;
	}
	return true;
}
//==================Creating a new model===========================//
//When creating a new model the following steps need to be done in order to load it correctly
//When adding the new model always push it last on the model list oterwise coordinates will for each model will be loaded wrong
//If the model has own texture which you dont want to load manually indicate that with the mesh SetTexture variable
//Automatic texture loading deals with the main mesh and submeshes texture reading instructions from the model mtl file
//Submesh loading allows you to load Diffuse maps, Normal Maps and Specular Maps
void ModelManager::CreateModels()
{
	for (unsigned int i = 0; i < kTreeNum; ++i)
	{
		gTree[i] = new Model(gTreeMesh);
		gModelList.push_back(gTree[i]);
	
	}
	for (unsigned int i = 0; i < kTreeNum; ++i)
	{
		gTree2[i] = new Model(gTree2Mesh);
		gModelList.push_back(gTree2[i]);
	}
	gTreeMesh->SetTexture = true;//Indicates tree has own texture resource
	gTree2Mesh->SetTexture = true;
	gCube[0] = new Model(gCubeMesh);
	gModelList.push_back(gCube[0]);
	gCube[1] = new Model(gCubeMesh);
	gModelList.push_back(gCube[1]);
	gCrate = new Model(gCrateMesh);
	gModelList.push_back(gCrate);
	gSphere = new Model(gSphereMesh);
	gGround = new Model(gGroundMesh);
	gWaterHouse = new Model(gWaterHouseMesh);
	gModelList.push_back(gWaterHouse);
	gMainHouse = new Model(gMainHouseMesh);
	gModelList.push_back(gMainHouse);
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		gLights[i].model = new Model(gLightMesh);
	}
	gPortal = new Model(gCubeMesh);
	gModelList.push_back(gPortal);
	gPortal2 = new Model(gCubeMesh);
	gModelList.push_back(gPortal2);
	gTeapot = new Model(gTeapotMesh);
	gModelList.push_back(gTeapot);
	gTroll = new Model(gTrollMesh);
	gTrollMesh->SetTexture = true;
	gModelList.push_back(gTroll);
	gHouseTwoMesh->SetTexture = true;
	gHouseTwo = new Model(gHouseTwoMesh);
	gModelList.push_back(gHouseTwo);
	gWater = new Model(gWaterMesh);
	gModelList.push_back(gWater);
	gFloor = new Model(gFloorMesh);
	gModelList.push_back(gFloor);
	gSky = new Model(gSkyMesh);
	gModelList.push_back(gSky);
	gDecal = new Model(gDecalMesh);
	gModelList.push_back(gDecal);
	gDuck = new Model(gDuckMesh);
	gDuckMesh->SetTexture = true;
	gModelList.push_back(gDuck);

}
//==================Scene set up===========================//
void ModelManager::InitialSceneSetup()
{
	
	//Create the lights around the scene
	//The lights are not added to the model list therefore cant be controlled by model loader
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		gLights[i].model->SetPosition(gLightsPosition[i]);
		gLights[i].strength = gLightStrengths[i];
		gLights[i].colour = gLightsColours[i];
		gLights[i].model->SetScale(pow(gLightStrengths[i],0.7f));
	}
	gLights[4].model->FaceTarget(gTroll->Position());
	gLights[5].model->FaceTarget(gTroll->Position());
	//Prepare scene by loading all the model coordinates
	//Position Coordinates
	gLoadCoordinates.open(CoordinatesFile);
	string line;
	auto it = gModelList.begin();
	while (std::getline(gLoadCoordinates, line))
	{
		std::istringstream iss(line);
		float x, y, z;
		if (!(iss >> x >> y >> z)) { break; }
		(*it)->SetPosition({ x,y,z });
		if(it!=gModelList.end())it++;
	}
	gLoadCoordinates.close();
	
	//Rotation coordinates
	gLoadCoordinates.open(RotationFile);

	it = gModelList.begin();
	while (std::getline(gLoadCoordinates, line))
	{
		std::istringstream iss(line);
		float x, y, z;
		if (!(iss >> x >> y >> z)) { break; }
		(*it)->SetRotation({ x,y,z });
		if (it != gModelList.end())it++;
	}
	gLoadCoordinates.close();

	gLoadCoordinates.open(ScaleFile);
	//Scale coordinates
	it = gModelList.begin();
	while (std::getline(gLoadCoordinates, line))
	{
		std::istringstream iss(line);
		float factor;
		if (!(iss >> factor)) { break; }
		(*it)->ScaleFactor = factor;
		if(factor>0)(*it)->SetScale(factor);
		if (it != gModelList.end())it++;
	}
	gLoadCoordinates.close();
	//Set dayCycle 
	gPerFrameConstants.dayCycle = 1.0f;
}
//==================Creating cameras===========================//

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

//==================Default models rendering===========================//
void ModelManager::RenderDefaultModels()
{
	//Set texture to be passed inside the shader if it was manually loaded
	//Render the model
	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gGreyDiffuseSpecularMapSRV);
	gWaterHouse->Render();
	gMainHouse->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);
	//Ground Texture Selection
	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gGroundDiffuseSpecularMapSRV);
	gFloor->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);


	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gStoneDiffuseSpecularMapSRV);
	gCube[0]->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);


	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gCrateDiffuseSpecularMapSRV);
	gCrate->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);



	gTroll->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);

	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gTeapotSpecularDiffuseMapSRV);
	gTeapot->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);

	gD3DContext->VSSetShader(gLerpVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gLerpPixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gStoneDiffuseSpecularMapSRV);
	gD3DContext->PSSetShaderResources(6, 1, &TextureCreator->gBrickDiffuseSpecularMapSRV);
	gCube[1]->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);
	gD3DContext->PSSetShaderResources(6, 1, &gNullSRV);

	
	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gPixelLightingPixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gPortalTextureSRV);
	gPortal->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);



	gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
	gD3DContext->RSSetState(gCullNoneState);

	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gDecalDiffuseSpecularMapSRV);
	gDecal->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);


	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
	gD3DContext->RSSetState(gCullBackState);

	gD3DContext->PSSetShader(gTreePixelShader, nullptr, 0);
	gD3DContext->OMSetBlendState(gAlphaBlendingState, nullptr, 0xffffff);
	gDuck->Render();
	gPerFrameConstants.alphaValue = 0.1f;
	gHouseTwo->Render();
	for (unsigned int i = 0; i < kTreeNum; ++i)
	{
		gTree[i]->Render();
		gTree2[i]->Render();
	}
	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gPixelLightingPixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gTVDiffuseSpecularMapSRV);
	gD3DContext->PSSetShaderResources(6, 1, &TextureCreator->gPortalTextureSRV);
	gPortal2->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);
	gD3DContext->PSSetShaderResources(6, 1, &gNullSRV);

}
//==================Camera details passed to shaders===========================//
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
//==================Render Lights===========================//
void ModelManager::RenderLights()
{
	gPerModelConstants.objectColour = { 1, 1, 1 };

	// Sky points inwards
	gD3DContext->RSSetState(gCullNoneState);

	// Render sky
	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gSkyDiffuseSpecularMapSRV);
	gSky->Render();
	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);


	//// Render lights ////

	//Using a pixel shader that tints the texture - don't need a tint on the sky so set it to white

	gD3DContext->PSSetShaderResources(0, 1, &TextureCreator->gLightDiffuseMapSRV);

	gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
	gD3DContext->RSSetState(gCullNoneState);

	// Render other lit models ////
	

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		gPerModelConstants.objectColour = gLights[i].colour;
		gLights[i].model->Render();
	}

	gD3DContext->PSSetShaderResources(0, 1, &gNullSRV);

	// Restore standard states
	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
	gD3DContext->RSSetState(gCullBackState);
}

//==================Prepare Render Order===========================//
void ModelManager::PrepareRenderModels( Camera* camera)
{
	//***************************
	// Render scene for portal texture
	//***************************
	//Perform normal Render for portal textures first
	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gShadowMappingPixelShader, nullptr, 0);
	
	gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
	gD3DContext->VSSetSamplers(0, 1, &gAnisotropic4xSampler);
	gD3DContext->PSSetSamplers(1, 1, &gBilinearMirrorSampler);
	

	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
	gD3DContext->RSSetState(gCullBackState);

	RenderDefaultModels();

	RenderLights();

	//***************************
	// Render water height
	//***************************
	gD3DContext->PSSetShaderResources(1, 1, &TextureCreator-> gWaterNormalMapSRV);
	gD3DContext->VSSetShaderResources(1, 1, &TextureCreator->gWaterNormalMapSRV);

	// Target the water height texture for rendering
	gD3DContext->OMSetRenderTargets(1, &TextureCreator->gWaterHeightRenderTarget, gDepthStencil);

	// Clear the water depth texture and depth buffer
	// Note we reuse the same depth buffer for all the rendering passes, clearing it each time
	float Zero[4] = { 0,0,0,0 };
	gD3DContext->ClearRenderTargetView(TextureCreator->gWaterHeightRenderTarget, Zero);
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
	gD3DContext->OMSetRenderTargets(1, &TextureCreator->gRefractionRenderTarget, gDepthStencil);
	gD3DContext->ClearRenderTargetView(TextureCreator->gRefractionRenderTarget, &gBackgroundColor.r);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Select the water height map (rendered in the last step) as a texture, so the refraction shader can tell what is underwater
	gD3DContext->PSSetShaderResources(2, 1, &TextureCreator->gWaterHeightSRV); // First parameter must match texture slot number in the shader

	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gRefractedPixelLightingPixelShader, nullptr, 0);

	RenderDefaultModels();

	gD3DContext->VSSetShader(gBasicTransformWorldPosVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gRefractedTintedTexturePixelShader, nullptr, 0);

	RenderLights();
	
	//***************************
	// Render reflected scene
	//***************************

	// Reflect the camera's matrix in the water plane - to show what is seen in the reflection.
	// Will assume the water is horizontal in the xz plane, which makes the reflection simple:
	// - Negate the y component of the x,y and z axes of the reflected camera matrix
	// - Put the reflected camera y position on the opposite side of the water y position
	CMatrix4x4 originalMatrix = camera->WorldMatrix();
	camera->XAxis().y *= -1; // Negate y component of each axis of the matrix
	camera->YAxis().y *= -1;
	camera->ZAxis().y *= -1;

	// Camera distance above water = Camera.y - Water.y
	// Reflected camera is same distance below water = Water.y - (Camera.y - Water.y) = 2*Water.y - Camera.y
	// (Position is on bottom row (row 3) of matrix so Camera.y is matrix element e31)
	camera->Position().y = gWater->Position().y * 2 - camera->Position().y;

	// Use camera with reflected matrix for rendering
	GetCamera(camera);

	// IMPORTANT: when rendering in a mirror must switch from back face culling to front face culling (because clockwise / anti-clockwise order of points will be reversed)
	gD3DContext->RSSetState(gCullFrontState);

	// Target the reflection texture for rendering and clear depth buffer
	gD3DContext->OMSetRenderTargets(1, &TextureCreator->gReflectionRenderTarget, gDepthStencil);
	gD3DContext->ClearRenderTargetView(TextureCreator->gReflectionRenderTarget, &gBackgroundColor.r);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Note that water height map is still selected as a texture (from the previous step) and will be used here to tell what is above the water

	////// Render lit models

	// Select shaders for reflection rendering of lit models
	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gReflectedPixelLightingPixelShader, nullptr, 0);

	RenderDefaultModels();


	// Select shaders for reflection rendering of non-lit models
	gD3DContext->VSSetShader(gBasicTransformWorldPosVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gReflectedTintedTexturePixelShader, nullptr, 0);

	RenderLights();
	
	// Restore original camera and culling state
	camera->WorldMatrix() = originalMatrix;
	GetCamera(camera);
	gD3DContext->RSSetState(gCullBackState);

	// Detach the water height map from being a source texture so it can be used as a render target again next frame (if you don't do this DX emits lots of warnings)
	gD3DContext->PSSetShaderResources(2, 1, &gNullSRV);


	/****************************
	 Render main scene
	****************************/

	// Finally target the back buffer for rendering, clear depth buffer
	gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, gDepthStencil);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	////// Render lit models

	// Select shaders for ordinary rendering of lit models
	gD3DContext->PSSetShaderResources(7, 1, &TextureCreator->gShadowMap1SRV);
	gD3DContext->PSSetShaderResources(8, 1, &TextureCreator->gShadowMap2SRV);

	gD3DContext->PSSetSamplers(1, 1, &gPointSampler);
	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gShadowMappingPixelShader, nullptr, 0);
	
	
	RenderDefaultModels();

	////// Render water surface - combining reflection and refraction
	// Render water before transparent objects or it will draw over them

	// Select the reflection and refraction textures (rendered in the previous steps)
	gD3DContext->PSSetShaderResources(3, 1, &TextureCreator->gRefractionSRV); // First parameter must match texture slot number in the shader
	gD3DContext->PSSetShaderResources(4, 1, &TextureCreator->gReflectionSRV);

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

	RenderLights();
	gD3DContext->PSSetShaderResources(7, 1, &gNullSRV);
	gD3DContext->PSSetShaderResources(8, 1, &gNullSRV);
	
	
}

//==================Update models===========================//
void ModelManager::UpdateModels(float &frameTime)
{

	static float rotate2 = 0.0f;
	static float go = true;
	//Attach lights with shadows to the troll
	gLights[4].model->SetPosition(gTroll->Position() + CVector3{ cos(rotate2) * gLightOrbit, 10, sin(rotate2) * gLightOrbit });
	gLights[4].model->FaceTarget(gTroll->Position());
	gLights[5].model->SetPosition({ gTeapot->Position().x,gTeapot->Position().y+20.0f,gTeapot->Position().z-20.0f });

	gLights[5].model->FaceTarget(gTeapot->Position());

	if (go)  rotate2 -= gLightOrbitSpeed * frameTime;
	if (KeyHit(Key_1))  go = !go;

	// Control sphere (will update its world matrix)
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
	//Water wiggle effect
	gPerFrameConstants.wiggle += gWiggleIncrement * frameTime;
	//Transition from Texture A to Texture B
	if (gPerFrameConstants.lerpCount <= ColourMax && !gTextureCount)
	{
		gPerFrameConstants.lerpCount += gLerpIncrement *frameTime;
		if (gPerFrameConstants.lerpCount >= ColourMax)gTextureCount = true;

	}
	//Transition from Texture B to Texture A
	if (gPerFrameConstants.lerpCount >= 0.0f && gTextureCount)
	{
		gPerFrameConstants.lerpCount -= gLerpIncrement *frameTime;
		if (gPerFrameConstants.lerpCount <= 0.0f)gTextureCount = false;
	}
	//When the camera is below the water level apply UnderWater Post Processing effect on screen
	//No bounding box applied only tested in the Y coordinate
	if (gCamera->Position().y<gWater->Position().y)
	{
		gCurrentPostProcess = PostProcess::UnderWater;
		SoundLoader->PlayUnderWaterFile(SoundState::Play);//Play underwater sound
	}
	else if(gCurrentPostProcess!=PostProcess::Bloom)//This check is to prevent switching off other post processing effects
	{
		gCurrentPostProcess = PostProcess::None;
		SoundLoader->PlayUnderWaterFile(SoundState::Stop);//Stop underwater sound

	}
	// Control water height
	gPerFrameConstants.waterPlaneY = gWater->Position().y;
	if (KeyHeld(Key_Period))  gPerFrameConstants.waterPlaneY += gWaterIncrement * frameTime;
	if (KeyHeld(Key_Comma))  gPerFrameConstants.waterPlaneY -= gWaterIncrement * frameTime;
	gWater->SetPosition({ gWater->Position().x, gPerFrameConstants.waterPlaneY, gWater->Position().z });

	// Control wave height
	static float waveScale = gWaveScale;
	if (KeyHeld(Key_Plus))  waveScale += gWaveIncrement * frameTime;
	if (KeyHeld(Key_Minus))  waveScale -= gWaveIncrement * frameTime;
	if (waveScale < 0)  waveScale = 0;
	gPerFrameConstants.waveScale = waveScale;

	// Move water
	static CVector2 waterPos = { 0, 0 };
	waterPos += frameTime * waterSpeed * CVector2(0.01f, 0.015f);
	gPerFrameConstants.waterMovement = waterPos;
	// Orbit the light
	static float rotate = 0.0f;
	gLights[0].model->SetPosition(gCube[0]->Position() + CVector3{ cos(rotate) * gLightOrbit, 0.0f, sin(rotate) * gLightOrbit });
	rotate -= gLightOrbitSpeed * frameTime;
	
	//Portal Camera controls
	// Controling the  camera (will update its view matrix inside the portal)
	gPortalCamera->Control(frameTime, Key_T, Key_G, Key_F, Key_H, Key_B, Key_N, Key_J, Key_M);

	if (KeyHeld(Key_Z))gPerFrameConstants.dayCycle += gDayIncrement * frameTime;
	if (KeyHeld(Key_X))gPerFrameConstants.dayCycle -= gDayIncrement * frameTime;
	//Control bloom strength whenthe bloom post processing effect has been applied
	if (KeyHeld(Key_I)) gPerFrameConstants.blurIncrement += gBloomIncrement * frameTime;
	if (KeyHeld(Key_K)) gPerFrameConstants.blurIncrement -= gBloomIncrement * frameTime;
	//Updating the matrix of the Water mill to create a spinning animation for the water mill
	MatrixCopy = gWaterHouse->WorldMatrix(2);
	MatrixCopy.RotateLocalX(-(gWaterMillSpin *frameTime));
	gWaterHouse->SetWorldMatrix(MatrixCopy,2);
	//Camera Work with selecting models
	//Main camera
	if (KeyHit(Key_F1))
	{
		gCurrentCamera = CameraTypes::Free;
		gCamera->SetPosition(gLastPosition);
	}
	//First person camera 
	if (KeyHit(Key_F2))
	{ 
		gCurrentCamera = CameraTypes::FirtsPerson;
		gCamera->SetRotation({ 0.0f,0.0f,0.0f });
		gCamera->SetPosition({ gCamera->Position().x, gFirstPersonY,gCamera->Position().z- gFirstPersonZ });
	}

	if (gCurrentCamera == CameraTypes::Free)
	{
		//Free camera controls
		gCamera->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D);
		//====================================Simple Level Builder====================================//
		//When the camera is in free view clicking with the Left Mouse button over a model will let you control it
		//You can Rotate, Move and Scale the model using the numpad keys
		//Once you are ready with the positioning of the model you can save the new position, scale and rotation to a file
		//Every time the game is ran after saving the model changed position, it will load the model with coordinates read from the file
		if (KeyHit(Mouse_LButton))
		{
			//Perform Camera Picking so the closest object in the camera Z-direction will be picked
			//Check which model you have selected by running through a list of models
			//Once the model has been selected make a new pointer to point at it so we can modify it's values 
			//The new pointer allows us to deal with individual model without having to access their main pointer
			for (auto it = gModelList.begin(); it != gModelList.end(); it++)
			{
				CVector3 modelPicked = gCamera->PixelFromWorldPt((*it)->Position(), gViewportWidth, gViewportHeight);
				float pixelDistance = Distance(CVector2((float)GetMouseX(), (float)GetMouseY()), CVector2(modelPicked.x, modelPicked.y));
				if (pixelDistance < gNearestDistance)
				{
					gSelectedModel = *it;
					gSelectedModel->Selected = true;//Additional check if the model has been selected
				}
			}

		}
		//If a model has been selected
		if (gSelectedModel != nullptr && gSelectedModel->Selected)
		{
		
			if (KeyHeld(Key_O))//Scale Up the model in all the axis with a set scale factor
			{
				gSelectedModel->ScaleFactor += gScaleFactor * frameTime;
				gSelectedModel->SetScale(gSelectedModel->ScaleFactor);
			}
			if (KeyHeld(Key_P))//Scale Down the model in all the axis with a set scale factor
			{
				gSelectedModel->ScaleFactor -= gScaleFactor * frameTime;
				gSelectedModel->SetScale(gSelectedModel->ScaleFactor);
			}
		}
		//Update all the model Position,Scale and Rotation coordinates
		if (KeyHit(Key_Numpad5))
		{
			//Open the individual files for each set of coordinates
			gCoordinatesFile.open(CoordinatesFile);
			gRotationFile.open(RotationFile);
			gScaleFile.open(ScaleFile);
			//Run through the model list and overwrite the old coodinates for each model
			for (auto it = gModelList.begin(); it != gModelList.end(); it++)
			{
				gCoordinatesFile << (*it)->Position().x << " " << (*it)->Position().y << " " << (*it)->Position().z << " " << endl;
				gRotationFile << (*it)->Rotation().x << " " << (*it)->Rotation().y << " " << (*it)->Rotation().z << " " << endl;
				gScaleFile << (*it)->ScaleFactor << endl;

			}
			//Close all the files after completing overwrite
			gCoordinatesFile.close();
			gRotationFile.close();
			gScaleFile.close();
		}
		//If you have selected model the Right Mouse Key will allow you to unselect it
		if (KeyHit(Mouse_RButton) && gSelectedModel != nullptr)
		{
			gSelectedModel->Selected = false;
			gSelectedModel = nullptr;
		}
		//Controls for the model
		if (gSelectedModel != nullptr && gSelectedModel->Selected)gSelectedModel->Control(0, frameTime,
			Key_Numpad0, Key_Numpad0, Key_Numpad1, Key_Numpad3, Key_Multiply, Key_Divide,
			Key_Numpad8, Key_Numpad2, Key_Numpad4, Key_Numpad6, Key_Numpad7, Key_Numpad9);
		gLastPosition = gCamera->Position();//Keeps the last position of the main camera for swithching between cameras
	}
	else if (gCurrentCamera == CameraTypes::FirtsPerson)
	{
		if (gCurrentCamera == CameraTypes::FirtsPerson)
		{//Switching to first person view updates the camera controls to WASD to simulate normal character movement
			gCamera->Control(frameTime, Key_Nonconvert, Key_Nonconvert,Key_A,Key_D,Key_W,Key_S,Key_Nonconvert,Key_Nonconvert);
			//When the in first person play footsteps sound when moving backwards and forwards
			if (KeyHeld(Key_W)|| KeyHeld(Key_S))
			{
				SoundLoader->PlayFootStepsFile(SoundState::Play);
			}
			//Stop footsteps sound when not moving
			else SoundLoader->PlayFootStepsFile(SoundState::Stop);
		}
	}
}

