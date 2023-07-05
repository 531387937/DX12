#include "SSAO.h"

#include "d3dUtil.h"

SSAO::SSAO(ID3D12Device* device,ID3D12GraphicsCommandList* cmdList, UINT width, UINT height):md3dDevice(device),mCmdList(cmdList),
                                                                                             mWidth(width),mHeight(height)
{
    mViewport = {0.0f,0.0f,(float)mWidth,(float)mHeight,0.0f,1.0f};
    mScissorRect = {0,0,(int)width,(int)height};

    BuildResource();
}

UINT SSAO::Width()
{
    return mWidth;
}

UINT SSAO::Height()
{
    return mHeight;
}

ID3D12Resource* SSAO::Resource()
{
    return mSSAOMap.Get();
}

CD3DX12_GPU_DESCRIPTOR_HANDLE SSAO::Srv() const
{
    return mhGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE SSAO::Rtv() const
{
    return mhCpuRtv;
}

D3D12_VIEWPORT SSAO::Viewport() const
{
    return mViewport;
}

D3D12_RECT SSAO::ScissorRect() const
{
    return mScissorRect;
}

void SSAO::BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
    CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv)
{
    mhCpuRtv = hCpuRtv;
    mhCpuSrv = hCpuSrv;
    mhGpuSrv = hGpuSrv;
}

void SSAO::OnResize(UINT newWidth, UINT newHeight)
{
    mWidth = newWidth;
    mHeight = newHeight;

    BuildResource();
    BuildDescriptors();
}

void SSAO::BuildDescriptors()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format=  mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    srvDesc.Texture2D.PlaneSlice = 0;
    md3dDevice->CreateShaderResourceView(mSSAOMap.Get(),&srvDesc,mhCpuSrv);

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Format = mFormat;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    md3dDevice->CreateDepthStencilView(mSSAOMap.Get(),&dsvDesc,mhCpuRtv);
}

void SSAO::BuildResource()
{
    D3D12_RESOURCE_DESC texDesc;
    ZeroMemory(&texDesc,sizeof(D3D12_RESOURCE_DESC));
    texDesc.Alignment = 0;
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = mWidth;
    texDesc.Height = mHeight;
    texDesc.DepthOrArraySize = 1;
    texDesc.Format = mFormat;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = mFormat;
    optClear.Color[0] = 0.0f;
    
    ThrowIfFailed(md3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),D3D12_HEAP_FLAG_NONE,&texDesc,D3D12_RESOURCE_STATE_GENERIC_READ,&optClear,IID_PPV_ARGS(mSSAOMap.GetAddressOf())));
}

void SSAO::BuildOffsetVectors()
{
    mOffsets.resize(14);
    mOffsets[0] = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
    mOffsets[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);
    mOffsets[2] = XMFLOAT4(-1.0f, 1.0f, 1.0f, 0.0f);
    mOffsets[3] = XMFLOAT4(1.0f, -1.0f, -1.0f, 0.0f);
    mOffsets[4] = XMFLOAT4(1.0f, 1.0f, -1.0f, 0.0f);
    mOffsets[5] = XMFLOAT4(-1.0f, -1.0f, 1.0f, 0.0f);
    mOffsets[6] = XMFLOAT4(-1.0f, 1.0f, -1.0f, 0.0f);
    mOffsets[7] = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
    mOffsets[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
    mOffsets[9] = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
    mOffsets[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
    mOffsets[11] = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
    mOffsets[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
    mOffsets[13] = XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f);

    for (int i = 0; i < 14; i++)
    {
        float s = MathHelper::RandF(0.25f, 1.0f);
        XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&mOffsets[i]));

        XMStoreFloat4(&mOffsets[i], v);
    }
}
