#include "stdafx.h"
#include "RenderTarget.h"
#include "DirectXTex.h"

RenderTarget::RenderTarget(const D3D11Context& d3dContext) :
	m_D3DContext(d3dContext)
{}

RenderTarget::~RenderTarget(void)
{
	CleanUp();
}

void RenderTarget::CleanUp()
{
	//if (!m_ColorBufferSupplied)
	SafeRelease(m_pColor);

	//if (!m_DepthBufferSupplied)
	SafeRelease(m_pDepth);

	SafeRelease(m_pRenderTargetView);
	SafeRelease(m_pColorShaderResourceView);
	SafeRelease(m_pDepthShaderResourceView);
	SafeRelease(m_pDepthStencilView);
}

HRESULT RenderTarget::Create(RENDERTARGET_DESC desc)
{
	CleanUp();

	//Check if input is valid
	desc.IsValid();
	m_Desc = desc;

	m_ColorBufferSupplied = m_Desc.pColor != nullptr;
	m_DepthBufferSupplied = m_Desc.pDepth != nullptr;

	CreateColor();
	CreateDepth();

	return S_OK; //TODO: change > redundant with HANDLE_ERROR macros
}

HRESULT RenderTarget::CreateColor()
{
	if (m_Desc.pColor)
	{
		m_pColor = m_Desc.pColor;

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));

		m_pColor->GetDesc(&texDesc);
		m_Desc.width = texDesc.Width;
		m_Desc.height = texDesc.Height;
		m_Desc.colorFormat = texDesc.Format;

		m_Desc.enableColorSRV = (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE;
	}

	if (m_Desc.enableColorBuffer)
	{
		if (m_pColor == nullptr)
		{
			//RESOURCE
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));

			textureDesc.Width = m_Desc.width;
			textureDesc.Height = m_Desc.height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = m_Desc.colorFormat;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | ((m_Desc.enableColorSRV) ? D3D11_BIND_SHADER_RESOURCE : 0);
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = ((m_Desc.generateMipMaps_Color) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

			HANDLE_ERROR(m_D3DContext.pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pColor));
			m_Desc.pColor = m_pColor;
		}

		//RENDERTARGET SRV
		HANDLE_ERROR(m_D3DContext.pDevice->CreateRenderTargetView(m_pColor, nullptr, &m_pRenderTargetView));

		//SHADER SRV
		if (m_Desc.enableColorSRV)
		{
			HANDLE_ERROR(m_D3DContext.pDevice->CreateShaderResourceView(m_pColor, nullptr, &m_pColorShaderResourceView));
		}
	}
	else
	{
		SafeRelease(m_pColor);
		SafeRelease(m_pRenderTargetView);
		SafeRelease(m_pColorShaderResourceView);
	}

	return S_OK;
}

DXGI_FORMAT RenderTarget::GetResourceFormat_Depth(DXGI_FORMAT sourceFormat)
{
	DXGI_FORMAT resourceFormat = {};
	switch (sourceFormat)
	{
	case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
		resourceFormat = DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
		resourceFormat = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
		resourceFormat = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		resourceFormat = DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS;
		break;
	default:
		Logger::LogError(L"RenderTarget::GetDepthResourceFormat(...) > Format not supported!");
		break;
	}

	return resourceFormat;
}

DXGI_FORMAT RenderTarget::GetShaderResourceViewFormat_Depth(DXGI_FORMAT sourceFormat)
{
	DXGI_FORMAT srvFormat = {};
	switch (sourceFormat)
	{
	case DXGI_FORMAT::DXGI_FORMAT_D16_UNORM:
		srvFormat = DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT:
		srvFormat = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT:
		srvFormat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		srvFormat = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		break;
	default:
		Logger::LogError(L"RenderTarget::GetDepthSRVFormat(...) > Format not supported!");
		break;
	}

	return srvFormat;
}

