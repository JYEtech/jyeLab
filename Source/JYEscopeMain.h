//---------------------------------------------------------------------------

#ifndef JYEscopeMainH
#define JYEscopeMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Graphics.hpp>
#include <ActnList.hpp>
#include <ActnMan.hpp>
#include <ImgList.hpp>
#include <StdActns.hpp>
#include <ActnCtrls.hpp>
#include <ActnMenus.hpp>
#include <ToolWin.hpp>
//#include <ctype.h>
#include <stdio.h>
//---------------------------------------------------------------------------

#include "Common.h"
#include "Scope.h"
#include <ComCtrls.hpp>
#include <Menus.hpp>

#define	Screenx0    		2
#define	Screeny0		30
#define	ScreenSizex		760
#define	ScreenSizey		380

// 0V position
#define	V0				((256/2) * 3)

// Vpos pane
#define	PaneVposOfsx	0
#define	PaneVposOfsy	0
#define	PaneVposSizex	50
#define	PaneVposSizey	380

// Hpos pane
#define	PaneHposOfsx	50
#define	PaneHposOfsy	0
#define	PaneHposSizex	(660 + 1)
#define	PaneHposSizey	40

// Readout pane
#define	PaneReadoutOfsx	50
#define	PaneReadoutOfsy	340
#define	PaneReadoutSizex	(660 + 1)
#define	PaneReadoutSizey	40

// Trigger level pane
#define	PaneTrigLvlOfsx		710
#define	PaneTrigLvlOfsy		0
#define	PaneTrigLvlSizex	50
#define	PaneTrigLvlSizey	380

#define	PaneWaveformOfsx	50
#define	PaneWaveformOfsy	40
#define	PaneWaveformSizex	(660 + 1)
#define	PaneWaveformSizey	(300 + 1)

//#define	WWindowx0		52
//#define	WWindowy0		60
#define	WWindowOfsx	50
#define	WWindowOfsy	40
#define	WWindowSizex	(660 + 1)
#define	WWindowSizey	(300 + 1)

#define	GridSizex		30
#define	GridSizey		30

//#define	Ch1ReadOutOfsx		(WWindowOfsx + 5)
//#define	Ch1ReadOutOfsy		(WWindowOfsy + WWindowSizey + 10)
#define	Ch1ReadOutOfsx		5
#define	Ch1ReadOutOfsy		10
#define	Ch1ReadOutSizex	135
#define	Ch1ReadOutSizey	25

#define	Ch2ReadOutOfsx		(Ch1ReadOutOfsx + Ch1ReadOutSizex + 10)
#define	Ch2ReadOutOfsy		Ch1ReadOutOfsy
#define	Ch2ReadOutSizex	135
#define	Ch2ReadOutSizey	25

#define	TBReadOutOfsx		(Ch2ReadOutOfsx + Ch2ReadOutSizex + 10)
#define	TBReadOutOfsy		Ch1ReadOutOfsy
#define	TBReadOutSizex		100
#define	TBReadOutSizey		25

#define	TriggerReadOutOfsx		(TBReadOutOfsx + TBReadOutSizex + 10)
#define	TriggerReadOutOfsy		Ch1ReadOutOfsy
#define	TriggerReadOutSizex		255
#define	TriggerReadOutSizey		25

#define	TrigModeReadOutOfsx	(TriggerReadOutOfsx + 3)
#define	TrigModeReadOutOfsy	Ch1ReadOutOfsy

#define	TrigSlopeReadOutOfsx	(TrigModeReadOutOfsx + 108 + 5)
#define	TrigSlopeReadOutOfsy	Ch1ReadOutOfsy + PaneReadoutOfsy

#define	TrigSrcReadOutOfsx		(TrigSlopeReadOutOfsx + 18 + 5)
#define	TrigSrcReadOutOfsy		Ch1ReadOutOfsy

#define	TrigLvlReadOutOfsx		(TrigSrcReadOutOfsx + 64 + 8)
#define	TrigLvlReadOutOfsy		Ch1ReadOutOfsy

