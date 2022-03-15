#pragma once
#include "d3dApp.h"
#include <DirectXColors.h>

using namespace DirectX;

class InitDirecrt3DApp :public D3DApp
{
public:
	InitDirecrt3DApp(HINSTANCE hInstance);
	~InitDirecrt3DApp();
	virtual bool Initialize()override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;
};

