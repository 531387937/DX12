#pragma once
#include "d3dApp.h"

class InitDirect3DApp:public D3DApp
{
public:
    InitDirect3DApp(HINSTANCE hInstance);
    ~InitDirect3DApp();

    bool Initialize() override;
private:
    void OnResize() override;
    void Draw(const GameTimer& gt) override;
    void Update(const GameTimer& gt) override;
};