#define	HPosIndicatorOfsx		205
#define	HPosIndicatorOfsy		20
#define	HPosIndicatorSizex		250
#define	HPosIndicatorSizey		15

#define	RunStopReadOutOfsx		620
#define	RunStopReadOutOfsy		15
#define	RunStopReadOutSizex		80
#define	RunStopReadOutSizey		22

#define	ConnectReadOutOfsx		(RunStopReadOutOfsx + RunStopReadOutSizex + 10)
#define	ConnectReadOutOfsy		RunStopReadOutOfsy
#define	ConnectReadOutSizex		80
#define	ConnectReadOutSizey		22

#define	TrigPosOfsx				(380 - 8)
#define	TrigPosOfsy				(HPosIndicatorOfsy + (HPosIndicatorSizey/2) - 12)

#define	TrigPointOfsx			(380 - 3)
#define	TrigPointOfsy			(WWindowOfsy + WWindowSizey + 1)

// Locations for Ch1 and Ch2 vertical position buttons
#define	ButtonSizex				15
#define	ButtonSizey				15

#define	Ch1PosUpOfsx			10
#define	Ch1PosUpOfsy			17
#define	Ch1PosDownOfsx			Ch1PosUpOfsx
#define	Ch1PosDownOfsy			(WWindowOfsy + WWindowSizey + 8)

#define	Ch2PosUpOfsx			(Ch1PosUpOfsx + ButtonSizex + 5)
#define	Ch2PosUpOfsy			Ch1PosUpOfsy
#define	Ch2PosDownOfsx			(Ch1PosDownOfsx + ButtonSizex + 5)
#define	Ch2PosDownOfsy			(WWindowOfsy + WWindowSizey + 8)

// Locations for trigger level buttons
#define	TrigLvlUpOfsx			(WWindowOfsx + WWindowSizex + 5)
#define	TrigLvlUpOfsy			Ch1PosUpOfsy
#define	TrigLvlDownOfsx			TrigLvlUpOfsx
#define	TrigLvlDownOfsy			(WWindowOfsy + WWindowSizey+ 8)

#define	VPosIndMax				(Screeny0 + WWindowOfsy + WWindowSizey - 1 - 8)
#define	VPosIndMin				(Screeny0 + WWindowOfsy - 8)

#define	clTrigger				0xFF99FF

// DSO Commands:
enum {
// 0x20 - 0x2F, sent by host
	CMD_DSO_GetConfig 		= 0x20,
	CMD_DSO_GetParam 		= 0x21,
	CMD_DSO_SetParam 		= 0x22,
	CMD_DSO_GetData  		= 0x23,
	CMD_DSO_SetMode  		= 0x24,

// 0x30 - 0x3F, sent by scope
	CMD_DSO_SendConfig  	= 0x30,
	CMD_DSO_SendParam  	= 0x31,
	CMD_DSO_SendBlock  	= 0x32,		// (block of samples)
	CMD_DSO_SendSample  	= 0x33,		//  (single sample)
	CMD_DSO_IsUSBscope	= 0x34,
};


