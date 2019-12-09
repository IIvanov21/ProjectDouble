#pragma once
//#include "Definitions.h"
#include "TextureManager.h"
class ModelManager
{
public:
	//Constant Varialbles
	
	static const int   kCubeNum = 2;
	const float gLightOrbit = 20.0f;
	const float gLightOrbitSpeed = 0.7f;
	static const int NUM_LIGHTS = 6;
	float ColourMax = 1.0f;
	const int gWholeMesh = 0;
	const std::string MeshesMediaFolder = "./Media/Meshes/";
	//==========Meshes=========//
	Mesh* gCubeMesh;
	Mesh* gTrollMesh;
	Mesh* gDecalMesh;
	Mesh* gCrateMesh;
	Mesh* gSphereMesh;
	Mesh* gGroundMesh;
	Mesh* gLightMesh;
	Mesh* gPortalMesh;
	Mesh* gFloorMesh;
	Mesh* gTeapotMesh;
	Mesh* gInnMesh;
	Mesh* gWaterHouseMesh;
	Mesh* gMainHouseMesh;
	//========Models========//
	Model* gFloor;
	Model* gTeapot;
	Model* gWaterHouse;
	Model* gCube[kCubeNum];
	Model* gDecal;
	Model* gCrate;
	Model* gSphere;
	Model* gGround;
	Model* gMainHouse;
	struct Light
	{
		Model*   model;
		CVector3 colour;
		float    strength;
	};
	Light gLights[NUM_LIGHTS];
	CVector3 gLightsPosition[NUM_LIGHTS];
	float gLightStrengths[NUM_LIGHTS];
	CVector3 gLightsColours[NUM_LIGHTS];
	Model* gInnScene;
	Model* gTroll;
	Model* gPortal;
	Model* gPortal2;
	//==========Cameras=========//
	Camera* gCamera;
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


	ModelManager();
	~ModelManager();
	bool LoadMeshes();
	void CreateModels();
	void InitialSceneSetup();
	void CreateCameras();
	void PrepareRenderModels(ID3D11DeviceContext* gD3DContext, TextureManager *GetTexture);
	void UpdateModels(float &frameTime, PerFrameConstants &gPerFrameConstants);
};