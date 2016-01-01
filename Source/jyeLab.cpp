//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Scope.h"

//---------------------------------------------------------------------------
USEFORM("JYEscopeMain.cpp", JYEscope);
USEFORM("PortSetup.cpp", fmPortSetup);
USEFORM("About.cpp", AboutBox);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->Title = "jyeLab v0.70";
                 Application->CreateForm(__classid(TJYEscope), &JYEscope);
         Application->CreateForm(__classid(TAboutBox), &AboutBox);
         Application->CreateForm(__classid(TfmPortSetup), &fmPortSetup);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
