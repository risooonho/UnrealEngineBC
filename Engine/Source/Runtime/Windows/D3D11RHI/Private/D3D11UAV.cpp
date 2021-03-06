// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.


#include "D3D11RHIPrivate.h"

FUnorderedAccessViewRHIRef FD3D11DynamicRHI::RHICreateUnorderedAccessView(FStructuredBufferRHIParamRef StructuredBufferRHI, bool bUseUAVCounter, bool bAppendBuffer)
{
	DYNAMIC_CAST_D3D11RESOURCE(StructuredBuffer,StructuredBuffer);

	D3D11_BUFFER_DESC BufferDesc;
	StructuredBuffer->Resource->GetDesc(&BufferDesc);

	const bool bByteAccessBuffer = (BufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS) != 0;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;

	if (BufferDesc.MiscFlags & D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS)
	{
		UAVDesc.Format = DXGI_FORMAT_R32_UINT;
	}
	else if (bByteAccessBuffer)
	{
		UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	}
		
	UAVDesc.Buffer.FirstElement = 0;

	// For byte access buffers and indirect draw argument buffers, GetDesc returns a StructureByteStride of 0 even though we created it with 4
	const uint32 EffectiveStride = BufferDesc.StructureByteStride == 0 ? 4 : BufferDesc.StructureByteStride;
	UAVDesc.Buffer.NumElements = BufferDesc.ByteWidth / EffectiveStride;
	UAVDesc.Buffer.Flags = 0;

	if (bUseUAVCounter)
	{
		UAVDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_COUNTER;
	}

	if (bAppendBuffer)
	{
		UAVDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_APPEND;
	}

	if (bByteAccessBuffer)
	{
		UAVDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_RAW;
	}

	TRefCountPtr<ID3D11UnorderedAccessView> UnorderedAccessView;
	VERIFYD3D11RESULT(Direct3DDevice->CreateUnorderedAccessView(StructuredBuffer->Resource,&UAVDesc,(ID3D11UnorderedAccessView**)UnorderedAccessView.GetInitReference()));

	return new FD3D11UnorderedAccessView(UnorderedAccessView,StructuredBuffer);
}

FUnorderedAccessViewRHIRef FD3D11DynamicRHI::RHICreateUnorderedAccessView(FTextureRHIParamRef TextureRHI)
{
	FD3D11TextureBase* Texture = GetD3D11TextureFromRHITexture(TextureRHI);
	
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;

	if (TextureRHI->GetTexture3D() != NULL)
	{
		FD3D11Texture3D* Texture3D = (FD3D11Texture3D*)Texture;
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		UAVDesc.Texture3D.MipSlice = 0;
		UAVDesc.Texture3D.FirstWSlice = 0;
		UAVDesc.Texture3D.WSize = Texture3D->GetSizeZ();
	}
	else
	{
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		UAVDesc.Texture2D.MipSlice = 0;
	}
	
	UAVDesc.Format = FindShaderResourceDXGIFormat((DXGI_FORMAT)GPixelFormats[TextureRHI->GetFormat()].PlatformFormat, false);

	TRefCountPtr<ID3D11UnorderedAccessView> UnorderedAccessView;
	VERIFYD3D11RESULT(Direct3DDevice->CreateUnorderedAccessView(Texture->GetResource(),&UAVDesc,(ID3D11UnorderedAccessView**)UnorderedAccessView.GetInitReference()));

	return new FD3D11UnorderedAccessView(UnorderedAccessView,Texture);
}

FUnorderedAccessViewRHIRef FD3D11DynamicRHI::RHICreateUnorderedAccessView(FVertexBufferRHIParamRef VertexBufferRHI, uint8 Format)
{
	DYNAMIC_CAST_D3D11RESOURCE(VertexBuffer,VertexBuffer);

	D3D11_BUFFER_DESC BufferDesc;
	VertexBuffer->Resource->GetDesc(&BufferDesc);

	const bool bByteAccessBuffer = (BufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS) != 0;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Format = FindUnorderedAccessDXGIFormat((DXGI_FORMAT)GPixelFormats[Format].PlatformFormat);
	UAVDesc.Buffer.FirstElement = 0;

	UAVDesc.Buffer.NumElements = BufferDesc.ByteWidth / GPixelFormats[Format].BlockBytes;
	UAVDesc.Buffer.Flags = 0;

	if (bByteAccessBuffer)
	{
		UAVDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_RAW;
		UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	}

	TRefCountPtr<ID3D11UnorderedAccessView> UnorderedAccessView;
	VERIFYD3D11RESULT(Direct3DDevice->CreateUnorderedAccessView(VertexBuffer->Resource,&UAVDesc,(ID3D11UnorderedAccessView**)UnorderedAccessView.GetInitReference()));

	return new FD3D11UnorderedAccessView(UnorderedAccessView,VertexBuffer);
}

