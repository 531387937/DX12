#include "InitDirect3D.h"
#include "d3dApp.h"
#include "GameTimer.h"
#include "UploadBuffer.h"
#include <DirectXColors.h>

struct Vertex {
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};
Vertex vertices[] =
{
	{XMFLOAT3(-1.0f,-1.0f,-1.0f),XMFLOAT4(Colors::White)},
	{XMFLOAT3(-1.0f,1.0f,-1.0f),XMFLOAT4(Colors::Black)},
	{XMFLOAT3(1.0f,1.0f,-1.0f),XMFLOAT4(Colors::Red)},
	{XMFLOAT3(1.0f,-1.0f,-1.0f),XMFLOAT4(Colors::Green)},
	{XMFLOAT3(-1.0f,-1.0f,1.0f),XMFLOAT4(Colors::Blue)},
	{XMFLOAT3(-1.0f,+1.0f,+1.0f),XMFLOAT4(Colors::Yellow)},
	{XMFLOAT3(1.0f,1.0f,1.0f),XMFLOAT4(Colors::Cyan)},
	{XMFLOAT3(1.0f,-1.0f,1.0f),XMFLOAT4(Colors::Magenta)}
};

class Box :D3DApp {
private:
	void OnMouseMove(WPARAM btnState, int x, int y);
	void Update(const GameTimer& gt);

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;
	POINT mLastMousePos;
};

void Box::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		//Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		//Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		//Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		//Make each pixel correspond to 0.005 until in the scene.
		float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

		//Update the camera radius based on input.
		mRadius += dx - dy;

		//Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
void Box::Update(const GameTimer& gt)
{
	//Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	//Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view * proj;

	//Update the constant buffer with the latest worldViewProj matrix.
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	mObjectCB->CopyData(0, objConstants);
}

std::uint16_t indices[] = {
	//front face
	0,1,2,
	0,2,3,

	//back face
	4,6,5,
	4,7,6,

	//left face
	4,5,1,
	4,1,0,

	//right face
	3,2,6,
	3,6,7,

	//top face
	1,5,6,
	1,6,2,

	//bottom face
	4,0,3,
	4,3,7
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	//Enable run-time memory check for debug builds.
#if defined(DEBUG)|defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		InitDirect3DApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}