//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "About.h"
#include "JYEscopeMain.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutBox *AboutBox;
//--------------------------------------------------------------------- 
__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------
void __fastcall TAboutBox::FormCreate(TObject *Sender)
{
JYEscope->Memo1->Lines->Append(AboutBox->ProductName->Caption + "  " + AboutBox->Version->Caption );
//JYEscope->Memo1->Lines->Append(AboutBox->Version->Caption);
JYEscope->Memo1->Lines->Append(AboutBox->Copyright->Caption);
JYEscope->Memo1->Lines->Append(AboutBox->Comments->Caption);
JYEscope->Memo1->Lines->Append("-----------------------------------------");
JYEscope->Memo1->Lines->Append(" ");

}
//---------------------------------------------------------------------------