FShaderResourceViewRHIRef FD3D11DynamicRHI::RHICreateShaderResourceView(FStructuredBufferRHIParamRef StructuredBufferRHI)
{
	DYNAMIC_CAST_D3D11RESOURCE(StructuredBuffer,StructuredBuffer);

	D3D11_BUFFER_DESC BufferDesc;
	StructuredBuffer->Resource->GetDesc(&BufferDesc);

	const bool bByteAccessBuffer = (BufferDesc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS) != 0;

	// Create a Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;

	if ( bByteAccessBuffer )
	{
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		SRVDesc.BufferEx.NumElements = BufferDesc.ByteWidth / 4;
		SRVDesc.BufferEx.FirstElement = 0;
		SRVDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		SRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	}
	else
	{
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		SRVDesc.Buffer.FirstElement = 0;
	    SRVDesc.Buffer.NumElements = BufferDesc.ByteWidth / BufferDesc.StructureByteStride;
		SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	}

	TRefCountPtr<ID3D11ShaderResourceView> ShaderResourceView;
	VERIFYD3D11RESULT(Direct3DDevice->CreateShaderResourceView(StructuredBuffer->Resource, &SRVDesc, (ID3D11ShaderResourceView**)ShaderResourceView.GetInitReference()));

	return new FD3D11ShaderResourceView(ShaderResourceView,StructuredBuffer);
}

FShaderResourceViewRHIRef FD3D11DynamicRHI::RHICreateShaderResourceView(FVertexBufferRHIParamRef VertexBufferRHI, uint32 Stride, uint8 Format)
{
	DYNAMIC_CAST_D3D11RESOURCE(VertexBuffer,VertexBuffer);
	check(VertexBuffer);
	check(VertexBuffer->Resource);

	D3D11_BUFFER_DESC BufferDesc;
	VertexBuffer->Resource->GetDesc(&BufferDesc);

	// Create a Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	FMemory::MemZero(SRVDesc);
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.FirstElement = 0;

	SRVDesc.Format = FindShaderResourceDXGIFormat((DXGI_FORMAT)GPixelFormats[Format].PlatformFormat,false);
	SRVDesc.Buffer.NumElements = BufferDesc.ByteWidth / Stride;
	TRefCountPtr<ID3D11ShaderResourceView> ShaderResourceView;
	
	HRESULT hr = Direct3DDevice->CreateShaderResourceView(VertexBuffer->Resource, &SRVDesc, (ID3D11ShaderResourceView**)ShaderResourceView.GetInitReference());
	if (FAILED(hr))
	{
		if (hr == E_OUTOFMEMORY)
		{
			// There appears to be a driver bug that causes SRV creation to fail with an OOM error and then succeed on the next call.
			hr = Direct3DDevice->CreateShaderResourceView(VertexBuffer->Resource, &SRVDesc, (ID3D11ShaderResourceView**)ShaderResourceView.GetInitReference());
		}
		if (FAILED(hr))
		{
			UE_LOG(LogD3D11RHI,Error,TEXT("Failed to create shader resource view for vertex buffer: ByteWidth=%d NumElements=%d"),BufferDesc.ByteWidth,BufferDesc.ByteWidth / Stride);
			VerifyD3D11Result(hr,"Direct3DDevice->CreateShaderResourceView",__FILE__,__LINE__,Direct3DDevice);
		}
	}

	return new FD3D11ShaderResourceView(ShaderResourceView,VertexBuffer);
}

void FD3D11DynamicRHI::RHIClearUAV(FUnorderedAccessViewRHIParamRef UnorderedAccessViewRHI, const uint32* Values)
{
	DYNAMIC_CAST_D3D11RESOURCE(UnorderedAccessView,UnorderedAccessView);
	
	Direct3DDeviceIMContext->ClearUnorderedAccessViewUint(UnorderedAccessView->View, Values);
	
	GPUProfilingData.RegisterGPUWork(1);
}