class TJYEscope : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *Ch1;
    TRadioButton *rbCh1_50mV;
    TRadioButton *rbCh1_20mV;
    TRadioButton *rbCh1_10mV;
    TRadioButton *rbCh1_02V;
    TRadioButton *rbCh1_01V;
    TRadioButton *rbCh1_05V;
    TRadioButton *rbCh1_2V;
    TRadioButton *rbCh1_1V;
    TRadioButton *rbCh1_5V;
    TGroupBox *Ch2;
    TRadioButton *rbCh2_50mV;
    TRadioButton *rbCh2_20mV;
    TRadioButton *rbCh2_10mV;
    TRadioButton *rbCh2_02V;
    TRadioButton *rbCh2_01V;
    TRadioButton *rbCh2_05V;
    TRadioButton *rbCh2_1V;
    TRadioButton *rbCh2_2V;
    TRadioButton *rbCh2_5V;
    TComboBox *cbCh1Cpl;
    TButton *btnCh1OnOff;
    TComboBox *cbCh2Cpl;
    TButton *btnCh2OnOff;
    TGroupBox *gbTimeBase;
    TRadioButton *rbTB05us;
    TRadioButton *rbTB02us;
    TRadioButton *rbTB01us;
    TRadioButton *rbTB1us;
    TRadioButton *rbTB2us;
    TRadioButton *rbTB5us;
    TRadioButton *rbTB20us;
    TRadioButton *rbTB50us;
    TRadioButton *rbTB01ms;
    TRadioButton *rbTB02ms;
    TRadioButton *rbTB05ms;
    TRadioButton *rbTB1ms;
    TRadioButton *rbTB2ms;
    TRadioButton *rbTB5ms;
    TRadioButton *rbTB10ms;
    TRadioButton *rbTB20ms;
    TRadioButton *rbTB50ms;
    TRadioButton *rbTB01s;
    TRadioButton *rbTB02s;
    TRadioButton *rbTB05s;
    TRadioButton *rbTB1s;
    TRadioButton *rbTB2s;
    TRadioButton *rbTB5s;
    TRadioButton *rbTB10s;
    TRadioButton *rbTB20s;
    TRadioButton *rbTB50s;
    TGroupBox *gbTrigger;
    TLabel *lbCh1Cpl;
    TLabel *lbCh2Cpl;
    TComboBox *cbTriggerMode;
    TComboBox *cbTriggerSlope;
    TComboBox *cbTriggerSrc;
    TLabel *lbTrigMode;
    TLabel *lbTrigSource;
    TLabel *lbTrigSlope;
    TLabel *lbTrigPos;
    TRadioButton *rbTB10us;
    TRadioButton *rbTB1min;
    TRadioButton *rbTB2min;
    TRadioButton *rbTB5min;
    TRadioButton *rbTB10min;
    TRadioButton *rbTB20min;
    TRadioButton *rbTBcustom;
    TGroupBox *Device;
        TButton *btnConnect;
    TGroupBox *Capture;
    TButton *btnRunStop;
    TGroupBox *Utilities;
    TButton *Measurement;
        TButton *btnSave;
        TButton *btnRecall;
        TButton *btnSettings;
    TImageList *ImageList1;
    TActionManager *ActionManager1;
    TAction *FileNew;
    TAction *FileSave;
    TAction *HelpAbout;
    TFileOpen *FileOpen1;
    TFileSaveAs *FileSaveAs1;
    TFileExit *FileExit1;
    TActionMainMenuBar *mnMain;
    TGroupBox *RecLen;
    TComboBox *cbRecordLength;
    TLabel *UnitModel;
    TLabel *UnitName;
    TLabel *UnitState;
    TImage *DeviceConnectionInd;
    TMemo *Memo1;
    TEdit *edTrigLvlReadout;
    TEdit *edTrigPos;
    TLabel *Label1;
    TEdit *edHPosReadout;
    TLabel *lbCh1Sen;
    TLabel *lbCh1;
    TLabel *lbCh1CplCurr;
    TLabel *lbCh2Sen;
    TLabel *lbCh2;
    TLabel *lbCh2CplCurr;
    TLabel *lbTB;
    TLabel *lbTimebase;
    TLabel *lbTrigModeCurr;
    TLabel *lbSrc;
    TLabel *lbTrigSrcCurr;
    TLabel *lbTL;
    TLabel *lbTrigLvlUnit;
    TLabel *lbTrigSen;
    TEdit *edTrigSen;
    TLabel *lbOscState;
    TAction *actPortSetup;
        TLabel *lbdT;
        TLabel *lbdV1;
        TLabel *lbdV2;
        TGroupBox *gbRecalls;
        TRadioButton *rbRecall0;
        TRadioButton *rbRecall1;
        TRadioButton *rbRecall2;
        TRadioButton *rbRecall3;
        TButton *btnDispCapture;
        TAction *FileClose;
        TAction *CaptureConnect;
        TAction *CaptureDisconnect;
        TAction *CaptureRun;
        TAction *CaptureStop;
        TAction *CaptureDisplay;
        TAction *GenDownload085;
        TAction *CaptureUpload;
        TAction *Action1;
        TAction *Action2;


    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);

