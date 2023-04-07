#include "DirectXColors.h"
#include "Chapter7/ShapesApp.h"

using namespace DirectX;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE preInstance,PSTR cmdLine,int showCmd)
{
#if defined(DEBUG)|defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        ShapesApp theApp(hInstance);
        if(!theApp.Initialize())
            return 0;
        
        return theApp.Run();
    }
    catch (DxException &e)
    {
        MessageBox(nullptr,e.ToString().c_str(),L"HR Failed",MB_OK);
        return 0;
    }
}