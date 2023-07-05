 #pragma once
 #include "d3dApp.h"
#include "PBR/PBRApp.h"



 int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE preInstance, PSTR cmdLine, int showCmd)
 {
#if defined(DEBUG)|defined(_DEBUG)
     _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

     try
     {
         PBRApp theApp(hInstance);
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