HRESULT RenderTarget::CreateDepth()
{
	if (m_Desc.pDepth)
	{
		m_pDepth = m_Desc.pDepth;

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));

		m_pDepth->GetDesc(&texDesc);
		m_Desc.width = texDesc.Width;
		m_Desc.height = texDesc.Height;
		m_Desc.depthFormat = texDesc.Format;
	}

	if (m_Desc.enableDepthBuffer)
	{
		if (m_pDepth == nullptr)
		{
			//RESOURCE
			D3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));

			textureDesc.Width = m_Desc.width;
			textureDesc.Height = m_Desc.height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = GetResourceFormat_Depth(m_Desc.depthFormat);
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | ((m_Desc.enableDepthSRV) ? D3D11_BIND_SHADER_RESOURCE : 0);
			textureDesc.MiscFlags = 0;

			HANDLE_ERROR(m_D3DContext.pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pDepth));
			m_Desc.pDepth = m_pDepth;
		}

		//DEPTHSTENCIL VIEW
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));

		descDSV.Format = m_Desc.depthFormat;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		HANDLE_ERROR(m_D3DContext.pDevice->CreateDepthStencilView(m_pDepth, &descDSV, &m_pDepthStencilView));

		//SHADER SRV
		if (m_Desc.enableDepthSRV)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc;
			ZeroMemory(&depthSrvDesc, sizeof(depthSrvDesc));

			depthSrvDesc.Format = GetShaderResourceViewFormat_Depth(m_Desc.depthFormat);
			depthSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			depthSrvDesc.Texture2D.MipLevels = 1;
			depthSrvDesc.Texture2D.MostDetailedMip = 0;

			HANDLE_ERROR(m_D3DContext.pDevice->CreateShaderResourceView(m_pDepth, &depthSrvDesc, &m_pDepthShaderResourceView));
		}
	}
	else
	{
		SafeRelease(m_pDepth);
		SafeRelease(m_pDepthStencilView);
		SafeRelease(m_pDepthShaderResourceView);
	}

	return S_OK;
}

ID3D11ShaderResourceView* RenderTarget::GetColorShaderResourceView() const
{
	if (!m_Desc.enableColorSRV)
		Logger::LogError(L"RenderTarget::GetShaderResourceView(...) > No COLOR SRV created during Creation. (Make sure to enable \'EnableColorSRV\' during RT Creation)");

	return m_pColorShaderResourceView;
}

ID3D11ShaderResourceView* RenderTarget::GetDepthShaderResourceView() const
{
	if (!m_Desc.enableDepthSRV)
		Logger::LogError(L"RenderTarget::GetDepthShaderResourceView(...) > No DEPTH SRV created during Creation. (Make sure to enable \'EnableDepthSRV\' during RT Creation)");

	return m_pDepthShaderResourceView;
}

void RenderTarget::SaveTextureToFile(const SceneContext& sceneContext, const std::wstring& fileName)
{
	// Save the pixel data to a file using the DirectXTex library
	ScratchImage image;
	if (SUCCEEDED(CaptureTexture(sceneContext.d3dContext.pDevice,
		sceneContext.d3dContext.pDeviceContext, m_pDepth, image)))
	{
		auto metadata = image.GetMetadata();
		metadata.format = DXGI_FORMAT_R32_FLOAT;
		HRESULT hr = SaveToDDSFile(image.GetImages(), image.GetImageCount(),
			metadata, DDS_FLAGS_NONE, fileName.c_str());
		if (FAILED(hr))
			Logger::LogWarning(L"RenderTarget::SaveTextureToFile(...) > Failed to save texture to file! (Error: %s)", hr);
	}
	else
	{
		Logger::LogWarning(L"RenderTarget::SaveTextureToFile(...) > Failed to capture texture!");
	}
}

void RenderTarget::LoadShadowMapFromFile(const D3D11Context& d3dContext, const std::wstring& fileName)
{
	TexMetadata info{};
	info.width = m_Desc.width;
	info.height = m_Desc.height;
	info.format = m_Desc.depthFormat;
	info.miscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	ScratchImage image;
	HRESULT hr = LoadFromDDSFile(fileName.c_str(),
		DDS_FLAGS_NONE, &info, image);
	if (SUCCEEDED(hr))
	{
		auto meta{ image.GetMetadata() };
		meta.miscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		meta.miscFlags2 = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		meta.format = DXGI_FORMAT_R32_FLOAT;
		hr = CreateShaderResourceView(d3dContext.pDevice,
			image.GetImages(), image.GetImageCount(),
			meta, &m_pDepthShaderResourceView);
		

		if (FAILED(hr))
		{
			Logger::LogWarning(L"RenderTarget::GetDepthShaderResourceView(...) > Failed to create SRV from texture! (Error: %s)", hr);
		}
	}
	else
	{
		Logger::LogWarning(L"RenderTarget::GetDepthShaderResourceView(...) > Failed to load texture from file! (Error: %s)", hr);
	}
}

void RenderTarget::Clear(XMFLOAT4 clearColor) const
{
	if (m_Desc.enableColorBuffer)
		m_D3DContext.pDeviceContext->ClearRenderTargetView(GetRenderTargetView(), &clearColor.x);

	if (m_Desc.enableDepthBuffer)
		m_D3DContext.pDeviceContext->ClearDepthStencilView(GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
