// #include "RendererApp.h"
//
// RendererApp::RendererApp(HINSTANCE hInstance):D3DApp(hInstance)
// {
// }
// RendererApp::~RendererApp()
// {
//     if(md3dDevice!=nullptr)
//         FlushCommandQueue();
// }
//
// bool RendererApp::Initialize()
// {
//     if(!D3DApp::Initialize())
//         return false;
//
//     ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(),nullptr));
//
//     LoadTextures();
//     BuildRootSignature();
//     BuildDescriptorHeaps();
//     BuildShadersAndInputLayout();
//     BuildRoomGeometry();
//     BuildSkullGeometry();
//     BuildMaterials();
//     BuildRenderItems();
//     BuildFrameResources();
//     BuildPSOs();
//
//     // Execute the initialization commands.
//     ThrowIfFailed(mCommandList->Close());
//     ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
//     mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
//
//     // Wait until initialization is complete.
//     FlushCommandQueue();
//
//     return true;
// }
//
// void RendererApp::OnResize()
// {
//     D3DApp::OnResize();
//
//     // The window resized, so update the aspect ratio and recompute the projection matrix.
//     XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
//     XMStoreFloat4x4(&mProj, P);
// }
//
// void RendererApp::Update(const GameTimer& gt)
// {
//     OnKeyboardInput(gt);
//     UpdateCamera(gt);
//
//     // Cycle through the circular frame resource array.
//     mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
//     mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
//
//     // Has the GPU finished processing the commands of the current frame resource?
//     // If not, wait until the GPU has completed commands up to this fence point.
//     if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
//     {
//         HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
//         ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
//         WaitForSingleObject(eventHandle, INFINITE);
//         CloseHandle(eventHandle);
//     }
//
//     AnimateMaterials(gt);
//     UpdateObjectCBs(gt);
//     UpdateMaterialCBs(gt);
//     UpdateMainPassCB(gt);
//     UpdateReflectedPassCB(gt);
// }
//
// void RendererApp::Draw(const GameTimer& gt)
// {
// }
//
// void RendererApp::OnMouseDown(WPARAM btnState, int x, int y)
// {
//     mLastMousePos.x = x;
//     mLastMousePos.y = y;
//
//     SetCapture(mhMainWnd);
// }
//
// void RendererApp::OnMouseUp(WPARAM btnState, int x, int y)
// {
//     ReleaseCapture();
// }
//
// void RendererApp::OnMouseMove(WPARAM btnState, int x, int y)
// {
//     if ((btnState & MK_LBUTTON) != 0)
//     {
//         // Make each pixel correspond to a quarter of a degree.
//         float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
//         float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));
//
//         // Update angles based on input to orbit camera around box.
//         mTheta += dx;
//         mPhi += dy;
//
//         // Restrict the angle mPhi.
//         mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
//     }
//     else if ((btnState & MK_RBUTTON) != 0)
//     {
//         // Make each pixel correspond to 0.2 unit in the scene.
//         float dx = 0.2f * static_cast<float>(x - mLastMousePos.x);
//         float dy = 0.2f * static_cast<float>(y - mLastMousePos.y);
//
//         // Update the camera radius based on input.
//         mRadius += dx - dy;
//
//         // Restrict the radius.
//         mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
//     }
//
//     mLastMousePos.x = x;
//     mLastMousePos.y = y;
// }
//
// void RendererApp::OnKeyboardInput(const GameTimer& gt)
// {
//     const float dt = gt.DeltaTime();
//
//     if (GetAsyncKeyState('A') & 0x8000)
//         mSkullTranslation.x -= 1.0f * dt;
//
//     if (GetAsyncKeyState('D') & 0x8000)
//         mSkullTranslation.x += 1.0f * dt;
//
//     if (GetAsyncKeyState('W') & 0x8000)
//         mSkullTranslation.y += 1.0f * dt;
//
//     if (GetAsyncKeyState('S') & 0x8000)
//         mSkullTranslation.y -= 1.0f * dt;
//
//    
// }
//
// void RendererApp::UpdateCamera(const GameTimer& gt)
// {
//     mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
//     mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
//     mEyePos.y = mRadius * cosf(mPhi);
//
//     // Build the view matrix.
//     XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
//     XMVECTOR target = XMVectorZero();
//     XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//
//     XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
//     XMStoreFloat4x4(&mView, view);
// }
//
// void RendererApp::AnimateMaterials(const GameTimer& gt)
// {
//     auto currObjectCB = mCurrFrameResource->ObjectCB.get();
//     for (auto& e : mAllRitems)
//     {
//         // Only update the cbuffer data if the constants have changed.  
//         // This needs to be tracked per frame resource.
//         if (e->NumFramesDirty > 0)
//         {
//             XMMATRIX world = XMLoadFloat4x4(&e->World);
//             XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);
//
//             ObjectConstants objConstants;
//             XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));
//             XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
//
//             currObjectCB->CopyData(e->ObjCBIndex, objConstants);
//
//             // Next FrameResource need to be updated too.
//             e->NumFramesDirty--;
//         }
//     }
// }
//
// void RendererApp::UpdateObjectCBs(const GameTimer& gt)
// {
// }
//
// void RendererApp::UpdateMaterialCBs(const GameTimer& gt)
// {
// }
//
// void RendererApp::UpdateMainPassCB(const GameTimer& gt)
// {
// }
//
// void RendererApp::UpdateReflectedPassCB(const GameTimer& gt)
// {
// }
//
// void RendererApp::LoadTextures()
// {
// }
//
// void RendererApp::BuildRootSignature()
// {
// }
//
// void RendererApp::BuildDescriptorHeaps()
// {
// }
//
// void RendererApp::BuildShadersAndInputLayout()
// {
// }
//
// void RendererApp::BuildRoomGeometry()
// {
// }
//
// void RendererApp::BuildSkullGeometry()
// {
// }
//
// void RendererApp::BuildPSOs()
// {
// }
//
// void RendererApp::BuildFrameResources()
// {
// }
//
// void RendererApp::BuildMaterials()
// {
// }
//
// void RendererApp::BuildRenderItems()
// {
// }
//
// void RendererApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
// {
// }
//
// std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> RendererApp::GetStaticSamplers()
// {
// }
