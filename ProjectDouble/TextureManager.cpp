#include "TextureManager.h"
TextureManager::TextureManager()
{
	gPortalWidth = 2000;
	gPortalHeight = 2000;
	gShadowMapSize = 1024;
}
bool TextureManager::LoadTextures()// Load all textures from image
{
	if (!LoadTexture("StoneDiffuseSpecular.dds", &gStoneDiffuseSpecularMap, &gStoneDiffuseSpecularMapSRV) ||
		!LoadTexture("Moogle.png", &gDecalDiffuseSpecularMap, &gDecalDiffuseSpecularMapSRV) ||
		!LoadTexture("CargoA.dds", &gCrateDiffuseSpecularMap, &gCrateDiffuseSpecularMapSRV) ||
		!LoadTexture("Brick1.jpg", &gBrickDiffuseSpecularMap, &gBrickDiffuseSpecularMapSRV) ||
		!LoadTexture("MountGrassTexture2.jpg", &gGroundDiffuseSpecularMap, &gGroundDiffuseSpecularMapSRV) ||
		!LoadTexture("Flare.jpg", &gLightDiffuseMap, &gLightDiffuseMapSRV) ||
		!LoadTexture("StoneDiffuseSpecular.dds", &gTeapotSpecularDiffuseMap, &gTeapotSpecularDiffuseMapSRV) ||
		!LoadTexture("WoodDiffuseSpecular.dds", &gWoodSpecularDiffuseMap, &gWoodSpecularDiffuseMapSRV)||
		!LoadTexture("TrollDiffuseSpecular.dds", &gTrollSpecularDiffuseMap,&gTrollSpecularDiffuseMapSRV)||
		!LoadTexture("tv.dds", &gTVDiffuseSpecularMap,&gTVDiffuseSpecularMapSRV))
	{
		gLastError = "Error loading textures";
		return false;
	}
	return true;
}

bool TextureManager::CreatePortalTextures()//Create all textures that are not laoded from image file
{
	portalDesc.Width = gPortalWidth;  // Size of the portal texture determines its quality
	portalDesc.Height = gPortalHeight;
	portalDesc.MipLevels = 1; // No mip-maps when rendering to textures (or we would have to render every level)
	portalDesc.ArraySize = 1;
	portalDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA texture (8-bits each)
	portalDesc.SampleDesc.Count = 1;
	portalDesc.SampleDesc.Quality = 0;
	portalDesc.Usage = D3D11_USAGE_DEFAULT;
	portalDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE; // IMPORTANT: Indicate we will use texture as render target, and pass it to shaders
	portalDesc.CPUAccessFlags = 0;
	portalDesc.MiscFlags = 0;
	if (FAILED(gD3DDevice->CreateTexture2D(&portalDesc, NULL, &gPortalTexture)))
	{
		gLastError = "Error creating portal texture";
		return false;
	}

	// We created the portal texture above, now we get a "view" of it as a render target, i.e. get a special pointer to the texture that
	// we use when rendering to it (see RenderScene function below)
	if (FAILED(gD3DDevice->CreateRenderTargetView(gPortalTexture, NULL, &gPortalRenderTarget)))
	{
		gLastError = "Error creating portal render target view";
		return false;
	}

	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
	srDesc.Format = portalDesc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;
	if (FAILED(gD3DDevice->CreateShaderResourceView(gPortalTexture, &srDesc, &gPortalTextureSRV)))
	{
		gLastError = "Error creating portal shader resource view";
		return false;
	}


	//**** Create Portal Depth Buffer ****//

	// We also need a depth buffer to go with our portal
	//**** This depth buffer can be shared with any other portals of the same size
	portalDesc = {};
	portalDesc.Width = gPortalWidth;
	portalDesc.Height = gPortalHeight;
	portalDesc.MipLevels = 1;
	portalDesc.ArraySize = 1;
	portalDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth buffers contain a single float per pixel
	portalDesc.SampleDesc.Count = 1;
	portalDesc.SampleDesc.Quality = 0;
	portalDesc.Usage = D3D11_USAGE_DEFAULT;
	portalDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	portalDesc.CPUAccessFlags = 0;
	portalDesc.MiscFlags = 0;
	if (FAILED(gD3DDevice->CreateTexture2D(&portalDesc, NULL, &gPortalDepthStencil)))
	{
		gLastError = "Error creating portal depth stencil texture";
		return false;
	}

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	portalDescDSV.Format = portalDesc.Format;
	portalDescDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	portalDescDSV.Texture2D.MipSlice = 0;
	portalDescDSV.Flags = 0;
	if (FAILED(gD3DDevice->CreateDepthStencilView(gPortalDepthStencil, &portalDescDSV, &gPortalDepthStencilView)))
	{
		gLastError = "Error creating portal depth stencil view";
		return false;
	}
	//Shadows
	ShadowDesc = {};
	ShadowDesc.Width = gShadowMapSize; // Size of the shadow map determines quality / resolution of shadows
	ShadowDesc.Height = gShadowMapSize;
	ShadowDesc.MipLevels = 1; // 1 level, means just the main texture, no additional mip-maps. Usually don't use mip-maps when rendering to textures (or we would have to render every level)
	ShadowDesc.ArraySize = 1;
	ShadowDesc.Format = DXGI_FORMAT_R32_TYPELESS; // The shadow map contains a single 32-bit value [tech gotcha: have to say typeless because depth buffer and shaders see things slightly differently]
	ShadowDesc.SampleDesc.Count = 1;
	ShadowDesc.SampleDesc.Quality = 0;
	ShadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ShadowDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE; // Indicate we will use texture as a depth buffer and also pass it to shaders
	ShadowDesc.CPUAccessFlags = 0;
	ShadowDesc.MiscFlags = 0;
	if (FAILED(gD3DDevice->CreateTexture2D(&ShadowDesc, NULL, &gShadowMap1Texture)))
	{
		gLastError = "Error creating shadow map texture";
		return false;
	}

	if (FAILED(gD3DDevice->CreateTexture2D(&ShadowDesc, NULL, &gShadowMap2Texture)))
	{
		gLastError = "Error creating shadow map texture";
		return false;
	}
	dsvDesc = {};
	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // See "tech gotcha" above. The depth buffer sees each pixel as a "depth" float
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;
	if (FAILED(gD3DDevice->CreateDepthStencilView(gShadowMap1Texture, &dsvDesc, &gShadowMap1DepthStencil)))
	{
		gLastError = "Error creating shadow map depth stencil view";
		return false;
	}
	

	if (FAILED(gD3DDevice->CreateDepthStencilView(gShadowMap2Texture, &dsvDesc, &gShadowMap2DepthStencil)))
	{
		gLastError = "Error creating shadow map depth stencil view";
		return false;
	}

	srvDesc = {};
	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // See "tech gotcha" above. The shaders see textures as colours, so shadow map pixels are not seen as depths
										   // but rather as "red" floats (one float taken from RGB). Although the shader code will use the value as a depth
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	if (FAILED(gD3DDevice->CreateShaderResourceView(gShadowMap1Texture, &srvDesc, &gShadowMap1SRV)))
	{
		gLastError = "Error creating shadow map shader resource view";
		return false;
	}
	

	if (FAILED(gD3DDevice->CreateShaderResourceView(gShadowMap2Texture, &srvDesc, &gShadowMap2SRV)))
	{
		gLastError = "Error creating shadow map shader resource view";
		return false;
	}
	return true;
}

