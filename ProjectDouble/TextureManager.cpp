#include "TextureManager.h"
TextureManager::TextureManager()
{
	gPortalWidth = 2000;
	gPortalHeight = 2000;
	gShadowMapSize = 1024;
	
}
bool TextureManager::LoadTextures()// Load all textures from image
{
	if (!LoadTexture(TextureMediaFolder + "CubeMapB.jpg", &gSkyDiffuseSpecularMap, &gSkyDiffuseSpecularMapSRV) ||
		!LoadTexture(TextureMediaFolder + "StoneDiffuseSpecular.dds", &gStoneDiffuseSpecularMap, &gStoneDiffuseSpecularMapSRV) ||
		!LoadTexture(TextureMediaFolder + "Moogle.png", &gDecalDiffuseSpecularMap, &gDecalDiffuseSpecularMapSRV) ||
		!LoadTexture(TextureMediaFolder + "CargoA.dds", &gCrateDiffuseSpecularMap, &gCrateDiffuseSpecularMapSRV) ||
		!LoadTexture(TextureMediaFolder + "Brick1.jpg", &gBrickDiffuseSpecularMap, &gBrickDiffuseSpecularMapSRV) ||
		!LoadTexture(TextureMediaFolder + "MountGrassTexture2.jpg", &gGroundDiffuseSpecularMap, &gGroundDiffuseSpecularMapSRV) ||
		!LoadTexture(TextureMediaFolder + "Flare.jpg", &gLightDiffuseMap, &gLightDiffuseMapSRV) ||
		!LoadTexture(TextureMediaFolder + "StoneDiffuseSpecular.dds", &gTeapotSpecularDiffuseMap, &gTeapotSpecularDiffuseMapSRV) ||
		!LoadTexture(TextureMediaFolder + "WoodDiffuseSpecular.dds", &gWoodSpecularDiffuseMap, &gWoodSpecularDiffuseMapSRV)||
		!LoadTexture(TextureMediaFolder + "TrollDiffuseSpecular.dds", &gTrollSpecularDiffuseMap,&gTrollSpecularDiffuseMapSRV)||
		!LoadTexture(TextureMediaFolder + "tv.dds", &gTVDiffuseSpecularMap,&gTVDiffuseSpecularMapSRV) ||
		!LoadTexture(TextureMediaFolder + "greyTexture.jpg", &gGreyDiffuseSpecularMap, &gGreyDiffuseSpecularMapSRV)||
		!LoadTexture(TextureMediaFolder + "WaterNormalHeight.png", &gWaterNormalMap, &gWaterNormalMapSRV))
	{
		gLastError = "Error loading textures";
		return false;
	}
	return true;

}

