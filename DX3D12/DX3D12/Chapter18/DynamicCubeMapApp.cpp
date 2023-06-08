#include "DynamicCubeMapApp.h"
const UINT CubeMapSize = 512;
DynamicCubeMapApp::DynamicCubeMapApp(HINSTANCE hInstance):D3DApp(hInstance)
{
}

DynamicCubeMapApp::~DynamicCubeMapApp()
{
    if(md3dDevice!=nullptr)
        FlushCommandQueue();
}

bool DynamicCubeMapApp::Initialize()
{
    if(!Initialize())
        return false;
}

void DynamicCubeMapApp::CreateRtvAndDsvDescriptorHeaps()
{
    //为立方体渲染目标添加6个RTV
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount+6;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&rtvHeapDesc,IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

    //为立方体渲染目标新增1个DSV
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 2;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&dsvHeapDesc,IID_PPV_ARGS(mDsvHeap.GetAddressOf())));

    mCubeDSV = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDsvHeap->GetCPUDescriptorHandleForHeapStart(),1,mDsvDescriptorSize);

    auto srvCpuStart = mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    auto srvGpuStart = mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    auto rtvCpuStart = mRtvHeap->GetCPUDescriptorHandleForHeapStart();

    int rtvOffset = SwapChainBufferCount;

    CD3DX12_CPU_DESCRIPTOR_HANDLE cubeRtvHandles[6];
    for(int i = 0;i<6;i++)
    {
        cubeRtvHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCpuStart,rtvOffset+i,mRtvDescriptorSize);
    }

    mDynamicCubeMap->BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart,mDynamicTexHeapIndex,mCbvSrvUavDescriptorSize),
        CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart,mDynamicTexHeapIndex,mCbvSrvUavDescriptorSize),cubeRtvHandles);
}

void DynamicCubeMapApp::OnResize()
{
    D3DApp::OnResize();
}

void DynamicCubeMapApp::Update(const GameTimer& gt)
{
}

void DynamicCubeMapApp::Draw(const GameTimer& gt)
{
}

void DynamicCubeMapApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    D3DApp::OnMouseDown(btnState, x, y);
}

void DynamicCubeMapApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    D3DApp::OnMouseUp(btnState, x, y);
}

void DynamicCubeMapApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    D3DApp::OnMouseMove(btnState, x, y);
}

void DynamicCubeMapApp::OnKeyboardInput(const GameTimer& gt)
{
}

void DynamicCubeMapApp::AnimateMaterials(const GameTimer& gt)
{
}

void DynamicCubeMapApp::UpdateObjectCBs(const GameTimer& gt)
{
}

void DynamicCubeMapApp::UpdateMaterialBuffer(const GameTimer& gt)
{
}

void DynamicCubeMapApp::UpdateMainPassCB(const GameTimer& gt)
{
}

void DynamicCubeMapApp::UpdateCubeMapFacePassCBs()
{
    for(int i = 0; i < 6; ++i)
    {
        PassConstants cubeFacePassCB = mMainPassCB;

        XMMATRIX view = mCubeMapCamera[i].GetView();
        XMMATRIX proj = mCubeMapCamera[i].GetProj();

        XMMATRIX viewProj = XMMatrixMultiply(view, proj);
        XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
        XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
        XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

        XMStoreFloat4x4(&cubeFacePassCB.View, XMMatrixTranspose(view));
        XMStoreFloat4x4(&cubeFacePassCB.InvView, XMMatrixTranspose(invView));
        XMStoreFloat4x4(&cubeFacePassCB.Proj, XMMatrixTranspose(proj));
        XMStoreFloat4x4(&cubeFacePassCB.InvProj, XMMatrixTranspose(invProj));
        XMStoreFloat4x4(&cubeFacePassCB.ViewProj, XMMatrixTranspose(viewProj));
        XMStoreFloat4x4(&cubeFacePassCB.InvViewProj, XMMatrixTranspose(invViewProj));
        cubeFacePassCB.EyePosW = mCubeMapCamera[i].GetPosition3f();
        cubeFacePassCB.RenderTargetSize = XMFLOAT2((float)CubeMapSize, (float)CubeMapSize);
        cubeFacePassCB.InvRenderTargetSize = XMFLOAT2(1.0f / CubeMapSize, 1.0f / CubeMapSize);

        auto currPassCB = mCurrFrameResource->PassCB.get();

        // Cube map pass cbuffers are stored in elements 1-6.
        currPassCB->CopyData(1 + i, cubeFacePassCB);
    }
}

