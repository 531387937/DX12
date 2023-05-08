// #pragma once
// #include "d3dApp.h"
// #include "d3dUtil.h"
// #include "UploadBuffer.h"
// #include "Waves.h"
// using namespace DirectX;
// using namespace Microsoft::WRL;
// using namespace DirectX::PackedVector;
//
// struct ObjectConstants
// {
//     XMFLOAT4X4 World = MathHelper::Identity4x4();
// };
//
// struct Vertex
// {
//     XMFLOAT3 Pos;
//     XMFLOAT3 Normal;
// };
//
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
//
//     Light Lights[MaxLights];
// };
//
// struct RenderItem
// {
//     RenderItem() = default;
//
//     // World matrix of the shape that describes the object's local space
//     // relative to the world space, which defines the position, orientation,
//     // and scale of the object in the world.
//     XMFLOAT4X4 World = MathHelper::Identity4x4();
//
//     XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
//
//     // Dirty flag indicating the object data has changed and we need to update the constant buffer.
//     // Because we have an object cbuffer for each FrameResource, we have to apply the
//     // update to each FrameResource.  Thus, when we modify obect data we should set 
//     // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
//     int NumFramesDirty = gNumFrameResources;
//
//     // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
//     UINT ObjCBIndex = -1;
//
//     Material* Mat = nullptr;
//     MeshGeometry* Geo = nullptr;
//
//     // Primitive topology.
//     D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//     // DrawIndexedInstanced parameters.
//     UINT IndexCount = 0;
//     UINT StartIndexLocation = 0;
//     int BaseVertexLocation = 0;
// };
//
// enum class RenderLayer:int
// {
//     Opaque = 0,
//     Count
// };
//
// struct FrameResource
// {
// public:
//     FrameResource() = delete;
//     ~FrameResource();
//     FrameResource(ID3D12Device* device, UINT passCount, UINT objCount, UINT materialCount, UINT waveVertCount);
//     FrameResource(const FrameResource& rhs) = delete;
//     FrameResource& operator=(const FrameResource& rhs) = delete;
//     ComPtr<ID3D12CommandAllocator> CmdListAlloc;
//     std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
//     std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
//     std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
//
//     std::unique_ptr<UploadBuffer<Vertex>> WavesVB = nullptr;
//
//     UINT64 Fence = 0;
// };
//
// inline FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount,
//                                     UINT waveVertCount)
// {
//     ThrowIfFailed(
//         device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
//     PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
//     MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
//     ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
//
//     WavesVB = std::make_unique<UploadBuffer<Vertex>>(device, waveVertCount, false);
// }
//
// inline FrameResource::~FrameResource()
// {
// }
//
// class LitWavesApp : public D3DApp
// {
// public:
//     LitWavesApp(HINSTANCE hInstance);
//     LitWavesApp(const LitWavesApp& rhs) = delete;
//     LitWavesApp& operator=(const LitWavesApp& rhs) = delete;
//     ~LitWavesApp();
//
//     bool Initialize() override;
//
// protected:
//     void Update(const GameTimer& gt) override;
//     void Draw(const GameTimer& gt) override;
// public:
// protected:
//     void OnResize() override;
//     void OnMouseDown(WPARAM btnState, int x, int y) override;
//     void OnMouseUp(WPARAM btnState, int x, int y) override;
//     void OnMouseMove(WPARAM btnState, int x, int y) override;
//
//     void OnKeyboardInput(const GameTimer& gt);
//     void UpdateCamera(const GameTimer& gt);
//     void UpdateObjectCBs(const GameTimer& gt);
//     void UpdateMaterialCBs(const GameTimer& gt);
//     void UpdateMainPassCB(const GameTimer& gt);
//     void UpdateWaves(const GameTimer& gt);
//
//     void BuildRootSignature();
//     void BuildShadersAndInputLayout();
//     void BuildLandGeometry();
//     void BuildWavesGeometryBuffers();
//     void BuildPSOs();
//     void BuildFrameResources();
//     void BuildMaterials();
//     void BuildRenderItems();
//     void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
//
//     float GetHillsHeight(float x, float z) const;
//     XMFLOAT3 GetHillsNormal(float x, float z) const;
//
// private:
//     std::vector<std::unique_ptr<FrameResource>> mFrameResources;
//     FrameResource* mCurrFrameResource = nullptr;
//     int mCurrFrameResourceIndex = 0;
//
//     UINT mCbvSrvDescriptorSize = 0;
//
//     ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
//
//     std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
//     std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
//     std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
//     std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
//     std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
//
//     std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
//
//     RenderItem* mWavesRitem = nullptr;
//
//     std::vector<std::unique_ptr<RenderItem>> mAllRitems;
//
//     std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];
//
//     std::unique_ptr<Waves> mWaves;
//
//     PassConstants mMainPassCB;
//
//     XMFLOAT3 mEyePos = {0.0f, 0.0f, 0.0f};
//     XMFLOAT4X4 mView = MathHelper::Identity4x4();
//     XMFLOAT4X4 mProj = MathHelper::Identity4x4();
//
//     float mTheta = 1.5f * XM_PI;
//     float mPhi = XM_PIDIV2 - 0.1f;
//     float mRadius = 50.0f;
//
//     float mSunTheta = 1.25f * XM_PI;
//     float mSunPhi = XM_PIDIV4;
//
//     POINT mLastMousePos;
// };
