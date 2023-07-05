#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl/client.h>
#include <vector>
#include <MathHelper.h>
using namespace DirectX;
class SSAO
{
public:
	SSAO(ID3D12Device* device,ID3D12GraphicsCommandList* cmdList, UINT width, UINT height);

	SSAO(const SSAO& rhs) = delete;
	SSAO& operator = (const SSAO& rhs) = delete;
	~SSAO() = default;

	UINT Width();
	UINT Height();
	ID3D12Resource* Resource();
	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Rtv() const;

	D3D12_VIEWPORT Viewport() const;
	D3D12_RECT ScissorRect() const;

	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv
	);

	void OnResize(UINT newWidth, UINT newHeight);

private:
	void BuildDescriptors();
	void BuildResource();
	void BuildOffsetVectors();
private:

	ID3D12Device* md3dDevice = nullptr;
	ID3D12GraphicsCommandList* mCmdList = nullptr;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R16_UNORM;

	static const int MaxBlurRadius = 5;

	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv;

	Microsoft::WRL::ComPtr<ID3D12Resource> mSSAOMap = nullptr;

	std::vector<XMFLOAT4> mOffsets;
};

