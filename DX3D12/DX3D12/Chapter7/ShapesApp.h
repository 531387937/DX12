// #pragma once
// #include "d3dApp.h"
// #include "d3dUtil.h"
// #include "UploadBuffer.h"
// #include "../Camera.h"
// using namespace DirectX;
// using namespace Microsoft::WRL;
//
// struct ObjectConstants
// {
//     XMFLOAT4X4 Word = MathHelper::Identity4x4();
// };
// struct Vertex
// {
//     XMFLOAT3 Pos;
//     XMFLOAT3 Normal;
// };
// struct PassConstants
// {
//     XMFLOAT4X4 View;
//     XMFLOAT4X4 InvView;
//     XMFLOAT4X4 Proj;
//     XMFLOAT4X4 InvProj;
//     XMFLOAT4X4 ViewProj;
//     XMFLOAT4X4 InvViewProj;
//     XMFLOAT3 EyePosW;
//     float cbPerObjectPad1 = 0.0f;
//     XMFLOAT2 RenderTargetSize;
//     XMFLOAT2 InvRenderTargetSize;
//     float NearZ;
//     float FarZ;
//     float TotalTime;
//     float DeltaTime;
//
//     XMFLOAT4 AmbientLight = {0.0f, 0.0f, 0.0f, 1.0f};
//     Light Lights[MaxLights];
// };
//
// struct RenderItem
// {
//     RenderItem() = default;
//
//     XMFLOAT4X4 World = MathHelper::Identity4x4();
//     //dirty flag
//     int NumFramesDirty = gNumFrameResources;
//     //the Index in ConstantBuffer
//     UINT ObjCBIndex = -1;
//
//     Material* Mat = nullptr;
//     //MeshGeoMetry to use
//     MeshGeometry* Geo = nullptr;
//
//     D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//     UINT IndexCount = 0;
//     UINT StartIndexLocation = 0;
//     int BaseVertexLocation = 0;
// };
//
// struct FrameResource
// {
// public:
//     FrameResource() = delete;
//     ~FrameResource();
//     FrameResource(ID3D12Device* device,UINT passCount,UINT materialCount, UINT objCount);
//     FrameResource(const FrameResource& rhs) = delete;
//     FrameResource& operator=(const FrameResource& rhs) = delete;
//     ComPtr<ID3D12CommandAllocator> CmdListAlloc;
//     std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
//     std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
//     std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
//
//     UINT64 Fence = 0;
// };
//
// inline FrameResource::FrameResource(ID3D12Device* device, UINT passCount,UINT materialCount, UINT objectCount)
// {
//     ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
//     PassCB = std::make_unique<UploadBuffer<PassConstants>>(device,passCount,true);
//     ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device,objectCount,true);
//     MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device,materialCount,true);
//     
// }
//
// inline FrameResource::~FrameResource()
// {
// }
//
// class ShapesApp:public D3DApp
// {
// public:
//     ShapesApp(HINSTANCE hInstance);
//     ShapesApp(const ShapesApp& rhs) = delete;
//     ShapesApp& operator=(const ShapesApp& rhs) = delete;
//     ~ShapesApp();
//     
//     bool Initialize() override;
//     
// private:
//     void OnMouseDown(WPARAM btnState, int x, int y) override;
//     void OnMouseMove(WPARAM btnState, int x, int y) override;
//     void OnMouseUp(WPARAM btnState, int x, int y) override;
//     void Update(const GameTimer& gt) override;
//     void Draw(const GameTimer& gt) override;
//     void OnResize() override;
//
//     void OnKeyboardInput(const GameTimer& gt);
//     void UpdateCamera(const GameTimer& gt);
//     void UpdateObjectCBs(const GameTimer& gt);
//     void UpdateMaterialCBs(const GameTimer& gt);
//     void UpdateMainPassCB(const GameTimer& gt);
//
//     void BuildFrameResources();
//     void BuildRootSignature();
//     void BuildConstantsBufferViews();
//     void BuildDescriptorHeaps();
//     void BuildShadersAndInputLayout();
//     void BuildShapeGeometry();
//     void BuildMaterials();
//     void BuildRenderItems();
//     void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*> ritems);
//     void BuildPSO();
//
// private:
//     Camera mCamera;
//     std::vector<std::unique_ptr<FrameResource>> mFrameResources;
//     FrameResource* mCurrFrameResource = nullptr;
//     int mCurrFrameResourceIndex = 0;
//     
//     ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
//     ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;
//
//     ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
//     std::unordered_map<std::string,std::unique_ptr<MeshGeometry>> mGeometries;
//     std::unordered_map<std::string,std::unique_ptr<Material>> mMaterials;
//     std::unordered_map<std::string,ComPtr<ID3DBlob>> mShaders;
//     std::unordered_map<std::string,ComPtr<ID3D12PipelineState>> mPSOs;
//
//     std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
//     
//     ComPtr<ID3DBlob> mvsByteCode = nullptr;
//     ComPtr<ID3DBlob> mpsByteCode = nullptr;
//     ComPtr<ID3D12PipelineState> mPSO = nullptr;
//
//     std::vector<std::unique_ptr<RenderItem>> mAllRitems;
//
//     std::vector<RenderItem*> mOpaqueRitems;
//
//     float mTheta = 1.5f*XM_PI;
//     float mPhi = 0.2f*XM_PI;
//     float mRadius = 15.0f;
//
//     XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
//     XMFLOAT4X4 mView = MathHelper::Identity4x4();
//     XMFLOAT4X4 mProj = MathHelper::Identity4x4();
//
//     PassConstants mMainPassCB;
//
//     UINT mPassCbvOffset = 0;
//
//     bool mIsWireframe = false;
//
//     POINT mLastMousePos;
// };
