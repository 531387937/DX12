#pragma once

#include <d3d12.h>
#include <wrl/event.h>


#include "string"
struct Texture
{
    std::string Name;

    std::wstring Filename;

    Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
};

class FrameResource
{
public:
    
};
