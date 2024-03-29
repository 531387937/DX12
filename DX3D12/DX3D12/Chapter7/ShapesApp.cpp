﻿// #include "ShapesApp.h"
//
// #include <complex.h>
//
// #include "GeometryGenerator.h"
//
// const int gNumFrameResources = 3;
//
// ShapesApp::ShapesApp(HINSTANCE hInstance): D3DApp(hInstance)
// {
// }
//
// ShapesApp::~ShapesApp()
// {
//     if (md3dDevice != nullptr)
//         FlushCommandQueue();
// }
//
// bool ShapesApp::Initialize()
// {
//     if (!D3DApp::Initialize())
//     {
//         return false;
//     }
//
//     ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(),nullptr));
//
//     BuildRootSignature();
//     BuildShadersAndInputLayout();
//     BuildShapeGeometry();
//     BuildMaterials();
//     BuildRenderItems();
//     BuildFrameResources();
//     BuildDescriptorHeaps();
//     BuildConstantsBufferViews();
//     BuildPSO();
//
//     ThrowIfFailed(mCommandList->Close());
//     ID3D12CommandList* cmdsList[] = {mCommandList.Get()};
//     mCommandQueue->ExecuteCommandLists(_countof(cmdsList), cmdsList);
//
//     FlushCommandQueue();
//     return true;
// }
//
// void ShapesApp::OnResize()
// {
//     D3DApp::OnResize();
//
//     mCamera.SetLens(0.25f*MathHelper::Pi,AspectRatio(),1.0f,1000.0f);
//     XMStoreFloat4x4(&mProj, mCamera.GetProj());
// }
//
// void ShapesApp::Update(const GameTimer& gt)
// {
//     OnKeyboardInput(gt);
//     UpdateCamera(gt);
//
//     mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
//     mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
//
//     if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
//     {
//         HANDLE eventHandle = CreateEventEx(nullptr, false, false,EVENT_ALL_ACCESS);
//         ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence,eventHandle));
//         WaitForSingleObject(eventHandle,INFINITE);
//         CloseHandle(eventHandle);
//     }
//
//     UpdateObjectCBs(gt);
//     UpdateMaterialCBs(gt);
//     UpdateMainPassCB(gt);
// }
//
// void ShapesApp::Draw(const GameTimer& gt)
// {
//     auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;
//
//     ThrowIfFailed(cmdListAlloc->Reset());
//     if (mIsWireframe)
//     {
//         ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque_wireframe"].Get()));
//     }
//     else
//     {
//         ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
//     }
//
//     mCommandList->RSSetViewports(1, &mScreenViewport);
//     mCommandList->RSSetScissorRects(1, &mScissorRect);
//     mCommandList->ResourceBarrier(
//         1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT,
//                                                  D3D12_RESOURCE_STATE_RENDER_TARGET));
//     mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::SteelBlue, 0, nullptr);
//     mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0,
//                                         0, nullptr);
//
//     mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
//
//     ID3D12DescriptorHeap* descriptorHeaps[] = {mCbvHeap.Get()};
//     mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
//
//     mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
//
//     int passCbvIndex = mPassCbvOffset + mCurrFrameResourceIndex;
//     auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
//     passCbvHandle.Offset(passCbvIndex, mCbvSrvUavDescriptorSize);
//     mCommandList->SetGraphicsRootDescriptorTable(2, passCbvHandle);
//
//     DrawRenderItems(mCommandList.Get(), mOpaqueRitems);
//
//     mCommandList->ResourceBarrier(
//         1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET,
//                                                  D3D12_RESOURCE_STATE_PRESENT));
//
//     ThrowIfFailed(mCommandList->Close());
//     ID3D12CommandList* cmdsList[] = {mCommandList.Get()};
//
//     mCommandQueue->ExecuteCommandLists(_countof(cmdsList), cmdsList);
//
//     ThrowIfFailed(mSwapChain->Present(0,0));
//     mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
//
//     mCurrFrameResource->Fence = ++mCurrentFence;
//
//     mCommandQueue->Signal(mFence.Get(), mCurrentFence);
// }
//
// void ShapesApp::OnMouseDown(WPARAM btnState, int x, int y)
// {
//     mLastMousePos.x = x;
//     mLastMousePos.y = y;
//
//     SetCapture(mhMainWnd);
// }
//
// void ShapesApp::OnMouseUp(WPARAM btnState, int x, int y)
// {
//     ReleaseCapture();
// }
//
// void ShapesApp::OnMouseMove(WPARAM btnState, int x, int y)
// {
//     if ((btnState & MK_LBUTTON) != 0)
//     {
//         // Make each pixel correspond to a quarter of a degree.
//         float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
//         float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));
//
//         mCamera.Pitch(dy);
//         mCamera.RotateY(dx);
//     }
//     
//
//     mLastMousePos.x = x;
//     mLastMousePos.y = y;
// }
//
// void ShapesApp::OnKeyboardInput(const GameTimer& gt)
// {
//     if (GetAsyncKeyState('1') & 0x8000)
//         mIsWireframe = true;
//     else
//         mIsWireframe = false;
// }
//
// void ShapesApp::UpdateCamera(const GameTimer& gt)
// {
//     if(GetAsyncKeyState('W')&0x8000)
//         mCamera.Walk(10.0f*gt.DeltaTime());
//     if(GetAsyncKeyState('S')&0x8000)
//         mCamera.Walk(-10.0f*gt.DeltaTime());
//     if(GetAsyncKeyState('A')&0x8000)
//         mCamera.Strafe(-10.0f*gt.DeltaTime());
//     if(GetAsyncKeyState('D')&0x8000)
//         mCamera.Strafe(10.0f*gt.DeltaTime());
//     mCamera.UpdateViewMatrix();
//     XMStoreFloat4x4(&mView, mCamera.GetView());
// }
//
// void ShapesApp::BuildFrameResources()
// {
//     for (int i = 0; i < gNumFrameResources; i++)
//     {
//         mFrameResources.push_back(
//             std::make_unique<FrameResource>(md3dDevice.Get(), 1, (UINT)mMaterials.size(), (UINT)mAllRitems.size()));
//     }
// }
//
// void ShapesApp::UpdateObjectCBs(const GameTimer& gt)
// {
//     auto currObjectCB = mCurrFrameResource->ObjectCB.get();
//     for (auto& e : mAllRitems)
//     {
//         if (e->NumFramesDirty > 0)
//         {
//             XMMATRIX world = XMLoadFloat4x4(&e->World);
//
//             ObjectConstants objConstants;
//             XMStoreFloat4x4(&objConstants.Word, XMMatrixTranspose(world));
//
//             currObjectCB->CopyData(e->ObjCBIndex, objConstants);
//             e->NumFramesDirty--;
//         }
//     }
// }
//
// void ShapesApp::UpdateMaterialCBs(const GameTimer& gt)
// {
//     auto currMatCB = mCurrFrameResource->MaterialCB.get();
//     for (auto& mat : mMaterials)
//     {
//         if (mat.second->NumFramesDirty > 0)
//         {
//             MaterialConstants matCB;
//             matCB.Roughness = mat.second->Roughness;
//             matCB.DiffuseAlbedo = mat.second->DiffuseAlbedo;
//             matCB.FresnelR0 = mat.second->FresnelR0;
//
//             currMatCB->CopyData(mat.second->MatCBIndex, matCB);
//             mat.second->NumFramesDirty--;
//         }
//     }
// }
//
// void ShapesApp::UpdateMainPassCB(const GameTimer& gt)
// {
//     XMMATRIX view = XMLoadFloat4x4(&mView);
//     XMMATRIX proj = XMLoadFloat4x4(&mProj);
//
//     XMMATRIX viewProj = XMMatrixMultiply(view, proj);
//     XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
//     XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
//     XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
//
//     XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
//     XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
//     XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
//     XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
//     XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
//     XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
//     mMainPassCB.EyePosW = mEyePos;
//     mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
//     mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / (float)mClientWidth, 1.0f / (float)mClientHeight);
//     mMainPassCB.NearZ = 1.0f;
//     mMainPassCB.FarZ = 1000.0f;
//     mMainPassCB.TotalTime = gt.TotalTime();
//     mMainPassCB.DeltaTime = gt.DeltaTime();
//     mMainPassCB.Lights[0].Strength = XMFLOAT3(0.6f, 0.6f, 0.6f);
//     mMainPassCB.Lights[0].Direction = XMFLOAT3(0.0f, -1.0f, 0.8f);
//
//     mMainPassCB.Lights[1].Strength = XMFLOAT3(0.3f, 0.3f, 0.3f);
//     mMainPassCB.Lights[1].Direction = XMFLOAT3(-1.0f, 0.0f, -0.2f);
//
//     mMainPassCB.Lights[2].Strength = XMFLOAT3(0.8f, 0.2f, 0.5f);
//     mMainPassCB.Lights[2].Direction = XMFLOAT3(0.4f, -0.3f, -0.5f);
//
//     auto currPassCB = mCurrFrameResource->PassCB.get();
//     currPassCB->CopyData(0, mMainPassCB);
// }
//
// void ShapesApp::BuildDescriptorHeaps()
// {
//     UINT objCount = (UINT)mOpaqueRitems.size();
//     UINT matCount = (UINT)mMaterials.size();
//
//     UINT numDescriptors = (objCount + matCount + 1) * gNumFrameResources;
//
//     mPassCbvOffset = (objCount + matCount) * gNumFrameResources;
//
//     D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
//     heapDesc.NumDescriptors = numDescriptors;
//     heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//     heapDesc.NodeMask = 0;
//     heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//     ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&heapDesc,IID_PPV_ARGS(&mCbvHeap)));
// }
//
// void ShapesApp::BuildConstantsBufferViews()
// {
//     UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
//
//     UINT objCount = (UINT)mOpaqueRitems.size();
//
//     for (int frameIndex = 0; frameIndex < gNumFrameResources; frameIndex++)
//     {
//         auto objectCB = mFrameResources[frameIndex]->ObjectCB.get();
//         for (UINT i = 0; i < objCount; i++)
//         {
//             D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->Resource()->GetGPUVirtualAddress();
//
//             cbAddress += i * objCBByteSize;
//
//             D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//             cbvDesc.BufferLocation = cbAddress;
//             cbvDesc.SizeInBytes = objCBByteSize;
//
//             int heapIndex = frameIndex * objCount + i;
//             auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
//             handle.Offset(heapIndex, mCbvSrvUavDescriptorSize);
//             md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
//         }
//     }
//
//     UINT matByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
//
//     UINT matCount = (UINT)mMaterials.size();
//     for (int frameIndex = 0; frameIndex < gNumFrameResources; frameIndex++)
//     {
//         auto materialCB = mFrameResources[frameIndex]->MaterialCB.get();
//         for (UINT i = 0; i < matCount; i++)
//         {
//             D3D12_GPU_VIRTUAL_ADDRESS cbAddress = materialCB->Resource()->GetGPUVirtualAddress();
//
//             cbAddress += i * matByteSize;
//
//             D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//             cbvDesc.BufferLocation = cbAddress;
//             cbvDesc.SizeInBytes = matByteSize;
//
//             int heapIndex = frameIndex * matCount + gNumFrameResources * objCount + i;
//             auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
//             handle.Offset(heapIndex, mCbvSrvUavDescriptorSize);
//             md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
//         }
//     }
//
//
//     UINT passByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
//
//     for (int frameIndex = 0; frameIndex < gNumFrameResources; frameIndex++)
//     {
//         auto PassCB = mFrameResources[frameIndex]->PassCB.get();
//
//         D3D12_GPU_VIRTUAL_ADDRESS cbAddress = PassCB->Resource()->GetGPUVirtualAddress();
//
//         D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
//         cbvDesc.BufferLocation = cbAddress;
//         cbvDesc.SizeInBytes = passByteSize;
//
//         int headIndex = gNumFrameResources * (objCount + matCount) + frameIndex;
//         auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
//         handle.Offset(headIndex, mCbvSrvUavDescriptorSize);
//         md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
//     }
// }
//
// void ShapesApp::BuildRootSignature()
// {
//     CD3DX12_DESCRIPTOR_RANGE table0;
//     table0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
//     CD3DX12_DESCRIPTOR_RANGE table1;
//     table1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
//     CD3DX12_DESCRIPTOR_RANGE table2;
//     table2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
//
//     CD3DX12_ROOT_PARAMETER slotParameters[3];
//     //slotParameters[0].InitAsConstants(16,0);
//     slotParameters[0].InitAsDescriptorTable(1, &table0);
//     slotParameters[1].InitAsDescriptorTable(1, &table1);
//     slotParameters[2].InitAsDescriptorTable(1, &table2);
//
//     CD3DX12_ROOT_SIGNATURE_DESC rootDesc(3, slotParameters, 0, nullptr,
//                                          D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
//
//     ComPtr<ID3DBlob> errorBlob = nullptr;
//     ComPtr<ID3DBlob> serializedBlob = nullptr;
//
//     HRESULT hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedBlob.GetAddressOf(),
//                                              errorBlob.GetAddressOf());
//     if (errorBlob != nullptr)
//     {
//         ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
//     }
//     ThrowIfFailed(hr);
//     md3dDevice->CreateRootSignature(0, serializedBlob->GetBufferPointer(), serializedBlob->GetBufferSize(),
//                                     IID_PPV_ARGS(mRootSignature.GetAddressOf()));
// }
//
// void ShapesApp::BuildShadersAndInputLayout()
// {
//     mShaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
//     mShaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");
//
//     mInputLayout =
//     {
//         {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
//         {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
//     };
// }
//
// void ShapesApp::BuildShapeGeometry()
// {
//     GeometryGenerator geoGen;
//     GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
//     GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
//     GeometryGenerator::MeshData sphere = geoGen.CreateGeosphere(0.5f, 3);
//     GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
//     GeometryGenerator::MeshData skull = geoGen.CreateSkull("skull.txt");
//
//     //
//     // We are concatenating all the geometry into one big vertex/index buffer.  So
//     // define the regions in the buffer each submesh covers.
//     //
//
//     // Cache the vertex offsets to each object in the concatenated vertex buffer.
//     UINT boxVertexOffset = 0;
//     UINT gridVertexOffset = (UINT)box.Vertices.size();
//     UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
//     UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();
//     UINT skullVertexOffset = cylinderVertexOffset + (UINT)cylinder.Vertices.size();
//
//     // Cache the starting index for each object in the concatenated index buffer.
//     UINT boxIndexOffset = 0;
//     UINT gridIndexOffset = (UINT)box.Indices32.size();
//     UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
//     UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();
//     UINT skullIndexxOffset = cylinderIndexOffset + (UINT)cylinder.Indices32.size();
//
//     // Define the SubmeshGeometry that cover different 
//     // regions of the vertex/index buffers.
//
//     SubmeshGeometry boxSubmesh;
//     boxSubmesh.IndexCount = (UINT)box.Indices32.size();
//     boxSubmesh.StartIndexLocation = boxIndexOffset;
//     boxSubmesh.BaseVertexLocation = boxVertexOffset;
//
//     SubmeshGeometry gridSubmesh;
//     gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
//     gridSubmesh.StartIndexLocation = gridIndexOffset;
//     gridSubmesh.BaseVertexLocation = gridVertexOffset;
//
//     SubmeshGeometry sphereSubmesh;
//     sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
//     sphereSubmesh.StartIndexLocation = sphereIndexOffset;
//     sphereSubmesh.BaseVertexLocation = sphereVertexOffset;
//
//     SubmeshGeometry cylinderSubmesh;
//     cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
//     cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
//     cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;
//
//     SubmeshGeometry skullSubmesh;
//     skullSubmesh.IndexCount = (UINT)skull.Indices32.size();
//     skullSubmesh.StartIndexLocation = skullIndexxOffset;
//     skullSubmesh.BaseVertexLocation = skullVertexOffset;
//
//     //
//     // Extract the vertex elements we are interested in and pack the
//     // vertices of all the meshes into one vertex buffer.
//     //
//
//     auto totalVertexCount =
//         box.Vertices.size() +
//         grid.Vertices.size() +
//         sphere.Vertices.size() +
//         cylinder.Vertices.size() +
//         skull.Vertices.size();
//
//     std::vector<Vertex> vertices(totalVertexCount);
//
//     UINT k = 0;
//     for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
//     {
//         vertices[k].Pos = box.Vertices[i].Position;
//         vertices[k].Normal = box.Vertices[i].Normal;
//     }
//
//     for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
//     {
//         vertices[k].Pos = grid.Vertices[i].Position;
//         vertices[k].Normal = grid.Vertices[i].Normal;
//     }
//
//     for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
//     {
//         vertices[k].Pos = sphere.Vertices[i].Position;
//         vertices[k].Normal = sphere.Vertices[i].Normal;
//     }
//
//     for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
//     {
//         vertices[k].Pos = cylinder.Vertices[i].Position;
//         vertices[k].Normal = cylinder.Vertices[i].Normal;
//     }
//
//     for (size_t i = 0; i < skull.Vertices.size(); ++i, ++k)
//     {
//         vertices[k].Pos = skull.Vertices[i].Position;
//         vertices[k].Normal = skull.Vertices[i].Normal;
//     }
//     std::vector<std::uint16_t> indices;
//     indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
//     indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
//     indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
//     indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));
//     indices.insert(indices.end(), std::begin(skull.GetIndices16()), std::end(skull.GetIndices16()));
//
//     const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
//     const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);
//
//     auto geo = std::make_unique<MeshGeometry>();
//     geo->Name = "shapeGeo";
//
//     ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
//     CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
//
//     ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
//     CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
//
//     geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
//                                                         mCommandList.Get(), vertices.data(), vbByteSize,
//                                                         geo->VertexBufferUploader);
//
//     geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
//                                                        mCommandList.Get(), indices.data(), ibByteSize,
//                                                        geo->IndexBufferUploader);
//
//     geo->VertexByteStride = sizeof(Vertex);
//     geo->VertexBufferByteSize = vbByteSize;
//     geo->IndexFormat = DXGI_FORMAT_R16_UINT;
//     geo->IndexBufferByteSize = ibByteSize;
//
//     geo->DrawArgs["box"] = boxSubmesh;
//     geo->DrawArgs["grid"] = gridSubmesh;
//     geo->DrawArgs["sphere"] = sphereSubmesh;
//     geo->DrawArgs["cylinder"] = cylinderSubmesh;
//     geo->DrawArgs["skull"] = skullSubmesh;
//
//     mGeometries[geo->Name] = std::move(geo);
// }
//
// void ShapesApp::BuildMaterials()
// {
//     auto box = std::make_unique<Material>();
//     box->Name = "box";
//     box->MatCBIndex = 0;
//     box->DiffuseAlbedo = XMFLOAT4(0.9f, 0.1f, 0.2f, 1.0f);
//     box->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
//     box->Roughness = 0.25f;
//
//     auto cylinder = std::make_unique<Material>();
//     cylinder->Name = "cylinder";
//     cylinder->MatCBIndex = 1;
//     cylinder->DiffuseAlbedo = XMFLOAT4(0.19f, 0.91f, 0.27f, 1.0f);
//     cylinder->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
//     cylinder->Roughness = 0.65f;
//
//     auto grid = std::make_unique<Material>();
//     grid->Name = "grid";
//     grid->MatCBIndex = 2;
//     grid->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.82f, 1.0f);
//     grid->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
//     grid->Roughness = 0.85f;
//
//     auto sphere = std::make_unique<Material>();
//     sphere->Name = "sphere";
//     sphere->MatCBIndex = 3;
//     sphere->DiffuseAlbedo = XMFLOAT4(0.39f, 0.75f, 0.62f, 1.0f);
//     sphere->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
//     sphere->Roughness = 0.45f;
//
//     auto skull = std::make_unique<Material>();
//     skull->Name = "skull";
//     skull->MatCBIndex = 4;
//     skull->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//     skull->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
//     skull->Roughness = 0.6f;
//
//     mMaterials["box"] = std::move(box);
//     mMaterials["sphere"] = std::move(sphere);
//     mMaterials["grid"] = std::move(grid);
//     mMaterials["cylinder"] = std::move(cylinder);
//     mMaterials["skull"] = std::move(skull);
// }
//
// void ShapesApp::BuildPSO()
// {
//     D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
//
//     ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
//
//     opaquePsoDesc.InputLayout = {mInputLayout.data(), (UINT)mInputLayout.size()};
//     opaquePsoDesc.pRootSignature = mRootSignature.Get();
//     opaquePsoDesc.VS =
//     {
//         reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
//         mShaders["standardVS"]->GetBufferSize()
//     };
//     opaquePsoDesc.PS =
//     {
//         reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
//         mShaders["opaquePS"]->GetBufferSize()
//     };
//     opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//     opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
//     opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//     opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
//     opaquePsoDesc.SampleMask = UINT_MAX;
//     opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//     opaquePsoDesc.NumRenderTargets = 1;
//     opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
//     opaquePsoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
//     opaquePsoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
//     opaquePsoDesc.DSVFormat = mDepthStencilFormat;
//     ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));
//
//     D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
//     opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
//     ThrowIfFailed(
//         md3dDevice->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
// }
//
//
// void ShapesApp::BuildRenderItems()
// {
//     auto boxRitem = std::make_unique<RenderItem>();
//     XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
//     boxRitem->ObjCBIndex = 0;
//     boxRitem->Mat = mMaterials["box"].get();
//     boxRitem->Geo = mGeometries["shapeGeo"].get();
//     boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//     boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
//     boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
//     boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
//     mAllRitems.push_back(std::move(boxRitem));
//
//     auto gridRitem = std::make_unique<RenderItem>();
//     gridRitem->World = MathHelper::Identity4x4();
//     gridRitem->Mat = mMaterials["grid"].get();
//     gridRitem->ObjCBIndex = 1;
//     gridRitem->Geo = mGeometries["shapeGeo"].get();
//     gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//     gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
//     gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
//     gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
//     mAllRitems.push_back(std::move(gridRitem));
//
//     UINT objCBIndex = 2;
//     for (int i = 0; i < 5; ++i)
//     {
//         auto leftCylRitem = std::make_unique<RenderItem>();
//         auto rightCylRitem = std::make_unique<RenderItem>();
//         auto leftSphereRitem = std::make_unique<RenderItem>();
//         auto rightSphereRitem = std::make_unique<RenderItem>();
//
//         XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
//         XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);
//
//         XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
//         XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);
//
//         XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
//         leftCylRitem->ObjCBIndex = objCBIndex++;
//         leftCylRitem->Geo = mGeometries["shapeGeo"].get();
//         leftCylRitem->Mat = mMaterials["cylinder"].get();
//         leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//         leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
//         leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
//         leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
//
//         XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
//         rightCylRitem->ObjCBIndex = objCBIndex++;
//         rightCylRitem->Geo = mGeometries["shapeGeo"].get();
//         rightCylRitem->Mat = mMaterials["cylinder"].get();
//         rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//         rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
//         rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
//         rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
//
//         XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
//         leftSphereRitem->ObjCBIndex = objCBIndex++;
//         leftSphereRitem->Mat = mMaterials["sphere"].get();
//         leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
//         leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//         leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
//         leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
//         leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
//
//         XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
//         rightSphereRitem->ObjCBIndex = objCBIndex++;
//         rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
//         rightSphereRitem->Mat = mMaterials["sphere"].get();
//         rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//         rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
//         rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
//         rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
//
//         mAllRitems.push_back(std::move(leftCylRitem));
//         mAllRitems.push_back(std::move(rightCylRitem));
//         mAllRitems.push_back(std::move(leftSphereRitem));
//         mAllRitems.push_back(std::move(rightSphereRitem));
//     }
//
//     auto skullRitem = std::make_unique<RenderItem>();
//     XMStoreFloat4x4(&skullRitem->World, XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
//     skullRitem->ObjCBIndex = objCBIndex++;
//     skullRitem->Geo = mGeometries["shapeGeo"].get();
//     skullRitem->Mat = mMaterials["skull"].get();
//     skullRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//     skullRitem->IndexCount = skullRitem->Geo->DrawArgs["skull"].IndexCount;
//     skullRitem->StartIndexLocation = skullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
//     skullRitem->BaseVertexLocation = skullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;
//     mAllRitems.push_back(std::move(skullRitem));
//
//     // All the render items are opaque.
//     for (auto& e : mAllRitems)
//         mOpaqueRitems.push_back(e.get());
// }
//
// void ShapesApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*> ritems)
// {
//     UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
//
//     auto objectCB = mCurrFrameResource->ObjectCB->Resource();
//
//     for (size_t i = 0; i < ritems.size(); i++)
//     {
//         auto ri = ritems[i];
//
//         cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
//         cmdList->IASetPrimitiveTopology(ri->PrimitiveType);
//         cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
//
//         UINT cbvIndex = mCurrFrameResourceIndex * (UINT)mOpaqueRitems.size() + ri->ObjCBIndex;
//         auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
//         cbvHandle.Offset(cbvIndex, mCbvSrvUavDescriptorSize);
//         // XMMATRIX world = XMLoadFloat4x4(&ri->World);
//         // ObjectConstants objConstants;
//         // XMStoreFloat4x4(&objConstants.Word, XMMatrixTranspose(world));
//         // cmdList->SetGraphicsRoot32BitConstants(0,16, &objConstants,0);
//         cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);
//
//         UINT cbvIndex1 = gNumFrameResources * (UINT)mOpaqueRitems.size() + mCurrFrameResourceIndex * (UINT)mMaterials.
//             size() + ri->Mat->MatCBIndex;
//         auto cbvHandle1 = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
//         cbvHandle1.Offset(cbvIndex1, mCbvSrvUavDescriptorSize);
//         cmdList->SetGraphicsRootDescriptorTable(1, cbvHandle1);
//
//         cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
//     }
// }
