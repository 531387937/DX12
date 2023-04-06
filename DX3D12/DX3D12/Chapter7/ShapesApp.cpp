#include "ShapesApp.h"

ShapesApp::ShapesApp(HINSTANCE hInstance):D3DApp(hInstance)
{
}
ShapesApp::~ShapesApp()
{
}

bool ShapesApp::Initialize()
{
    if(!D3DApp::Initialize())
    {
        return false;
    }
    BuildConstantsBuffers();
    BuildRootSignature();
    BuildDescriptorHeaps();
}

void ShapesApp::OnResize()
{
    D3DApp::OnResize();
}

void ShapesApp::Update(const GameTimer& gt)
{
    mCurrFrameResourceIndex = (mCurrFrameResourceIndex+1)%gNumFrameResources;
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

    if(mCurrFrameResource->Fence!=0&&mFence->GetCompletedValue()<mCurrFrameResource->Fence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr,false,false,EVENT_ALL_ACCESS);
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence,eventHandle));
        WaitForSingleObject(eventHandle,INFINITE);
        CloseHandle(eventHandle);
    }
    
}



void ShapesApp::BuildFrameResources()
{
    for(int i = 0;i<gNumFrameResources;i++)
    {
        mFrameResources.push_back(std::make_unique<FrameResource>(md3dDevice.Get(),1,(UINT)mAllRitems.size()));
    }
}

void ShapesApp::UpdateObjectCBs(const GameTimer& gt)
{
    auto currObjectCB = mCurrFrameResource->ObjectCB.get();
    for(auto& e:mAllRitems)
    {
        if(e->NumFramesDirty>0)
        {
            XMMATRIX world = XMLoadFloat4x4(&e->World);

            ObjectConstants objConstants;
            XMStoreFloat4x4(&objConstants.Word,XMMatrixTranspose(world));

            currObjectCB->CopyData(e->ObjCBIndex,objConstants);
            e->NumFramesDirty--;
        }
    }
}

void ShapesApp::UpdateMainPassCB(const GameTimer& gt)
{
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);

    XMMATRIX viewProj = XMMatrixMultiply(view,proj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view),view);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj),proj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj),viewProj);

    XMStoreFloat4x4(&mMainPassCB.View,XMMatrixTranspose(view));
    XMStoreFloat4x4(&mMainPassCB.Proj,XMMatrixTranspose(proj));
    XMStoreFloat4x4(&mMainPassCB.InvView,XMMatrixTranspose(invView));
    XMStoreFloat4x4(&mMainPassCB.InvProj,XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&mMainPassCB.InvViewProj,XMMatrixTranspose(invViewProj));
    mMainPassCB.EyePosW = mEyePos;
}