void TextureManager::ReleaseTextures()//Release all texture and prepare for use
{
	if (gPortalDepthStencilView)  gPortalDepthStencilView->Release();
	if (gPortalDepthStencil)      gPortalDepthStencil->Release();
	if (gPortalTextureSRV)        gPortalTextureSRV->Release();
	if (gPortalRenderTarget)      gPortalRenderTarget->Release();
	if (gPortalTexture)           gPortalTexture->Release();

	if (gLightDiffuseMapSRV)            gLightDiffuseMapSRV->Release();
	if (gLightDiffuseMap)               gLightDiffuseMap->Release();
	if (gGroundDiffuseSpecularMapSRV)   gGroundDiffuseSpecularMapSRV->Release();
	if (gGroundDiffuseSpecularMap)      gGroundDiffuseSpecularMap->Release();
	if (gBrickDiffuseSpecularMapSRV)    gBrickDiffuseSpecularMapSRV->Release();
	if (gBrickDiffuseSpecularMap)       gBrickDiffuseSpecularMap->Release();
	if (gCrateDiffuseSpecularMapSRV)    gCrateDiffuseSpecularMapSRV->Release();
	if (gCrateDiffuseSpecularMap)       gCrateDiffuseSpecularMap->Release();
	if (gDecalDiffuseSpecularMapSRV)    gDecalDiffuseSpecularMapSRV->Release();
	if (gDecalDiffuseSpecularMap)       gDecalDiffuseSpecularMap->Release();
	if (gStoneDiffuseSpecularMapSRV)    gStoneDiffuseSpecularMapSRV->Release();
	if (gStoneDiffuseSpecularMap)       gStoneDiffuseSpecularMap->Release();
	if (gTeapotSpecularDiffuseMap)		gTeapotSpecularDiffuseMap->Release();
	if (gTeapotSpecularDiffuseMapSRV)   gTeapotSpecularDiffuseMapSRV->Release();
	if (gWoodSpecularDiffuseMap)		gWoodSpecularDiffuseMap->Release();
	if (gWoodSpecularDiffuseMapSRV)		gWoodSpecularDiffuseMapSRV->Release();

	if (gShadowMap1DepthStencil)		gShadowMap1DepthStencil->Release();
	if (gShadowMap1SRV)					gShadowMap1SRV->Release();
	if (gShadowMap1Texture)				gShadowMap1Texture->Release();

	if (gShadowMap2DepthStencil)		gShadowMap2DepthStencil->Release();
	if (gShadowMap2SRV)					gShadowMap2SRV->Release();
	if (gShadowMap2Texture)			    gShadowMap2Texture->Release();

	if (gTrollSpecularDiffuseMap)		gTrollSpecularDiffuseMap->Release();
	if (gTrollSpecularDiffuseMapSRV)     gTrollSpecularDiffuseMapSRV->Release();

	if (gTVDiffuseSpecularMap)gTVDiffuseSpecularMap->Release();
	if (gTVDiffuseSpecularMapSRV)gTVDiffuseSpecularMapSRV->Release();
}