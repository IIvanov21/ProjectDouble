#pragma once
#include "Definitions.h"

class TextureManager
{
public:
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

	D3D11_TEXTURE2D_DESC ShadowDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	int gShadowMapSize;

	//--------------------------------------------------------------------------------------
	// Textures
	//--------------------------------------------------------------------------------------

	ID3D11Resource*           gStoneDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gStoneDiffuseSpecularMapSRV = nullptr; 

	ID3D11Resource*           gTVDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gTVDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gDecalDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gDecalDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gCrateDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gCrateDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gBrickDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gBrickDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gGroundDiffuseSpecularMap = nullptr;
	ID3D11ShaderResourceView* gGroundDiffuseSpecularMapSRV = nullptr;

	ID3D11Resource*           gLightDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gLightDiffuseMapSRV = nullptr;

	ID3D11Resource*			  gWoodSpecularDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gWoodSpecularDiffuseMapSRV = nullptr;

	ID3D11Resource*			 gTeapotSpecularDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gTeapotSpecularDiffuseMapSRV = nullptr;

	ID3D11Resource*			 gTrollSpecularDiffuseMap = nullptr;
	ID3D11ShaderResourceView* gTrollSpecularDiffuseMapSRV = nullptr;
	//--------------------------------------------------------------------------------------
	// Texture Functionality
	//--------------------------------------------------------------------------------------
	TextureManager();
	bool LoadTextures();
	bool CreatePortalTextures();
	void ReleaseTextures();
};