#pragma once
#include "d3dApp.h"
#include "d3dUtil.h"
#include "UploadBuffer.h"
using namespace DirectX;
using namespace Microsoft::WRL;

struct ObjectConstants
{
    XMFLOAT4X4 Word = MathHelper::Identity4x4();
};

struct PassConstants
{
    XMFLOAT4X4 View;
    XMFLOAT4X4 InvView;
    XMFLOAT4X4 Proj;
    XMFLOAT4X4 InvProj;
    XMFLOAT4X4 ViewProj;
    XMFLOAT4X4 InvViewProj;
    XMFLOAT3 EyePosW;
    XMFLOAT2 RenderTargetSize;
    XMFLOAT2 InvRenderTargetSize;
    float NearZ;
    float FarZ;
    float TotalTime;
    float DeltaTime;
};

struct RenderItem
{
    RenderItem() = default;

    XMFLOAT4X4 World = MathHelper::Identity4x4();
    //dirty flag
    int NumFramesDirty = gNumFrameResources;
    //the Index in ConstantBuffer
    UINT ObjCBIndex = -1;
    //MeshGeoMetry to use
    MeshGeometry* Geo = nullptr;

    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;
};

struct FrameResource
{
public:
    FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
    FrameResource(const FrameResource& rhs) = delete;
    FrameResource& operator=(const FrameResource& rhs) = delete;
    ~FrameResource();

    ComPtr<ID3D12CommandAllocator> CmdListAlloc;

    std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

    UINT64 Fence = 0;
};

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount)
{
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&CmdListAlloc)));
    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device,passCount,true);
    ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device,objectCount,true);
}

inline FrameResource::~FrameResource()
{
}

const int gNumFrameResources = 3;
std::vector<std::unique_ptr<FrameResource>> mFrameResources;
FrameResource* mCurrFrameResource = nullptr;
int mCurrFrameResourceIndex = 0;

class ShapesApp:public D3DApp
{
public:
    ShapesApp(HINSTANCE hInstance);
    ShapesApp(const ShapesApp& rhs) = delete;
    ShapesApp& operator=(const ShapesApp& rhs) = delete;
    ~ShapesApp();
    
    bool Initialize() override;
    void OnResize() override;
    
private:
    void OnMouseDown(WPARAM btnState, int x, int y) override;
    void OnMouseMove(WPARAM btnState, int x, int y) override;
    void OnMouseUp(WPARAM btnState, int x, int y) override;
    void Update(const GameTimer& gt) override;
    void Draw(const GameTimer& gt) override;

    void BuildFrameResources();
    void BuildRootSignature();
    void BuildConstantsBuffers();
    void BuildDescriptorHeaps();
    void BuildShaderAndInputLayout();
    void BuildPSO();
    void UpdateObjectCBs(const GameTimer& gt);
    void UpdateMainPassCB(const GameTimer& gt);

private:
    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
    ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;
    ComPtr<ID3D12PipelineState> mPSO = nullptr;

    std::vector<std::unique_ptr<RenderItem>> mAllRitems;

    std::vector<RenderItem*> mOpaqueRitems;
    std::vector<RenderItem*> mTransparentRitems;

    float mTheta = 1.5f*XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;

    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    PassConstants mMainPassCB;

    POINT mLastMousePos;
};
