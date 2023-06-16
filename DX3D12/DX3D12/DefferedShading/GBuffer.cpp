#include "GBuffer.h"
#include "d3dUtil.h"

GBuffer::GBuffer(ID3D12Device* device, UINT width, UINT height)
{
    mViewport = {0.0f,0.0f,(float)width,(float)height,0.0f,1.0f};
    mScissorRect = {0,0,(int)width,(int)height};
    mHeight = height;
    mWidth = width;
    md3dDevice = device;
    mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    BuildResources();
}

UINT GBuffer::Width()
{
    return mWidth;
}

UINT GBuffer::Height()
{
    return mHeight;
}

ID3D12Resource** GBuffer::Resource()
{
    ID3D12Resource* resources[] = {mPositionSpeMap.Get(),mNormalMap.Get(),mAlbedoMap.Get()};
    return resources;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::Srv() const
{
    return mhGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::Rtv() const
{
    return mhCpuRtv;
}

D3D12_VIEWPORT GBuffer::Viewport() const
{
    return mViewport;
}

D3D12_RECT GBuffer::ScissorRect() const
{
    return mScissorRect;
}

void GBuffer::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
    CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv)
{
    mhCpuSrv = hCpuSrv;
    mhGpuSrv = hGpuSrv;
    mhCpuRtv = hCpuRtv;

    BuildDescriptors();
}

void GBuffer::OnResize(UINT newWidth, UINT newHeight)
{
    mHeight = newHeight;
    mWidth = newWidth;
    mViewport = { 0.0f,0.0f,(float)mWidth,(float)mHeight,0.0f,1.0f };
    mScissorRect = { 0,0,(int)mWidth,(int)mHeight };
    BuildResources();
    BuildDescriptors();
}

void GBuffer::BuildDescriptors()
{
    auto srvheap = mhCpuSrv;
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format=  mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    srvDesc.Texture2D.PlaneSlice = 0;
    md3dDevice->CreateShaderResourceView(mPositionSpeMap.Get(),&srvDesc,srvheap);

    srvheap.Offset(1,mCbvSrvUavDescriptorSize);

    md3dDevice->CreateShaderResourceView(mNormalMap.Get(),&srvDesc,srvheap);

    srvheap.Offset(1,mCbvSrvUavDescriptorSize);
    md3dDevice->CreateShaderResourceView(mAlbedoMap.Get(),&srvDesc,srvheap);

    auto rvtheap = mhCpuRtv;
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = mFormat;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;
    md3dDevice->CreateRenderTargetView(mPositionSpeMap.Get(),&rtvDesc,rvtheap);

    rvtheap.Offset(1,mRtvDescriptorSize);

    md3dDevice->CreateRenderTargetView(mNormalMap.Get(),&rtvDesc,rvtheap);
    
    rvtheap.Offset(1,mRtvDescriptorSize);
    
    md3dDevice->CreateRenderTargetView(mAlbedoMap.Get(),&rtvDesc,rvtheap);
    
}

void GBuffer::BuildResources()
{
    D3D12_RESOURCE_DESC texDesc;
    ZeroMemory(&texDesc,sizeof(D3D12_RESOURCE_DESC));
    texDesc.Format = mFormat;
    texDesc.Width = mWidth;
    texDesc.Height = mHeight;
    texDesc.Alignment = 0;
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = mFormat;
    optClear.Color[0] = 0.0f;
    optClear.Color[1] = 0.0f;
    optClear.Color[2] = 0.0f;
    optClear.Color[3] = 1.0f;
    ThrowIfFailed(md3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),D3D12_HEAP_FLAG_NONE,&texDesc,D3D12_RESOURCE_STATE_GENERIC_READ,&optClear,IID_PPV_ARGS(mPositionSpeMap.GetAddressOf())));

    ThrowIfFailed(md3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),D3D12_HEAP_FLAG_NONE,&texDesc,D3D12_RESOURCE_STATE_GENERIC_READ,&optClear,IID_PPV_ARGS(mNormalMap.GetAddressOf())));

    ThrowIfFailed(md3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),D3D12_HEAP_FLAG_NONE,&texDesc,D3D12_RESOURCE_STATE_GENERIC_READ,&optClear,IID_PPV_ARGS(mAlbedoMap.GetAddressOf())));
}