void DynamicCubeMapApp::LoadTextures()
{
}

void DynamicCubeMapApp::BuildRootSignature()
{
}

void DynamicCubeMapApp::BuildDescriptorHeaps()
{
}

void DynamicCubeMapApp::BuildCubeDepthStencil()
{
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = CubeMapSize;
    depthStencilDesc.Height = CubeMapSize;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = mDepthStencilFormat;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = mDepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    ThrowIfFailed(md3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,D3D12_RESOURCE_STATE_COMMON,&optClear,IID_PPV_ARGS(mCubeDepthStencilBuffer.GetAddressOf())));

    md3dDevice->CreateDepthStencilView(mCubeDepthStencilBuffer.Get(),nullptr,mCubeDSV);

    mCommandList->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(mCubeDepthStencilBuffer.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void DynamicCubeMapApp::BuildShadersAndInputLayout()
{
}

void DynamicCubeMapApp::BuildSkullGeometry()
{
}

void DynamicCubeMapApp::BuildShapeGeometry()
{
}

void DynamicCubeMapApp::BuildPSOs()
{
}

void DynamicCubeMapApp::BuildFrameResources()
{
    for(int i = 0;i<gNumFrameResources;i++)
    {
        mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),7,(UINT)mAllRitems.size(),(UINT)mMaterials.size()));
    }
}

void DynamicCubeMapApp::BuildMaterials()
{
}

void DynamicCubeMapApp::BuildRenderItems()
{
}

void DynamicCubeMapApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
}

void DynamicCubeMapApp::DrawSceneToCubeMap()
{
    mCommandList->RSSetViewports(1,&mDynamicCubeMap->Viewport());
    mCommandList->RSSetScissorRects(1,&mDynamicCubeMap->ScissorRect());

    //立方体资源转为渲染目标
    mCommandList->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(mDynamicCubeMap->Resource(),D3D12_RESOURCE_STATE_GENERIC_READ,
        D3D12_RESOURCE_STATE_RENDER_TARGET));

    UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

    for(int i = 0;i<6;++i)
    {
        mCommandList->ClearRenderTargetView(mDynamicCubeMap->Rtv(i),Colors::LightSteelBlue,0,nullptr);
        mCommandList->ClearDepthStencilView(mCubeDSV,D3D12_CLEAR_FLAG_DEPTH|D3D12_CLEAR_FLAG_STENCIL,1.0f,0,0,nullptr);
        
    }
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> DynamicCubeMapApp::GetStaticSamplers()
{
}

void DynamicCubeMapApp::BuildCubeFaceCamera(float x, float y, float z)
{
    XMFLOAT3 center(x,y,z);
    XMFLOAT3 worldUp(0.0f,1.0f,0.0f);

    XMFLOAT3 targets[6] = {
        XMFLOAT3(x+1.0f,y,z),
        XMFLOAT3(x-1.0f,y,z),
        XMFLOAT3(x,y+1.0f,z),
        XMFLOAT3(x,y-1.0f,z),
        XMFLOAT3(x,y,z+1.0f),
        XMFLOAT3(x,y,z-1.0f)
    };

    XMFLOAT3 ups[6] = {
        XMFLOAT3(0.0f,1.0f,0.0f),
        XMFLOAT3(0.0f,1.0f,0.0f),
        XMFLOAT3(0.0f,0.0f,-1.0f),
        XMFLOAT3(0.0f,0.0f,1.0f),
        XMFLOAT3(0.0f,1.0f,0.0f),
        XMFLOAT3(0.0f,1.0f,0.0f)
    };

    for(int i = 0;i<6;++i)
    {
        mCubeMapCamera[i].LookAt(center,targets[i],ups[i]);
        mCubeMapCamera[i].SetLens(0.5f*XM_PI,1.0f,0.1f,1000.0f);
        mCubeMapCamera[i].UpdateViewMatrix();
    }
}
