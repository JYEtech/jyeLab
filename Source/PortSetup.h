//---------------------------------------------------------------------------

#ifndef PortSetupH
#define PortSetupH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfmPortSetup : public TForm
{
__published:	// IDE-managed Components
    TLabel *lbPortNameTag;
    TLabel *lbBaudrate;
    TLabel *lbDataBits;
    TLabel *lbStopBits;
    TLabel *lbParity;
    TComboBox *cbBaudrate;
    TComboBox *cbDataBits;
    TComboBox *cbStopBits;
    TComboBox *cbParity;
    TButton *btnOK;
    TComboBox *cbPortList;
    TLabel *lbFlowCtrl;
    TComboBox *cbFlowCtrl;
    TButton *btnPortScan;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall cbPortListChange(TObject *Sender);
    void __fastcall btnPortScanClick(TObject *Sender);
    void __fastcall btnOKClick(TObject *Sender);
        void __fastcall cbBaudrateChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TfmPortSetup(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmPortSetup *fmPortSetup;
//---------------------------------------------------------------------------
#endif
