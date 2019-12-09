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
	delete gTroll;   gTroll = nullptr;;
	delete gGround;  gGround = nullptr;
	delete gSphere;  gSphere = nullptr;
	delete gCrate;   gCrate = nullptr;
	delete gDecal;   gDecal = nullptr;
	delete *gCube;    for (int i = 0; i < kCubeNum; i++)gCube[i] = nullptr;
	delete gTeapot;  gTeapot = nullptr;
	delete gFloor;   gFloor = nullptr;
	delete gWaterHouse; gWaterHouse = nullptr;

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
		gMainHouseMesh = new Mesh(MeshesMediaFolder + "mainHouse.fbx");
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
}

void ModelManager::InitialSceneSetup()
{
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

void ModelManager::PrepareRenderModels(ID3D11DeviceContext* gD3DContext, TextureManager *GetTexture)
{
	// Shaders Selection
	gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gShadowMappingPixelShader, nullptr, 0);


	gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
	gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
	gD3DContext->RSSetState(gCullBackState);


	//Note to self:Add house shaders after textures have been complete
	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gGreyDiffuseSpecularMapSRV);
	gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
	gWaterHouse->Render();
	gMainHouse->Render();
	//Ground Texture Selection
	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gGroundDiffuseSpecularMapSRV);
	gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

	gFloor->Render();


	//// Render other lit models ////


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

	gD3DContext->PSSetShaderResources(0, 1, &GetTexture->gPortalTextureSRV);
	gPortal->Render();


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