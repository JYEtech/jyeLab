//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PortSetup.h"

#include "JYEscopeMain.h"
#include "Tserial_event.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmPortSetup *fmPortSetup;

extern 	Tserial_event *serial_port;

//---------------------------------------------------------------------------
__fastcall TfmPortSetup::TfmPortSetup(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfmPortSetup::FormCreate(TObject *Sender)
{
 U16 tmp0;

 ScanPorts();

 // Select the com port with name 'PortName"
 if((tmp0 = fmPortSetup->cbPortList->Items->IndexOf(PortName)) == -1) {
 	// 'PortName' is not in the PortList. Select the first item in the PortList
 	 fmPortSetup->cbPortList->ItemIndex = 0;
 	}
 else {
 	fmPortSetup->cbPortList->ItemIndex = tmp0;
 	}
 PortName = fmPortSetup->cbPortList->Items->Strings[fmPortSetup->cbPortList->ItemIndex];
 JYEscope->Memo1->Lines->Append(PortName + " selected");
 
 tmp0 = this->cbBaudrate->Items->IndexOf(IntToStr(serial_port->baudrate));
 if(tmp0 != -1) {
 	this->cbBaudrate->ItemIndex = tmp0;
 	}
 else {
 	this->cbBaudrate->ItemIndex = 0;
 	}
}
//---------------------------------------------------------------------------

void __fastcall TfmPortSetup::cbPortListChange(TObject *Sender)
{
 JYEscope->Memo1->Lines->Append(this->cbPortList->Items->Strings[this->cbPortList->ItemIndex] + " selected");
 PortName = this->cbPortList->Items->Strings[this->cbPortList->ItemIndex];    
    
}
//---------------------------------------------------------------------------

void __fastcall TfmPortSetup::btnPortScanClick(TObject *Sender)
{
 ScanPorts();    
 this->cbPortList->ItemIndex = 0;
 PortName = this->cbPortList->Items->Strings[this->cbPortList->ItemIndex];    
 JYEscope->Memo1->Lines->Append(PortName + " selected");
}
//---------------------------------------------------------------------------

void __fastcall TfmPortSetup::btnOKClick(TObject *Sender)
{
 serial_port->baudrate = this->cbBaudrate->Items->Strings[this->cbBaudrate->ItemIndex].ToInt(); 
 JYEscope->Memo1->Lines->Append("Baudrate set to " + IntToStr(serial_port->baudrate) + " bps");
 fmPortSetup->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TfmPortSetup::cbBaudrateChange(TObject *Sender)
{
 serial_port->baudrate = this->cbBaudrate->Items->Strings[this->cbBaudrate->ItemIndex].ToInt(); 
 JYEscope->Memo1->Lines->Append("Baudrate set to " + IntToStr(serial_port->baudrate) + " bps");

}
//---------------------------------------------------------------------------

