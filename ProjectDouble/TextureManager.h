#pragma once
#include "Definitions.h"

#ifndef _TEXTURE_H_INCLUDED_
#define _TEXTURE_H_INCLUDED_
enum TextureTypes
{
	Diffuse,
	Speuclar,
	None
};
class TextureManager
{
public:
	const std::string TextureMediaFolder = "./Media/Textures/";
	unsigned int ViewportWidth;
	unsigned int ViewportHeight;

	ID3D11Texture2D* gSceneTexture = nullptr;
	ID3D11RenderTargetView* gSceneRenderTarget = nullptr;
	ID3D11ShaderResourceView* gSceneTextureSRV = nullptr;

	//--------------------------------------------------------------------------------------
	// Portal Textures
	//--------------------------------------------------------------------------------------
	ID3D11Texture2D*          gPortalTexture = nullptr;
	ID3D11RenderTargetView*   gPortalRenderTarget = nullptr;
	ID3D11ShaderResourceView* gPortalTextureSRV = nullptr; 

	//Portal 2
	ID3D11Texture2D*          gPortalTexture2 = nullptr; 
	ID3D11RenderTargetView*   gPortalRenderTarget2 = nullptr; 
	ID3D11ShaderResourceView* gPortalTextureSRV2 = nullptr;


	ID3D11Texture2D*        gPortalDepthStencil = nullptr; 
	ID3D11DepthStencilView* gPortalDepthStencilView = nullptr; 

	//Portal 2
	ID3D11Texture2D*        gPortalDepthStencil2 = nullptr; 
	ID3D11DepthStencilView* gPortalDepthStencilView2 = nullptr; 
	
	//Portal Textures
	D3D11_TEXTURE2D_DESC portalDesc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc = {};
	D3D11_DEPTH_STENCIL_VIEW_DESC portalDescDSV = {};

	int gPortalWidth;
	int gPortalHeight;
	//Shadow Textures
	ID3D11Texture2D*          gShadowMap1Texture = nullptr;
	ID3D11DepthStencilView*   gShadowMap1DepthStencil = nullptr;
	ID3D11ShaderResourceView* gShadowMap1SRV = nullptr;

	ID3D11Texture2D*          gShadowMap2Texture = nullptr; 
	ID3D11DepthStencilView*   gShadowMap2DepthStencil = nullptr;
	ID3D11ShaderResourceView* gShadowMap2SRV = nullptr;

	ID3D11Texture2D* gATexture;
	ID3D11RenderTargetView* gATextureRenderTarget;
	ID3D11ShaderResourceView* gATextureSRV;

	ID3D11Texture2D* gBTexture;
	ID3D11RenderTargetView* gBTextureRenderTarget;
	ID3D11ShaderResourceView* gBTextureSRV;

	ID3D11Texture2D* gCTexture;
	ID3D11RenderTargetView* gCTextureRenderTarget;
	ID3D11ShaderResourceView* gCTextureSRV;

	D3D11_TEXTURE2D_DESC ShadowDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	int gShadowMapSize;

	//--------------------------------------------------------------------------------------
	// Textures
	//--------------------------------------------------------------------------------------

	ID3D11Resource*           gStoneDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gStoneDiffuseSpecularMapSRV = nullptr; 

	ID3D11Resource*			  gGreyDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gGreyDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gTVDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gTVDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gDecalDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gDecalDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gCrateDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gCrateDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gBrickDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gBrickDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*			  gSkyDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gSkyDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gGroundDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gGroundDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gLightDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gLightDiffuseMapSRV = nullptr;

	ID3D11Resource*			  gWoodSpecularDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gWoodSpecularDiffuseMapSRV = nullptr;

	ID3D11Resource*			  gTeapotSpecularDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gTeapotSpecularDiffuseMapSRV = nullptr;

	ID3D11Resource*			  gTrollSpecularDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gTrollSpecularDiffuseMapSRV = nullptr;                  

	//***************************
	// Water Rendering Resources
	//***************************
	//// Water textures / render targets
	D3D11_TEXTURE2D_DESC		gWaterDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC gWaterDepthDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC gWaterShaderDesc;
	ID3D11Resource*				gWaterNormalMap = nullptr;          // The normal/height map used for the waves on the surface of the water
	ID3D11ShaderResourceView*	gWaterNormalMapSRV = nullptr;       // --"--
	ID3D11Texture2D*			gWaterHeight = nullptr;            // The height of the water above the floor at each pixel - a data texture rendered each frame
	ID3D11ShaderResourceView*	gWaterHeightSRV = nullptr;          // --"--  Used to detect the boundary between above water and underwater
	ID3D11RenderTargetView*		gWaterHeightRenderTarget = nullptr; // --"--
	ID3D11Texture2D*			gReflection = nullptr;             // The reflected scene is rendered into this texture 
	ID3D11ShaderResourceView*	gReflectionSRV = nullptr;           // --"-- For reading the texture in shaders
	ID3D11RenderTargetView*		gReflectionRenderTarget = nullptr;  // --"-- For writing to the texture as a render target
	ID3D11Texture2D*			gRefraction = nullptr;             // The refracted scene is rendered into this texture
	ID3D11ShaderResourceView*	gRefractionSRV = nullptr;           // --"-- For reading the texture in shaders
	ID3D11RenderTargetView*		gRefractionRenderTarget = nullptr;  // --"-- For writing to the texture as a render target

	//***************************
	//--------------------------------------------------------------------------------------
	// Texture Functionality
	//--------------------------------------------------------------------------------------
	TextureManager();
	bool LoadTextures();
	bool CreateTextures();
	void ReleaseTextures();
};

extern TextureManager* TextureCreator;
#endif //TEXTURE_H