void __fastcall CursorT1topMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall CursorT1topMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall CursorT1topMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall CursorT2topMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall CursorT2topMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall CursorT2topMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall CursorT1bottomMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall CursorT1bottomMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall CursorT1bottomMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall CursorT2bottomMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall CursorT2bottomMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall CursorT2bottomMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

void __fastcall Ch1Vcursor1leftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor1leftMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor1leftMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor2leftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor2leftMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor2leftMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor1rightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor1rightMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor1rightMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor2rightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor2rightMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall Ch1Vcursor2rightMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

void __fastcall Ch2Vcursor1leftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor1leftMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor1leftMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor2leftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor2leftMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor2leftMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor1rightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor1rightMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor1rightMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor2rightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor2rightMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
void __fastcall Ch2Vcursor2rightMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);

void __fastcall dTClick(TObject *Sender);
void __fastcall dV_1Click(TObject *Sender);
void __fastcall dV_2Click(TObject *Sender);

	void __fastcall Ch1PosMouseDown(TObject *Sender, 
		TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch1PosMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall Ch1PosMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch2PosMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch1PosDblClick(TObject *Sender);
	void __fastcall Ch2PosMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall Ch2PosMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch2PosDblClick(TObject *Sender);

	void __fastcall TrigLvlMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrigLvlMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall TrigLvlMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrigLvlDblClick(TObject *Sender);

	void __fastcall TrigLvlUpMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrigLvlUpMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrigLvlDownMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrigLvlDownMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);

	void __fastcall Ch1PosUpMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch1PosUpMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch1PosDownMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch1PosDownMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);

	void __fastcall Ch2PosUpMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch2PosUpMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch2PosDownMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Ch2PosDownMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);

	void __fastcall HPosBarMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall HPosBarMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall HPosBarMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);

	void __fastcall HPosRightMouseDown(TObject *Sender,
     		 TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall HPosRightMouseUp(TObject *Sender,
      		TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall HPosLeftMouseDown(TObject *Sender,
      		TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall HPosLeftMouseUp(TObject *Sender,
      		TMouseButton Button, TShiftState Shift, int X, int Y);

	void __fastcall TrigPosMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrigPosMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall TrigPosMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);

	void __fastcall TrigPointMouseDown(TObject *Sender,
	      TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrigPointMouseUp(TObject *Sender,
  	    TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TrigPointDblClick(TObject *Sender);
	
	void __fastcall TrigSlopeClick(TObject *Sender);
	
    void __fastcall btnSaveClick(TObject *Sender);
    void __fastcall btnRecallClick(TObject *Sender);
    void __fastcall btnRunStopClick(TObject *Sender);
    void __fastcall btnConnectClick(TObject *Sender);
    void __fastcall btnSettingsClick(TObject *Sender);
    void __fastcall rbCh1_10mVClick(TObject *Sender);
    void __fastcall rbCh1_20mVClick(TObject *Sender);
    void __fastcall rbCh1_50mVClick(TObject *Sender);
    void __fastcall rbCh1_01VClick(TObject *Sender);
    void __fastcall rbCh1_02VClick(TObject *Sender);
    void __fastcall rbCh1_05VClick(TObject *Sender);
    void __fastcall rbCh1_1VClick(TObject *Sender);
    void __fastcall rbCh1_2VClick(TObject *Sender);
    void __fastcall rbCh1_5VClick(TObject *Sender);
    void __fastcall rbCh2_10mVClick(TObject *Sender);
    void __fastcall rbCh2_20mVClick(TObject *Sender);
    void __fastcall rbCh2_50mVClick(TObject *Sender);
    void __fastcall rbCh2_01VClick(TObject *Sender);
    void __fastcall rbCh2_02VClick(TObject *Sender);
    void __fastcall rbCh2_05VClick(TObject *Sender);
    void __fastcall rbCh2_1VClick(TObject *Sender);
    void __fastcall rbCh2_2VClick(TObject *Sender);
    void __fastcall rbCh2_5VClick(TObject *Sender);
    void __fastcall cbCh1CplChange(TObject *Sender);
    void __fastcall cbCh2CplChange(TObject *Sender);
    void __fastcall rbTB1usClick(TObject *Sender);
    void __fastcall rbTB2usClick(TObject *Sender);
    void __fastcall rbTB5usClick(TObject *Sender);
    void __fastcall rbTB10usClick(TObject *Sender);
    void __fastcall rbTB20usClick(TObject *Sender);
    void __fastcall rbTB50usClick(TObject *Sender);
    void __fastcall rbTB01msClick(TObject *Sender);
    void __fastcall rbTB02msClick(TObject *Sender);
    void __fastcall rbTB05msClick(TObject *Sender);
    void __fastcall rbTB1msClick(TObject *Sender);
    void __fastcall rbTB2msClick(TObject *Sender);
    void __fastcall rbTB5msClick(TObject *Sender);
    void __fastcall rbTB10msClick(TObject *Sender);
    void __fastcall rbTB20msClick(TObject *Sender);
    void __fastcall rbTB50msClick(TObject *Sender);
    void __fastcall rbTB01sClick(TObject *Sender);
    void __fastcall rbTB02sClick(TObject *Sender);
    void __fastcall rbTB05sClick(TObject *Sender);
    void __fastcall rbTB1sClick(TObject *Sender);
    void __fastcall rbTB2sClick(TObject *Sender);
    void __fastcall rbTB5sClick(TObject *Sender);
    void __fastcall rbTB10sClick(TObject *Sender);
    void __fastcall rbTB20sClick(TObject *Sender);
    void __fastcall rbTB50sClick(TObject *Sender);
    void __fastcall rbTB1minClick(TObject *Sender);
    void __fastcall rbTB2minClick(TObject *Sender);
    void __fastcall rbTB5minClick(TObject *Sender);
    void __fastcall rbTB10minClick(TObject *Sender);
    void __fastcall rbTB05usClick(TObject *Sender);
    void __fastcall cbTriggerModeChange(TObject *Sender);
    void __fastcall cbTriggerSlopeChange(TObject *Sender);
    void __fastcall cbTriggerSrcChange(TObject *Sender);
    void __fastcall edTrigPosKeyPress(TObject *Sender, char &Key);
    void __fastcall cbRecordLengthChange(TObject *Sender);
    void __fastcall edTrigLvlReadoutKeyPress(TObject *Sender, char &Key);
    void __fastcall edHPosReadoutKeyPress(TObject *Sender, char &Key);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
//    void __fastcall cbPortListChange(TObject *Sender);
    void __fastcall edTrigSenKeyPress(TObject *Sender, char &Key);
//    void __fastcall btnPortScanClick(TObject *Sender);
//    void __fastcall btnPortSetupClick(TObject *Sender);
    void __fastcall actPortSetupExecute(TObject *Sender);
    void __fastcall btnCh1OnOffClick(TObject *Sender);
    void __fastcall btnCh2OnOffClick(TObject *Sender);
    void __fastcall lbCh1SenMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall lbCh2SenMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall lbTimebaseMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall lbTrigModeCurrMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall lbTrigSrcCurrMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall HelpAboutExecute(TObject *Sender);
    void __fastcall rbTB02usClick(TObject *Sender);
        void __fastcall FileOpen1Accept(TObject *Sender);
        void __fastcall FileSaveExecute(TObject *Sender);
        void __fastcall FileSaveAs1BeforeExecute(TObject *Sender);
        void __fastcall FileSaveAs1Accept(TObject *Sender);
        void __fastcall lbdV1Click(TObject *Sender);
        void __fastcall lbdTClick(TObject *Sender);
        void __fastcall lbdV2Click(TObject *Sender);
//        void __fastcall FileOpen1BeforeExecute(TObject *Sender);
        void __fastcall rbRecall0Click(TObject *Sender);
        void __fastcall rbRecall1Click(TObject *Sender);
        void __fastcall rbRecall2Click(TObject *Sender);
        void __fastcall rbRecall3Click(TObject *Sender);
        void __fastcall FileCloseExecute(TObject *Sender);
        void __fastcall btnDispCaptureClick(TObject *Sender);
        void __fastcall GenDownload085Execute(TObject *Sender);
        void __fastcall CaptureUploadExecute(TObject *Sender);
	 void __fastcall btnSelectBufferOKClick(TObject *Sender);
	 void __fastcall btnSelectBufferOKCancel(TObject *Sender);
	void __fastcall rbBuffers0Click(TObject *Sender);
	void __fastcall rbBuffers1Click(TObject *Sender);
	void __fastcall rbBuffers2Click(TObject *Sender);
	void __fastcall rbBuffers3Click(TObject *Sender);

private:	// User declarations

public:		// User declarations
    __fastcall TJYEscope(TComponent* Owner);

};

class JRecall {
public:
	Oscilloscope *Recall;	// Pointer to opened waveform
	TRadioButton *Btn;	// The associated radio button

// Member functions
	JRecall(void);
	~JRecall(void);
	
private:		// User declarations

public:		// User declarations

	} ;

void 	DisplayRefreshThreadStart(void *arg);
void 	DispRefresh(void);
void		SwapBuffer(void);
void 	ShowWave(void);
//S32 		DrawWave(U16 *WaveBuf, U16 *bufferend, S16  VPos, TColor Color, Graphics::TBitmap *BitmapCanvas);
S32 		DrawWave(Oscilloscope *osc, U16 chn,  TColor Color, Graphics::TBitmap *BitmapCanvas);
void		RepaintWave(Oscilloscope *osc);

void		VSenRefresh(void);
void		VPosRefresh(void);
void		CplRefresh(void);
void		TBRefresh(void);
void		HPosRefresh(void);
void		TrigModeRefresh(void);
void		TrigSlopeRefresh(void);
void		TrigSrcRefresh(void);
void		TrigLvlRefresh(void);
void		TrigPosRefresh(void);
void 	TrigSenRefresh(void);
void 	CursorT_Refresh(void);
void 	CursorV_Refresh(void);
void		OscStateRefresh(void);
void		UpdateWaveform(void);
void		ScanPorts(void);
void 	SaveWaveToFile(AnsiString filename);
Oscilloscope	*RecallWaveFromFile(AnsiString filename);
void		Transform(Oscilloscope *osc, U16 method);
void		BackupWaveform(Oscilloscope *osc);
void 	RepaintScreen(void);
void 	ConnectSuccessful(void);
void 	ConnectFailed(void);
void 	DisplayRecall(void);
bool 	IsInt(const char *str);
bool		LineParser(char * line, char **fields, U16 fieldnum, bool checkdigit);
bool 	ValidLine(FILE *file, char *str, U16 len, char **fields, U16 fieldnum, bool checkdigit);
void		DSO068_Setup(void);
void		DSO_CmdProc068(void);
void		SetParam(void);
void		SendParamToDevice(void);

//---------------------------------------------------------------------------
extern 	PACKAGE TJYEscope *JYEscope;
extern 	Oscilloscope	*Capturer;		// Pointer to instance of capturing data
extern 	Oscilloscope	*OnScreen;		// Pointer to currently displayed instance

extern	AnsiString	CplString[];
extern	AnsiString 	PortName;
extern	HANDLE		WaveformUpdateEvent;	// Event for swaping waveform buffer
extern	HANDLE		WaveformUploadEvent;	// Event for upload waveform
extern	HANDLE		ConnectWaitEvent;	// Event for connect/disconnect waiting
extern	unsigned int 	ConnectState;
extern	TImage 		*DeviceDisconnected;

extern	JRecall	*Recall[];
extern	char		FrontEndType;
extern	Oscilloscope *Upload;

//---------------------------------------------------------------------------
/*
void ShowWave(void);
void	DrawWave(U8 *WaveBuf, S16  VPos, TColor Color, Graphics::TBitmap *BitmapCanvas);
U16	GetIndVPos(U16 chn);
S16 SetIndVPos(U16 chn, S16 vposind);
*/

#endif