bool TextureManager::CreateTextures()//Create all textures that are not laoded from image file
{
	HRESULT hr = S_OK;


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


	//**************************************
	// Water Rendering Resources
	//**************************************
	// Using a helper function to load textures from files above. Here we create the scene texture manually
	// as we are creating a special kind of texture (one that we can render to). Many settings to prepare:
	gWaterDesc = {};
	gWaterDesc.Width = gViewportWidth;  // Reflection / refraction / water surface textures are full screen size - could experiment with making them smaller
	gWaterDesc.Height = gViewportHeight;
	gWaterDesc.MipLevels = 1; // No mip-maps when rendering to textures (or we would have to render every level)
	gWaterDesc.ArraySize = 1;
	gWaterDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA texture (8-bits each)
	gWaterDesc.SampleDesc.Count = 1;
	gWaterDesc.SampleDesc.Quality = 0;
	gWaterDesc.Usage = D3D11_USAGE_DEFAULT;
	gWaterDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // IMPORTANT: Indicate we will use texture as render target, and pass it to shaders
	gWaterDesc.CPUAccessFlags = 0;
	gWaterDesc.MiscFlags = 0;
	if (FAILED(gD3DDevice->CreateTexture2D(&gWaterDesc, NULL, &gReflection)))
	{
		gLastError = "Error creating reflection texture";
		return false;
	}

	// We created the relection texture above, now we get a "view" of it as a render target, i.e. get a special pointer to the texture that we use when rendering to it
	if (FAILED(gD3DDevice->CreateRenderTargetView(gReflection, NULL, &gReflectionRenderTarget)))
	{
		gLastError = "Error creating reflection render target view";
		return false;
	}

	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
	gWaterShaderDesc = {};
	gWaterShaderDesc.Format = gWaterDesc.Format;
	gWaterShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	gWaterShaderDesc.Texture2D.MostDetailedMip = 0;
	gWaterShaderDesc.Texture2D.MipLevels = 1;
	if (FAILED(gD3DDevice->CreateShaderResourceView(gReflection, &gWaterShaderDesc, &gReflectionSRV)))
	{
		gLastError = "Error creating reflection shader resource view";
		return false;
	}



	// Same again for refraction texture. Structures already set up so can save a few lines
	if (FAILED(gD3DDevice->CreateTexture2D(&gWaterDesc, NULL, &gRefraction)))
	{
		gLastError = "Error creating refraction texture";
		return false;
	}
	if (FAILED(gD3DDevice->CreateRenderTargetView(gRefraction, NULL, &gRefractionRenderTarget)))
	{
		gLastError = "Error creating refraction render target view";
		return false;
	}
	if (FAILED(gD3DDevice->CreateShaderResourceView(gRefraction, &gWaterShaderDesc, &gRefractionSRV)))
	{
		gLastError = "Error creating refraction shader resource view";
		return false;
	}


	// Same again for the water height texture.
	gWaterDesc.Format = gWaterShaderDesc.Format = DXGI_FORMAT_R32_FLOAT; // Water surface height is just one value per pixel - so texture only needs red channel using a 32-bit float
	if (FAILED(gD3DDevice->CreateTexture2D(&gWaterDesc, NULL, &gWaterHeight)))
	{
		gLastError = "Error creating water height texture";
		return false;
	}
	if (FAILED(gD3DDevice->CreateRenderTargetView(gWaterHeight, NULL, &gWaterHeightRenderTarget)))
	{
		gLastError = "Error creating water height render target view";
		return false;
	}
	if (FAILED(gD3DDevice->CreateShaderResourceView(gWaterHeight, &gWaterShaderDesc, &gWaterHeightSRV)))
	{
		gLastError = "Error creating water height shader resource view";
		return false;
	}
	//PortalTexture
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


	//**** Create Portal Depth Buffer ****

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
	return true;
}

void TextureManager::ReleaseTextures()//Release all texture and prepare for use
{
	if (gSkyDiffuseSpecularMapSRV)     gSkyDiffuseSpecularMapSRV->Release();
	if (gSkyDiffuseSpecularMap)        gSkyDiffuseSpecularMap->Release();
	if (gWaterNormalMap         ) gWaterNormalMap->Release();
	if (gWaterNormalMapSRV      ) gWaterNormalMapSRV->Release();
	if (gWaterHeight            ) gWaterHeight->Release();
	if (gWaterHeightSRV         ) gWaterHeightSRV->Release();
	if (gWaterHeightRenderTarget) gWaterHeightRenderTarget->Release();
	if (gReflection             ) gReflection->Release();
	if (gReflectionSRV          ) gReflectionSRV->Release();
	if (gReflectionRenderTarget ) gReflectionRenderTarget->Release();
	if (gRefraction             ) gRefraction->Release();
	if (gRefractionSRV          ) gRefractionSRV->Release();
	if (gRefractionRenderTarget ) gRefractionRenderTarget->Release();


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

	if (gGreyDiffuseSpecularMap)gGreyDiffuseSpecularMap->Release();
	if (gGreyDiffuseSpecularMapSRV)gGreyDiffuseSpecularMapSRV->Release();

}