#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl/client.h>

class GBuffer
{
public:
    GBuffer(ID3D12Device* device,UINT width,UINT height);

    GBuffer(const GBuffer& rhs) = delete;
    GBuffer& operator=(const GBuffer& rhs) = delete;
    ~GBuffer() = default;

    UINT Width();
    UINT Height();
    ID3D12Resource** Resource();

    CD3DX12_GPU_DESCRIPTOR_HANDLE Srv() const;
    CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv() const;

    D3D12_VIEWPORT Viewport() const;
    D3D12_RECT ScissorRect() const;

    void BuildDescriptors(
        CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
        CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
        CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv
    );

    void OnResize(UINT newWidth,UINT newHeight);

private:
    void BuildDescriptors();
    void BuildResources();

private:

    ID3D12Device* md3dDevice = nullptr;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT mScissorRect;

    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

    UINT mRtvDescriptorSize = 0;
    UINT mCbvSrvUavDescriptorSize = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv;

    Microsoft::WRL::ComPtr<ID3D12Resource> mPositionSpeMap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mNormalMap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mAlbedoMap = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mOtherMap = nullptr;
};
