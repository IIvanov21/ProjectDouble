#pragma once
//#include "Definitions.h"
#include "TextureManager.h"
#include "Collision.h"
#include ".//Common//Defines.h"
#include "Input.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "SoundClass.h"
#ifndef _MODELMANAGER_H_INCLUDED_
#define _MODELMANAGER_H_INCLUDED_
class ModelManager
{
public:
	//Constant Varialbles
	
	static const int   kCubeNum = 2;
	static const int kTreeNum = 10;
	const float gLightOrbit = 20.0f;
	const float gLightOrbitSpeed = 0.7f;
	static const int NUM_LIGHTS = 6;
	const float gScaleFactor = 0.5f;
	float ColourMax = 1.0f;
	float gWiggleIncrement = 6.0f;
	float gLerpIncrement = 0.2f;
	const float gDayIncrement = 0.5f;
	const float gBloomIncrement = 10.0f;
	const float gWaterMillSpin = 0.5f;
	const int gWholeMesh = 0;
	const float gNearestDistance = 150;
	float gFirstPersonY = 27.0f;
	float gFirstPersonZ = 2.0f;
	const std::string MeshesMediaFolder = "./Media/Meshes/";
	ColourRGBA gBackgroundColor = { 0.5f, 0.5f, 0.5f, 1.0f };
	//========Writing to a/Reading from a file======//
	ofstream gCoordinatesFile;
	ofstream gRotationFile;
	ofstream gScaleFile;
	ifstream gLoadCoordinates;
	ifstream gRotationCoordinates;
	string CoordinatesFile = "ModelCoords.txt";
	string RotationFile = "RotationCoords.txt";
	string ScaleFile = "ScaleFactor.txt";
	//==========Meshes=========//
	vector <Model*> gModelList;

	Mesh* gCubeMesh;
	Mesh* gTreeMesh;
	Mesh* gTree2Mesh;
	Mesh* gDuckMesh;
	Mesh* gTrollMesh;
	Mesh* gDecalMesh;
	Mesh* gCrateMesh;
	Mesh* gSphereMesh;
	Mesh* gGroundMesh;
	Mesh* gLightMesh;
	Mesh* gPortalMesh;
	Mesh* gFloorMesh;
	Mesh* gTeapotMesh;
	Mesh* gWaterHouseMesh;
	Mesh* gMainHouseMesh;
	Mesh* gWaterMesh;
	Mesh* gSkyMesh;
	Mesh* gDummyMesh;
	Mesh* gHouseTwoMesh;

	//========Models========//
	Model* gDuck;
	Model* gTree[kTreeNum];
	Model* gTree2[kTreeNum];
	Model* gFloor;
	Model* gTeapot;
	Model* gWaterHouse;
	Model* gCube[kCubeNum];
	Model* gDecal;
	Model* gCrate;
	Model* gSphere;
	Model* gGround;
	Model* gMainHouse;
	Model* gWater;
	Model* gSky;
	Model* gSelectedModel;
	struct Light
	{
		Model*   model;
		CVector3 colour;
		float    strength;
	};
	bool gRenderPortal = true;
	Light gLights[NUM_LIGHTS];
	CVector3 gLightsPosition[NUM_LIGHTS];
	float gLightStrengths[NUM_LIGHTS];
	CVector3 gLightsColours[NUM_LIGHTS];
	Model* gInnScene;
	Model* gTroll;
	Model* gPortal;
	Model* gPortal2;
	Model* gHouseTwo;
	//========Water Values======//
	const float gWaterIncrement = 5.0f;
	const float gWaveIncrement = 0.1f;
	const float gWaveScale = 0.6f;
	
	const float waterSpeed = 1.0f;
	//==========Cameras=========//
	Camera* gCamera;
	CMatrix4x4* gCameraMatrix;
	Camera* gPortalCamera;
	Camera* gPortalCamera2;
	// Additional light information variables
	CVector3 gLight1Colour;
	float    gLight1Strength; // Allows the light to be stronger or weaker - also controls the light model scale

	CVector3 gLight2Colour ;
	float    gLight2Strength;
	//Teapot's lights
	//Colour Switching
	bool gColourSwitch;
	int	  gColourCount;
	enum ColourSelect {Red=1,Blue=2,Green=3};
	float gRed ;
	float gGreen;
	float gBlue ;
	float gColourRate;

	CVector3 gLight3Colour ;
	float    gLight3Strength ;

	CVector3 gLight4Colour;
	float    gLight4Strength;

	CVector3 gLight5Colour;
	float    gLight5Strength;

	CVector3 gLight6Colour;
	float    gLight6Strength;

	bool gLightPulsation;
	float gPulseRate;
	float gMinLightStrength;
	float gMaxLightStrength;
	bool gTextureCount;
	float gSpotlightConeAngle;
	CMatrix4x4 MatrixCopy;

	ID3D11ShaderResourceView* gNullSRV = nullptr;
	enum class CameraTypes
	{
		Free,
		FirtsPerson,
		ThirdPerson
	};
	CameraTypes gCurrentCamera;
	CVector3 gLastPosition;
	ModelManager();
	~ModelManager();
	bool LoadMeshes();
	void CreateModels();
	void InitialSceneSetup();
	void CreateCameras();
	void RenderDefaultModels();
	void RenderLights();
	void GetCamera(Camera* camera);
	void PrepareRenderModels( Camera *camera);
	void UpdateModels(float &frameTime);
};
extern ModelManager* ModelCreator;
#endif // _MODEMANAGER_H