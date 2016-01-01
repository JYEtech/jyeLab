//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#define  STRICT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <conio.h>
#include <windows.h>

#include "JYEscopeMain.h"
#include "Common.h"
#include "Scope.h"
#include "Tserial_event.h"
#include "PortSetup.h"

#include <memory>
#include <math.h>
#include "About.h"

typedef unsigned (WINAPI *PBEGINTHREADEX_THREADFUNC) (LPVOID lpThreadParameter);
typedef unsigned *PBEGINTHREADEX_THREADID;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TJYEscope *JYEscope;
#pragma resource "jyeLab.res"


char 	VSenInMicroVolt[VS_Last][20] = {
		"00000000",	//	VS_20V,
		"100000000",	//	VS_100V,
		"50000000",	//	VS_50V,
		"20000000",	//	VS_20V,
		"10000000",	//	VS_10V,
		"5000000",	//	VS_5V,
		"2000000",	//	VS_2V,
		"1000000",	//	VS_1V,
		"500000",	//	VS_05V,
		"200000",	//	VS_02V,
		"100000",	//	VS_01V,
		"50000",		//	VS_50mV,
		"20000",		//	VS_20mV,
		"10000",		//	VS_10mV,
		"5000",		//	VS_5mV,
		"2000",		//	VS_2mV,
		"1000"		//	VS_1mV,
					//	VS_Last
		};

char		TBinNanoSecond[TB_max][20] = {
		"0",						//	TB_Custom,
		"3000000000000",			//	TB_50m,
		"1200000000000",			//	TB_20m,
		"600000000000",			//	TB_10m,	//		60s
		"300000000000",			//	TB_5m,		//		30s
		"120000000000",			//	TB_2m,		//		12s
		"60000000000",			//	TB_1m,		//		6s
		"50000000000",			//	TB_50s,		//		5s
		"20000000000",			//	TB_20s,		// 		2s
		"10000000000",			//	TB_10s,		// 1Hz	1s
		"5000000000",			//	TB_5s,		// 2Hz	500ms
		"2000000000",			//	TB_2s,		// 5Hz	200ms
		"1000000000",			//	TB_1s,		// 10Hz	100ms
		"500000000",				//	TB_05s,		// 20Hz	50ms
		"200000000",				//	TB_02s,		// 50Hz	20ms
		"100000000",				//	TB_01s,		// 100Hz	10ms
		"50000000",				//	TB_50ms,	// 200Hz	5ms
		"20000000",				//	TB_20ms,	// 500Hz	2ms
		"10000000",				//	TB_10ms,	// 1K	1ms
		"5000000",				//	TB_5ms,		// 2K	500us
		"2000000",				//	TB_2ms,		// 5K	200us
		"1000000",				//	TB_1ms,		// 10K	100us
		"500000",				//	TB_05ms,	// 20K	50us
		"200000",				//	TB_02ms,	// 50K	20us (400 cycles)
		"100000",				//	TB_01ms,	// 100K	10us (200 cycles)
		"50000",					//	TB_50us,	// 200K	100 cycles
		"20000",					//	TB_20us,	// 500K	40 cycles
		"10000",					//	TB_10us,	// 1M	20 cycles
		"5000",					//	TB_5us,		// 2M	10 cycles
		"2000",					//	TB_2us,		// 5M	
		"1000",					//	TB_1us,		// 10M
		"500",					//	TB_05us,	// 20M
		"200",					//	TB_02us,	// 50M
		"100",					//	TB_01us,
		"50",					//	TB_50ns,
		"20",					//	TB_20ns,
		"10",					//	TB_10ns,
		"5"						//	TB_5ns,

		};

char		WaveformFileSignature[5];
char		FileType[12];
char		FileDate[10];
char		FileTime[10];
char		FrontEndType;

TForm *frUploadWaveform = NULL;
TButton	*btnSelectBufferOK = 0;
TButton 	*btnSelectBufferCancel = 0;
TRadioGroup	*rgSelectBuffer = 0;
TRadioButton	*rbBuffers[4];
U8	SelectedBuffer = 0;

Graphics::TBitmap *HPosBarBitmap = 0;
Graphics::TBitmap	*Ch1SenBmp = 0;
Graphics::TBitmap	*Ch1CplBmp = 0;
Graphics::TBitmap	*Ch2SenBmp = 0;
Graphics::TBitmap	*Ch2CplBmp = 0;
Graphics::TBitmap	*TBBmp = 0;
Graphics::TBitmap	*TrigModeBmp = 0;
Graphics::TBitmap	*TrigSlopeBmp = 0;
Graphics::TBitmap	*TrigSrcBmp = 0;
Graphics::TBitmap	*TrigLvlUnitBmpmV = 0;
Graphics::TBitmap	*TrigLvlUnitBmpV = 0;

// Image buttons
TImage *Ch1Pos = 0;
TImage *Ch2Pos = 0;
TImage *Ch1PosUp = 0;
TImage *Ch2PosUp = 0;
TImage *Ch1PosDown = 0;
TImage *Ch2PosDown = 0;
//TImage *TrigLvl = 0;
TImage *TrigLvl_Ch1 = 0;
TImage *TrigLvl_Ch2 = 0;
TImage *TrigLvlUp = 0;
TImage *TrigLvlDown = 0;
TImage *HPosBar = 0;
TImage *TrigPos = 0;

TImage	*Ch1Sen = 0;
TImage	*Ch1Cpl = 0;
TImage	*Ch2Sen = 0;
TImage	*Ch2Cpl = 0;
TImage	*TB = 0;
TImage	*TrigMode = 0;
TImage	*TrigSlope = 0;
TImage	*TrigSrc = 0;
TImage	*PosSlopeBmp = 0;
TImage	*NegSlopeBmp = 0;
TImage	*TrigLvlUnit = 0;
TImage	*HPosRightBtn = 0;
TImage	*HPosLeftBtn = 0;

TImage	*OscStateInd = 0;
TImage	*OscStateRunning = 0;
TImage	*OscStateStopped = 0;
TImage	*OscStateOffline = 0;

TImage	*TPOutLeft = 0;
TImage	*TPOutRight = 0;

TImage	*dT = 0;
TImage	*dV_1 = 0;
TImage	*dV_2 = 0;

TImage	*CursorT1_Top = 0;
TImage	*CursorT1_Bottom = 0;
TImage	*CursorT2_Top = 0;
TImage	*CursorT2_Bottom = 0;
TImage	*Ch1Vcursor1_Left = 0;
TImage	*Ch1Vcursor1_Right = 0;
TImage	*Ch1Vcursor2_Left = 0;
TImage	*Ch1Vcursor2_Right = 0;
TImage	*Ch2Vcursor1_Left = 0;
TImage	*Ch2Vcursor1_Right = 0;
TImage	*Ch2Vcursor2_Left = 0;
TImage	*Ch2Vcursor2_Right = 0;

TImage	*CursorV_Left_Yellow = 0;
TImage	*CursorV_Right_Yellow = 0;
TImage	*CursorV_Left_Aqua = 0;
TImage	*CursorV_Right_Aqua = 0;

//Graphics::TBitmap *GridBackground = 0;

//Graphics::TBitmap *PaneWaveform = 0;
TImage *PaneWaveform = 0;

Graphics::TBitmap *PaneVpos = 0;
Graphics::TBitmap *PaneTrigLvl = 0;
Graphics::TBitmap *PaneHpos = 0;
Graphics::TBitmap *PaneReadout = 0;
Graphics::TBitmap *Waveform = 0;

TImage *DeviceConnected = 0;
TImage *DeviceDisconnected = 0;
TImage *TrigPoint = 0;

AnsiString PortName;

signed int  Mousex0, Mousey0;
unsigned char mousedown;
unsigned int ConnectState = 0;
U16		CurrRecallIndex = 0;		// The recall currently in display


U16			HPosBarWidthPrev;

TColor	BGColor;

Oscilloscope	*Capturer;		// Pointer to instance of capturing data
Oscilloscope	*OnScreen;		// Pointer to currently displayed instance

/*
Oscilloscope	*Wave[4] = {
	(Oscilloscope *)NULL,
	(Oscilloscope *)NULL,
	(Oscilloscope *)NULL,
	(Oscilloscope *)NULL
	};
*/

TRadioButton 	*ptr1;

HANDLE		WaveformUpdateEvent;		// Event for swaping waveform buffer
HANDLE		WaveformUploadEvent;		// Event for uploading saved waveform from frontend
HANDLE		ShowWaveMutex;
HANDLE		ConnectWaitEvent;			// Event for connect/disconnect waiting

unsigned int  	ThreadID_Disp;                     	// 
HANDLE		ThreadHandle;

JRecall	*Recall[4] = {
	NULL,
	NULL,
	NULL,
	NULL
	};

AnsiString      FileName = "Untitled.txt";

AnsiString	VSenString[18] = {
	"Custom",
	"100V/DIV",
	"50V/DIV",
	"20V/DIV",
	"10V/DIV",
	"5V/DIV",
	"2V/DIV",
	"1V/DIV",
	"0.5V/DIV",
	"0.2V/DIV",
	"0.1V/DIV",
	"50mV/DIV",
	"20mV/DIV",
	"10mV/DIV",
	"5mV/DIV",
	"2mV/DIV",
	"1mV/DIV",
	};

AnsiString	VSenUnitString[18] = {
	" ",		//	"Custom",
	"V",		//	"100V/DIV",
	"V",		//	"50V/DIV",
	"V",		//	"20V/DIV",
	"V",		//	"10V/DIV",
	"V",		//	"5V/DIV",
	"V",		//	"2V/DIV",
	"V",		//	"1V/DIV",
	"V",		//	"0.5V/DIV",
	"V",		//	"0.2V/DIV",
	"V",		//	"0.1V/DIV",
	"mV",	//	"50mV/DIV",
	"mV",	//	"20mV/DIV",
	"mV",	//	"10mV/DIV",
	"mV",	//	"5mV/DIV",
	"mV",	//	"2mV/DIV",
	"mV",	//	"1mV/DIV",
	};


float		VSenRate[18] = {
	1.0,		// Custom
	100.0,	// 100V/DIV
	50.0,	// 50V/DIV
	20.0,	// 20V/DIV
	10.0,	// 10V/DIV
	5.0,		// 5V/DIV
	2.0,		// 2V/DIV
	1.0,		// 1V/DIV
	0.5,		// 0.5V/DIV
	0.2,		// 0.2V/DIV
	0.1,		// 0.1V/DIV
	50.0,	// 50mV/DIV
	20.0,	// 20mV/DIV
	10.0,	// 10mV/DIV
	5.0,		// 5mV/DIV
	2.0,		// 2mV/DIV
	1.0,		// 1mV/DIV

};


AnsiString	CplString[4] = {
	"DC",
	"AC",
	"GND",
	};

AnsiString	TBString[TB_max] = {
	"Custom",
	"50min/DIV",
	"20min/DIV",
	"10min/DIV",
	"5min/DIV",
	"2min/DIV",
	"1min/DIV",
	"50s/DIV",
	"20s/DIV",
	"10s/DIV",
	"5s/DIV",
	"2s/DIV",
	"1s/DIV",
	"0.5s/DIV",
	"0.2s/DIV",
	"0.1s/DIV",
	"50ms/DIV",
	"20ms/DIV",
	"10ms/DIV",
	"5ms/DIV",
	"2ms/DIV",
	"1ms/DIV",
	"0.5ms/DIV",
	"0.2ms/DIV",
	"0.1ms/DIV",
	"50us/DIV",
	"20us/DIV",
	"10us/DIV",
	"5us/DIV",
	"2us/DIV",
	"1us/DIV",
	"0.5us/DIV",
	"0.2us/DIV",
	"0.1us/DIV",
	"50ns/DIV",
	"20s/DIV",
	"10ns/DIV",
	"5ns/DIV"
	};

AnsiString	TrigModeString[5] = {
	"AUTO",
	"NORMAL",
	"SINGLE",
	};

AnsiString	TrigSrcString[5] = {
	"Ch 1",
	"Ch 2",
	};

AnsiString	Ch1VsenButtonNames[VS_Last] = {
	"Custom",
	"rbCh1_100V",
	"rbCh1_50V",
	"rbCh1_20V",
	"rbCh1_10V",
	"rbCh1_5V",
	"rbCh1_2V",
	"rbCh1_1V",
	"rbCh1_05V",
	"rbCh1_02V",
	"rbCh1_01V",
	"rbCh1_50mV",
	"rbCh1_20mV",
	"rbCh1_10mV",
	"rbCh1_5mV",
	"rbCh1_2mV",
	"rbCh1_1mV"
	};

AnsiString	Ch2VsenButtonNames[VS_Last] = {
	"Custom",
	"rbCh2_100V",
	"rbCh2_50V",
	"rbCh2_20V",
	"rbCh2_10V",
	"rbCh2_5V",
	"rbCh2_2V",
	"rbCh2_1V",
	"rbCh2_05V",
	"rbCh2_02V",
	"rbCh2_01V",
	"rbCh2_50mV",
	"rbCh2_20mV",
	"rbCh2_10mV",
	"rbCh2_5mV",
	"rbCh2_2mV",
	"rbCh2_1mV"
	};

AnsiString	TBButtonNames[TB_max] = {
	"rbTBCustom",
	"rbTB50min",
	"rbTB20min",
	"rbTB10min",
	"rbTB5min",
	"rbTB2min",
	"rbTB1min",
	"rbTB50s",
	"rbTB20s",
	"rbTB10s",
	"rbTB5s",
	"rbTB2s",
	"rbTB1s",
	"rbTB05s",
	"rbTB02s",
	"rbTB01s",
	"rbTB50ms",
	"rbTB20ms",
	"rbTB10ms",
	"rbTB5ms",
	"rbTB2ms",
	"rbTB1ms",
	"rbTB05ms",
	"rbTB02ms",
	"rbTB01ms",
	"rbTB50us",
	"rbTB20us",
	"rbTB10us",
	"rbTB5us",
	"rbTB2us",
	"rbTB1us",
	"rbTB05us",
	"rbTB02us",
	"rbTB01us",
	"rbTB50ns",
	"rbTB20ns",
	"rbTB10ns",
	"rbTB5ns"
	};

AnsiString	TBUnitStrings[TB_max] = {
	"N/A",	//	"rbTBCustom",
	"min",	//	"rbTB50min",
	"min",	//	"rbTB20min",
	"min",	//	"rbTB10min",
	"min",	//	"rbTB5min",
	"min",	//	"rbTB2min",
	"min",	//	"rbTB1min",
	"s",		//	"rbTB50s",
	"s",		//	"rbTB20s",
	"s",		//	"rbTB10s",
	"s",		//	"rbTB5s",
	"s",		//	"rbTB2s",
	"s",		//	"rbTB1s",
	"s",		//	"rbTB05s",
	"s",		//	"rbTB02s",
	"s",		//	"rbTB01s",
	"ms",	//	"rbTB50ms",
	"ms",	//	"rbTB20ms",
	"ms",	//	"rbTB10ms",
	"ms",	//	"rbTB5ms",
	"ms",	//	"rbTB2ms",
	"ms",	//	"rbTB1ms",
	"ms",	//	"rbTB05ms",
	"ms",	//	"rbTB02ms",
	"ms",	//	"rbTB01ms",
	"us",		//	"rbTB50us",
	"us",		//	"rbTB20us",
	"us",		//	"rbTB10us",
	"us",		//	"rbTB5us",
	"us",		//	"rbTB2us",
	"us",		//	"rbTB1us",
	"us",		//	"rbTB05us",
	"us",		//	"rbTB02us",
	"us",		//	"rbTB01us",
	"ns",		//	"rbTB50ns",
	"ns",		//	"rbTB20ns",
	"ns",		//	"rbTB10ns",
	"ns"		//	"rbTB5ns"
	};

AnsiString	FreqUnitStrings[TB_max] = {
	"N/A",	//	"rbTBCustom",
	"mHz",	//	"rbTB50min",
	"mHz",	//	"rbTB20min",
	"mHz",	//	"rbTB10min",
	"mHz",	//	"rbTB5min",
	"mHz",	//	"rbTB2min",
	"mHz",	//	"rbTB1min",
	"mHz",	//	"rbTB50s",
	"mHz",	//	"rbTB20s",
	"mHz",	//	"rbTB10s",
	"Hz",	//	"rbTB5s",
	"Hz",	//	"rbTB2s",
	"Hz",	//	"rbTB1s",
	"Hz",	//	"rbTB05s",
	"Hz",	//	"rbTB02s",
	"Hz",	//	"rbTB01s",
	"Hz",	//	"rbTB50ms",
	"Hz",	//	"rbTB20ms",
	"Hz",	//	"rbTB10ms",
	"Hz",	//	"rbTB5ms",
	"Hz",	//	"rbTB2ms",
	"Hz",	//	"rbTB1ms",
	"KHz",	//	"rbTB05ms",
	"KHz",	//	"rbTB02ms",
	"KHz",	//	"rbTB01ms",
	"KHz",	//	"rbTB50us",
	"KHz",	//	"rbTB20us",
	"KHz",	//	"rbTB10us",
	"KHz",	//	"rbTB5us",
	"KHz",	//	"rbTB2us",
	"KHz",	//	"rbTB1us",
	"MHz",	//	"rbTB05us",
	"MHz",	//	"rbTB02us",
	"MHz",	//	"rbTB01us",
	"MHz",	//	"rbTB50ns",
	"MHz",	//	"rbTB20ns",
	"MHz",	//	"rbTB10ns",
	"MHz"	//	"rbTB5ns"
	};

float	FreqUnitRatio[TB_max] = {
	1.0,		//	"rbTBCustom",
	0.06,	//	"rbTB50min",
	0.06,	//	"rbTB20min",
	0.06,	//	"rbTB10min",
	0.06,	//	"rbTB5min",
	0.06,	//	"rbTB2min",
	0.06,	//	"rbTB1min",
	0.001,		//	"rbTB50s",
	0.001,		//	"rbTB20s",
	0.001,		//	"rbTB10s",
	1.0,		//	"rbTB5s",
	1.0,		//	"rbTB2s",
	1.0,		//	"rbTB1s",
	1.0,		//	"rbTB05s",
	1.0,		//	"rbTB02s",
	1.0,		//	"rbTB01s",
	0.001,		//	"rbTB50ms",
	0.001,		//	"rbTB20ms",
	0.001,		//	"rbTB10ms",
	0.001,		//	"rbTB5ms",
	0.001,		//	"rbTB2ms",
	0.001,		//	"rbTB1ms",
	1.0,		//	"rbTB05ms",
	1.0,		//	"rbTB02ms",
	1.0,		//	"rbTB01ms",
	0.001,		//	"rbTB50us",
	0.001,		//	"rbTB20us",
	0.001,		//	"rbTB10us",
	0.001,		//	"rbTB5us",
	0.001,		//	"rbTB2us",
	0.001,		//	"rbTB1us",
	1.0,		//	"rbTB05us",
	1.0,		//	"rbTB02us",
	1.0,		//	"rbTB01us",
	1.0,		//	"rbTB50ns",
	1.0,		//	"rbTB20ns",
	1.0,		//	"rbTB10ns",
	1.0		//	"rbTB5ns"
	};


float	TBUnitRate[TB_max] = {
	1.0,		//	"rbTBCustom",
	50.0,	//	"rbTB50min",
	20.0,	//	"rbTB20min",
	10.0,	//	"rbTB10min",
	5.0,		//	"rbTB5min",
	2.0,		//	"rbTB2min",
	1.0,		//	"rbTB1min",
	50.0,	//	"rbTB50s",
	20.0,	//	"rbTB20s",
	10.0,	//	"rbTB10s",
	5.0,		//	"rbTB5s",
	2.0,		//	"rbTB2s",
	1.0,		//	"rbTB1s",
	0.5,		//	"rbTB05s",
	0.2,		//	"rbTB02s",
	0.1,		//	"rbTB01s",
	50.0,	//	"rbTB50ms",
	20.0,	//	"rbTB20ms",
	10.0,	//	"rbTB10ms",
	5.0,		//	"rbTB5ms",
	2.0,		//	"rbTB2ms",
	1.0,		//	"rbTB1ms",
	0.5,		//	"rbTB05ms",
	0.2,		//	"rbTB02ms",
	0.1,		//	"rbTB01ms",
	50.0,	//	"rbTB50us",
	20.0,	//	"rbTB20us",
	10.0,	//	"rbTB10us",
	5.0,		//	"rbTB5us",
	2.0,		//	"rbTB2us",
	1.0,		//	"rbTB1us",
	0.5,		//	"rbTB05us",
	0.2,		//	"rbTB02us",
	0.1,		//	"rbTB01us",
	50.0,	//	"rbTB50ns",
	20.0,	//	"rbTB20ns",
	10.0,	//	"rbTB10ns",
	5.0		//	"rbTB5ns"
	};

/*
TButtonControl	*Ch1VsenButtons[9] = {
	JYEscope->Ch1_5V,
	JYEscope->Ch1_2V,
	JYEscope->Ch1_1V,
	JYEscope->Ch1_05V,
	JYEscope->Ch1_02V,
	JYEscope->Ch1_01V,
	JYEscope->Ch1_50mV,
	JYEscope->Ch1_20mV,
	JYEscope->Ch1_10mV
	
};

TButtonControl	*Ch2VsenButtons[9] = {
	JYEscope->Ch2_5V,
	JYEscope->Ch2_2V,
	JYEscope->Ch2_1V,
	JYEscope->Ch2_05V,
	JYEscope->Ch2_02V,
	JYEscope->Ch2_01V,
	JYEscope->Ch2_50mV,
	JYEscope->Ch2_20mV,
	JYEscope->Ch2_10mV
	
};
*/

//---------------------------------------------------------------------------

__fastcall TJYEscope::TJYEscope(TComponent* Owner)
    : TForm(Owner)
{
 unsigned int tmp0, tmp1, tmp2,  tmp3, tmp4, tmp5;

// AnsiString sPortName;

 this->DoubleBuffered = 1;

// InitTxBuf();
// InitRxBuf();
 CommInit();
 
 serial_port = new Tserial_event();
// serial_port->Parent = this;

 serial_port->setManager(SerialEventManager);

// PortName = this->cbPortList->Items->Strings[this->cbPortList->ItemIndex];

 
}

// ===============================================================
void __fastcall TJYEscope::FormCreate(TObject *Sender)
{
 unsigned int 	tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
 sSIZE	*ptmp, *ptmp1;
 
// -----------------------------------------------------------------------
 // Create Capturer
 if(!(Capturer = new Oscilloscope(2, 25000))) {
 	Application->MessageBox("Create Oscilloscope object failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate Oscilloscope object failed");
 	}
 Capturer->MyParent = this;		// 

 // Allocate memories for waveform buffers. X 3 because data will be expanded 3 times horizontally
 if(!(Capturer->oscCh[0].BufA = new sSIZE[(Capturer->oscRecLen)])) {
 	Application->MessageBox("Channel 1 buffer A allocation failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate oscBuf1a object failed");
 	}
 
 if(!(Capturer->oscCh[1].BufA= new sSIZE[(Capturer->oscRecLen)])) {
 	Application->MessageBox("Channel 2 buffer A allocation failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate oscBuf2a object failed");
 	}

 if(!(Capturer->oscCh[0].BufB = new sSIZE[(Capturer->oscRecLen)])) {
 	Application->MessageBox("Channel 1 buffer B allocation failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate oscBuf1b object failed");
 	}
 
 if(!(Capturer->oscCh[1].BufB = new sSIZE[(Capturer->oscRecLen)])) {
 	Application->MessageBox("Channel 2 buffer B allocation failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate oscBuf2b object failed");
 	}

 // Allocate memories for waveform buffers. X 3 because data will be expanded 3 times horizontally
 if(!(Capturer->oscCh[0].BufDupA = new sSIZE[(Capturer->oscRecLen)])) {
 	Application->MessageBox("Channel 1 buffer A allocation failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate oscBuf1a object failed");
 	}
 
 if(!(Capturer->oscCh[1].BufDupA= new sSIZE[(Capturer->oscRecLen)])) {
 	Application->MessageBox("Channel 2 buffer A allocation failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate oscBuf2a object failed");
 	}

 if(!(Capturer->oscCh[0].BufDupB = new sSIZE[(Capturer->oscRecLen)])) {
 	Application->MessageBox("Channel 1 buffer B allocation failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate oscBuf1b object failed");
 	}
 
 if(!(Capturer->oscCh[1].BufDupB = new sSIZE[(Capturer->oscRecLen)])) {
 	Application->MessageBox("Channel 2 buffer B allocation failed", "Message", MB_OK);
// 	JYEscope->Memo1->Lines->Add("-- Allocate oscBuf2b object failed");
 	}


 BGColor = clBlack;
// BGColor = clSilver;
// BGColor = 0xDCDCDC;
// BGColor = clWhite;
// -------------------------------------------------------------------------------
 // Create Vpos pane
 PaneVpos = new Graphics::TBitmap;
 PaneVpos->Width = PaneVposSizex;
 PaneVpos->Height = PaneVposSizey;
 PaneVpos ->Canvas->Brush->Color = BGColor;
 PaneVpos ->Canvas->FillRect(Rect(0, 0,  PaneVposSizex, PaneVposSizey));

// -------------------------------------------------------------------------------
 // Create Hpos pane
 PaneHpos = new Graphics::TBitmap;
 PaneHpos->Width = PaneHposSizex;
 PaneHpos->Height = PaneHposSizey;
 PaneHpos ->Canvas->Brush->Color = BGColor;
 PaneHpos ->Canvas->FillRect(Rect(0, 0,  PaneHposSizex, PaneHposSizey));
 
 // HPos indicator
 PaneHpos->Canvas->Pen->Color = 0xFFCC33;
// PaneHpos->Canvas->Pen->Color = clMenuHighlight;
 PaneHpos->Canvas->Pen->Width = 2;
 // Draw left bracket
 PaneHpos->Canvas->MoveTo(HPosIndicatorOfsx + 4, HPosIndicatorOfsy);
 PaneHpos->Canvas->LineTo(HPosIndicatorOfsx, HPosIndicatorOfsy); 
 PaneHpos->Canvas->LineTo(HPosIndicatorOfsx, HPosIndicatorOfsy + HPosIndicatorSizey); 
 PaneHpos->Canvas->LineTo(HPosIndicatorOfsx + 4, HPosIndicatorOfsy + HPosIndicatorSizey); 
 // Draw right bracket
 PaneHpos->Canvas->MoveTo(HPosIndicatorOfsx + HPosIndicatorSizex - 4, HPosIndicatorOfsy);
 PaneHpos->Canvas->LineTo(HPosIndicatorOfsx + HPosIndicatorSizex, HPosIndicatorOfsy); 
 PaneHpos->Canvas->LineTo(HPosIndicatorOfsx + HPosIndicatorSizex, HPosIndicatorOfsy + HPosIndicatorSizey); 
 PaneHpos->Canvas->LineTo(HPosIndicatorOfsx  + HPosIndicatorSizex - 4, HPosIndicatorOfsy + HPosIndicatorSizey); 
 // Draw horizontoal line
 PaneHpos->Canvas->MoveTo(HPosIndicatorOfsx + 5, HPosIndicatorOfsy + (HPosIndicatorSizey/2));
 PaneHpos->Canvas->LineTo(HPosIndicatorOfsx + HPosIndicatorSizex - 5, HPosIndicatorOfsy + (HPosIndicatorSizey/2)); 

 // Delta T and Delta V
/*
 PaneHpos->Canvas->Pen->Color = clBluesky;
 PaneHpos->Canvas->Brush->Color = clBluesky;
 PaneHpos ->Canvas->RoundRect(10, Ch1ReadOutOfsy, 
 		Ch1ReadOutOfsx + Ch1ReadOutSizex -1, Ch1ReadOutOfsy + Ch1ReadOutSizey - 1, 6, 6);
*/ 		
// -------------------------------------------------------------------------------
 // Create Readout pane
 PaneReadout = new Graphics::TBitmap;
 PaneReadout->Width = PaneReadoutSizex;
 PaneReadout->Height = PaneReadoutSizey;
 PaneReadout ->Canvas->Brush->Color = BGColor;
 PaneReadout ->Canvas->FillRect(Rect(0, 0,  PaneReadoutSizex, PaneReadoutSizey));

 PaneReadout ->Canvas->Font->Style = TFontStyles()<< fsBold;
 PaneReadout->Canvas->Font->Height = 15;
 // Ch1 vertical displays
 PaneReadout->Canvas->Pen->Color = clYellow;
 PaneReadout->Canvas->Brush->Color = clYellow;
 PaneReadout ->Canvas->RoundRect(Ch1ReadOutOfsx, Ch1ReadOutOfsy, 
 		Ch1ReadOutOfsx + Ch1ReadOutSizex -1, Ch1ReadOutOfsy + Ch1ReadOutSizey - 1, 6, 6);

 // Ch2 vertical displays
 PaneReadout->Canvas->Pen->Color = clAqua;
 PaneReadout->Canvas->Brush->Color = clAqua;
 PaneReadout ->Canvas->RoundRect(Ch2ReadOutOfsx, Ch2ReadOutOfsy, 
 		Ch2ReadOutOfsx + Ch2ReadOutSizex -1, Ch2ReadOutOfsy + Ch2ReadOutSizey - 1, 6, 6);

 // Horizontal displays
 PaneReadout->Canvas->Pen->Color = clLime;
 PaneReadout->Canvas->Brush->Color = clLime;
 PaneReadout ->Canvas->RoundRect(TBReadOutOfsx, TBReadOutOfsy, 
 		TBReadOutOfsx + TBReadOutSizex -1, TBReadOutOfsy + TBReadOutSizey - 1, 6, 6);

 // Trigger displays
// PaneReadout->Canvas->Pen->Color = clFuchsia;
// PaneReadout->Canvas->Brush->Color = clFuchsia;
 PaneReadout->Canvas->Pen->Color = 0xFF99FF;
 PaneReadout->Canvas->Brush->Color = 0xFF99FF;
 PaneReadout ->Canvas->RoundRect(TriggerReadOutOfsx, TriggerReadOutOfsy, 
 		TriggerReadOutOfsx + TriggerReadOutSizex -1, TriggerReadOutOfsy + TriggerReadOutSizey - 1, 6, 6);

// -------------------------------------------------------------------------------
 // Create TrigLvl pane
 PaneTrigLvl = new Graphics::TBitmap;
 PaneTrigLvl->Width = PaneTrigLvlSizex;
 PaneTrigLvl->Height = PaneTrigLvlSizey;
 PaneTrigLvl ->Canvas->Brush->Color = BGColor;
 PaneTrigLvl ->Canvas->FillRect(Rect(0, 0,  PaneTrigLvlSizex, PaneTrigLvlSizey));

// -------------------------------------------------------------------------------
// Create Waveform pane
 PaneWaveform = new TImage(JYEscope);
 PaneWaveform->Parent = JYEscope;
 PaneWaveform->Top = Screeny0 + PaneWaveformOfsy;
 PaneWaveform->Left = Screenx0 + PaneWaveformOfsx;
 PaneWaveform->Width = PaneWaveformSizex + 1;
 PaneWaveform->Height = PaneWaveformSizey + 1;
 PaneWaveform->Picture->Bitmap = Waveform;
// PaneWaveform->Picture->Bitmap->Canvas->Brush->Color = clBlack;
// PaneWaveform->Picture->Bitmap->Canvas->FillRect(Rect(0, 0,  PaneWaveform->Width, PaneWaveform->Height));

// -------------------------------------------------------------------------------
 // VSen display initialization
 JYEscope->lbCh1->Top = Screeny0 + PaneHposSizey + PaneWaveformSizey + Ch1ReadOutOfsy + 5;
 JYEscope->lbCh1->Left = Screenx0 + PaneVposSizex + Ch1ReadOutOfsx + 3;
 JYEscope->lbCh1Sen->Top = JYEscope->lbCh1->Top;
 JYEscope->lbCh1Sen->Left = JYEscope->lbCh1->Left + JYEscope->lbCh1->Width;
 JYEscope->lbCh1CplCurr->Top = JYEscope->lbCh1->Top;
 JYEscope->lbCh1CplCurr->Left = JYEscope->lbCh1Sen->Left + JYEscope->lbCh1Sen->Width;

 JYEscope->lbCh2->Top = JYEscope->lbCh1->Top;
 JYEscope->lbCh2->Left = Screenx0 + PaneVposSizex + Ch2ReadOutOfsx + 3;
 JYEscope->lbCh2Sen->Top = JYEscope->lbCh1->Top;
 JYEscope->lbCh2Sen->Left = JYEscope->lbCh2->Left + JYEscope->lbCh2->Width;
 JYEscope->lbCh2CplCurr->Top = JYEscope->lbCh1->Top;
 JYEscope->lbCh2CplCurr->Left = JYEscope->lbCh2Sen->Left + JYEscope->lbCh2Sen->Width;

// -------------------------------------------------------------------------------
 // Timebase display initialization
 JYEscope->lbTB->Top = JYEscope->lbCh1->Top;
 JYEscope->lbTB->Left = Screenx0 + PaneVposSizex + TBReadOutOfsx + 5;
 JYEscope->lbTimebase->Top = JYEscope->lbCh1->Top;
 JYEscope->lbTimebase->Left = JYEscope->lbTB->Left + JYEscope->lbTB->Width;

// -------------------------------------------------------------------------------
 // Trigger display initialization
 JYEscope->lbTrigModeCurr->Top = JYEscope->lbCh1->Top;
 JYEscope->lbTrigModeCurr->Left = Screenx0 + PaneVposSizex + TriggerReadOutOfsx + 10;
 JYEscope->lbTrigModeCurr->Color = 0xFF99FF;
 
 JYEscope->lbSrc->Top = JYEscope->lbCh1->Top;
 JYEscope->lbSrc->Left = JYEscope->lbTrigModeCurr->Left + JYEscope->lbTrigModeCurr->Width + 30;
 JYEscope->lbSrc->Color = 0xFF99FF;

 JYEscope->lbTrigSrcCurr->Top = JYEscope->lbCh1->Top;
 JYEscope->lbTrigSrcCurr->Left = JYEscope->lbSrc->Left + JYEscope->lbSrc->Width;
 JYEscope->lbTrigSrcCurr->Color = 0xFF99FF;

 JYEscope->lbTL->Top = JYEscope->lbCh1->Top;
 JYEscope->lbTL->Left = JYEscope->lbTrigSrcCurr->Left + JYEscope->lbTrigSrcCurr->Width;
 JYEscope->lbTL->Color = 0xFF99FF;

 JYEscope->edTrigLvlReadout->Top = JYEscope->lbCh1->Top - 2;
 JYEscope->edTrigLvlReadout->Left = JYEscope->lbTL->Left + JYEscope->lbTL->Width;

 JYEscope->lbTrigLvlUnit->Top = JYEscope->lbCh1->Top;
 JYEscope->lbTrigLvlUnit->Left = JYEscope->edTrigLvlReadout->Left + JYEscope->edTrigLvlReadout->Width + 3;
 JYEscope->lbTrigLvlUnit->Color = 0xFF99FF;

// -------------------------------------------------------------------------------
 	 // Ch1 VPos indicator
 	Ch1Pos = new TImage(JYEscope);
 	Ch1Pos->Parent = JYEscope;
 	Ch1Pos->OnMouseDown = JYEscope->Ch1PosMouseDown;
 	Ch1Pos->OnMouseMove = JYEscope->Ch1PosMouseMove;
 	Ch1Pos->OnMouseUp = JYEscope->Ch1PosMouseUp;
 	Ch1Pos->OnDblClick = JYEscope->Ch1PosDblClick;
	//The Top property initialization is not required here since it will be done later anyway
// 	Ch1Pos->Top = Screeny0 + WWindowOfsy + (WWindowSizey/2 - Capturer->GetVPos(Channel_1)) -  7;
 	Ch1Pos->Left = (Screenx0 + WWindowOfsx) - 37;
 	Ch1Pos->Width = 37;        
 	Ch1Pos->Height = 17;      
 	Ch1Pos->Picture->LoadFromFile("Ch1PosIndicator.bmp");
// 	Ch1Pos->Picture->Bitmap->LoadFromResourceName((U16)this, "Ch1PosIndicator"); // LoadFromFile("Ch1PosIndicator.bmp");

 	Ch1Pos->Transparent = True;
 	Ch1Pos->Picture->Bitmap->TransparentMode = tmFixed;
 	Ch1Pos->Picture->Bitmap->TransparentColor = Ch1Pos->Canvas->Pixels[0][0];

 	// Ch1 VPos up button
 	Ch1PosUp = new TImage(JYEscope);
 	Ch1PosUp->Parent = JYEscope;
 	Ch1PosUp->OnMouseDown = JYEscope->Ch1PosUpMouseDown;
 	Ch1PosUp->OnMouseUp = JYEscope->Ch1PosUpMouseUp;
 	Ch1PosUp->Top = Screeny0 + Ch1PosUpOfsy;
 	Ch1PosUp->Left = Screenx0 + Ch1PosUpOfsx;
 	Ch1PosUp->Width = 15;        
 	Ch1PosUp->Height = 15;     
 	Ch1PosUp->Picture->LoadFromFile("Ch1PosUp.bmp");

 	// Ch1 VPos down button
 	Ch1PosDown = new TImage(JYEscope);
 	Ch1PosDown->Parent = JYEscope;
 	Ch1PosDown->OnMouseDown = JYEscope->Ch1PosDownMouseDown;
 	Ch1PosDown->OnMouseUp = JYEscope->Ch1PosDownMouseUp;
 	Ch1PosDown->Top = Screeny0 + Ch1PosDownOfsy;
 	Ch1PosDown->Left = Screenx0 + Ch1PosDownOfsx;
 	Ch1PosDown->Width = 15;        
 	Ch1PosDown->Height = 15;     
 	Ch1PosDown->Picture->LoadFromFile("Ch1PosDown.bmp");

// -------------------------------------------------------------------------------
   	// Ch2 VPos indicator
 	Ch2Pos = new TImage(JYEscope);
 	Ch2Pos->Parent = JYEscope;
 	Ch2Pos->OnMouseDown = JYEscope->Ch2PosMouseDown;
 	Ch2Pos->OnMouseMove = JYEscope->Ch2PosMouseMove;
 	Ch2Pos->OnMouseUp = JYEscope->Ch2PosMouseUp;
 	Ch2Pos->OnDblClick = JYEscope->Ch2PosDblClick;
	//The Top property initialization is not required here since it will be done later anyway
// 	Ch2Pos->Top = Screeny0 + WWindowOfsy + (WWindowSizey/2 - Capturer->GetVPos(Channel_2)) -  7;
 	Ch2Pos->Left = (Screenx0 + WWindowOfsx) - 37;
 	Ch2Pos->Width = 37;        
 	Ch2Pos->Height = 17;      
 	Ch2Pos->Picture->LoadFromFile("Ch2PosIndicator.bmp");
 	Ch2Pos->Transparent = True;
 	Ch2Pos->Picture->Bitmap->TransparentMode = tmFixed;
 	Ch2Pos->Picture->Bitmap->TransparentColor = Ch2Pos->Canvas->Pixels[0][0];
	
 	 // Ch2 VPos up button
 	Ch2PosUp = new TImage(JYEscope);
 	Ch2PosUp->Parent = JYEscope;
 	Ch2PosUp->OnMouseDown = JYEscope->Ch2PosUpMouseDown;
 	Ch2PosUp->OnMouseUp = JYEscope->Ch2PosUpMouseUp;
 	Ch2PosUp->Top = Screeny0 + Ch2PosUpOfsy;
 	Ch2PosUp->Left = Screenx0 + Ch2PosUpOfsx;
 	Ch2PosUp->Width = 15;        
 	Ch2PosUp->Height = 15;
 	Ch2PosUp->Picture->LoadFromFile("Ch2PosUp.bmp");

 	// Ch2 VPos down button
 	Ch2PosDown = new TImage(JYEscope);
 	Ch2PosDown->Parent = JYEscope;
 	Ch2PosDown->OnMouseDown = JYEscope->Ch2PosDownMouseDown;
 	Ch2PosDown->OnMouseUp = JYEscope->Ch2PosDownMouseUp;
	Ch2PosDown->Top = Screeny0 + Ch2PosDownOfsy;
	Ch2PosDown->Left = Screenx0 + Ch2PosDownOfsx;
 	Ch2PosDown->Width = 15;        
 	Ch2PosDown->Height = 15;
 	Ch2PosDown->Picture->LoadFromFile("Ch2PosDown.bmp");

// -------------------------------------------------------------------------------
/*
 // Initialize Trigger level indicator
 TrigLvl = new TImage(JYEscope);
 TrigLvl->Parent = JYEscope;
 TrigLvl->OnMouseDown = JYEscope->TrigLvlMouseDown;
 TrigLvl->OnMouseMove = JYEscope->TrigLvlMouseMove;
 TrigLvl->OnMouseUp = JYEscope->TrigLvlMouseUp;
 TrigLvl->OnDblClick = JYEscope->TrigLvlDblClick; 
 TrigLvl->Top =  (Screeny0 + WWindowOfsy) + 50;
 TrigLvl->Left = (Screenx0 + WWindowOfsx) + WWindowSizex + 1;
 TrigLvl->Width = 28;        
 TrigLvl->Height = 17;     
 TrigLvl->Picture->LoadFromFile("TriggerLevelIndicator.bmp");
// TrigLvlPrev = TrigLvl->Top;
 TrigLvl->Hide();
*/

 TrigLvl_Ch1 = new TImage(JYEscope);
 TrigLvl_Ch1->Parent = JYEscope;
 TrigLvl_Ch1->OnMouseDown = JYEscope->TrigLvlMouseDown;
 TrigLvl_Ch1->OnMouseMove = JYEscope->TrigLvlMouseMove;
 TrigLvl_Ch1->OnMouseUp = JYEscope->TrigLvlMouseUp;
 TrigLvl_Ch1->OnDblClick = JYEscope->TrigLvlDblClick; 
 TrigLvl_Ch1->Top =  (Screeny0 + WWindowOfsy) + 50;
 TrigLvl_Ch1->Left = (Screenx0 + WWindowOfsx) + WWindowSizex + 1;
 TrigLvl_Ch1->Width = 37;        
 TrigLvl_Ch1->Height = 17;     
 TrigLvl_Ch1->Picture->LoadFromFile("TriggerLevelIndicator_Ch1.bmp");
// TrigLvl_Ch1Prev = TrigLvl_Ch1->Top;

 TrigLvl_Ch2 = new TImage(JYEscope);
 TrigLvl_Ch2->Parent = JYEscope;
 TrigLvl_Ch2->OnMouseDown = JYEscope->TrigLvlMouseDown;
 TrigLvl_Ch2->OnMouseMove = JYEscope->TrigLvlMouseMove;
 TrigLvl_Ch2->OnMouseUp = JYEscope->TrigLvlMouseUp;
 TrigLvl_Ch2->OnDblClick = JYEscope->TrigLvlDblClick; 
 TrigLvl_Ch2->Top =  (Screeny0 + WWindowOfsy) + 50;
 TrigLvl_Ch2->Left = (Screenx0 + WWindowOfsx) + WWindowSizex + 1;
 TrigLvl_Ch2->Width = 38;        
 TrigLvl_Ch2->Height = 17;     
 TrigLvl_Ch2->Picture->LoadFromFile("TriggerLevelIndicator_Ch2.bmp");
// TrigLvl_Ch2Prev = TrigLvl_Ch2->Top;
 TrigLvl_Ch2->Hide();

 // Trigger level up button
 TrigLvlUp = new TImage(JYEscope);
 TrigLvlUp->Parent = JYEscope;
 TrigLvlUp->OnMouseDown = JYEscope->TrigLvlUpMouseDown;
 TrigLvlUp->OnMouseUp = JYEscope->TrigLvlUpMouseUp;
 TrigLvlUp->Top = Screeny0 + TrigLvlUpOfsy;
 TrigLvlUp->Left = Screenx0 + TrigLvlUpOfsx;
 TrigLvlUp->Width = 15;        
 TrigLvlUp->Height = 15;     
 TrigLvlUp->Picture->LoadFromFile("TriggerLevelUp.bmp");

 // Trigger level up button
 TrigLvlDown = new TImage(JYEscope);
 TrigLvlDown->Parent = JYEscope;
 TrigLvlDown->OnMouseDown = JYEscope->TrigLvlDownMouseDown;
 TrigLvlDown->OnMouseUp = JYEscope->TrigLvlDownMouseUp;
 TrigLvlDown->Top = Screeny0 + TrigLvlDownOfsy;
 TrigLvlDown->Left = Screenx0 + TrigLvlDownOfsx;
 TrigLvlDown->Width = 15;        
 TrigLvlDown->Height = 15;     
 TrigLvlDown->Picture->LoadFromFile("TriggerLevelDown.bmp");

// -------------------------------------------------------------------------------
 // Pre-load bitmaps
	// Load bitmap from file
 	PosSlopeBmp = new TImage(JYEscope);
	PosSlopeBmp->Parent = JYEscope;
 	PosSlopeBmp->Width = 17;        
 	PosSlopeBmp->Height = 17;      
 	PosSlopeBmp->Transparent = True;
 	PosSlopeBmp->Picture->Bitmap->TransparentMode = tmFixed;
 	PosSlopeBmp->Picture->Bitmap->TransparentColor = PosSlopeBmp->Canvas->Pixels[0][0];
 	PosSlopeBmp->Picture->LoadFromFile("PosSlope.bmp");
	
	// Load bitmap from file
 	NegSlopeBmp = new TImage(JYEscope);
	NegSlopeBmp->Parent = JYEscope;
 	NegSlopeBmp->Width = 17;        
 	NegSlopeBmp->Height = 17;      
 	NegSlopeBmp->Transparent = True;
 	NegSlopeBmp->Picture->Bitmap->TransparentMode = tmFixed;
 	NegSlopeBmp->Picture->Bitmap->TransparentColor = NegSlopeBmp->Canvas->Pixels[0][0];
 	NegSlopeBmp->Picture->LoadFromFile("NegSlope.bmp");
	
 	TrigSlope = new TImage(JYEscope);
 	TrigSlope->Parent = JYEscope;
	// TrigSlope->OnMouseDown = TrigSlopeMouseDown;
	// TrigSlope->OnMouseMove = TrigSlopeMouseMove;
	// TrigSlope->OnMouseUp = TrigSlopeMouseUp;
	TrigSlope->OnClick = TrigSlopeClick;
 	TrigSlope->Top = Screeny0 + TrigSlopeReadOutOfsy + 4;
 	TrigSlope->Left = Screenx0 + TrigSlopeReadOutOfsx + 5;
 	TrigSlope->Width = 17;        
 	TrigSlope->Height = 17;   
 	TrigSlope->Transparent = True;
 	TrigSlope->Picture->Bitmap->TransparentMode = tmFixed;
 	TrigSlope->Picture->Bitmap->TransparentColor = TrigSlope->Canvas->Pixels[0][0];

 	// HPos bar
 	HPosBar = new TImage(JYEscope);
 	HPosBar->Parent = JYEscope;
 	HPosBar->OnMouseDown = JYEscope->HPosBarMouseDown;
 	HPosBar->OnMouseMove = JYEscope->HPosBarMouseMove;
 	HPosBar->OnMouseUp = JYEscope->HPosBarMouseUp;
 	HPosBar->Top = Screeny0 + 20 + 3;
 	HPosBar->Transparent = True;
 	HPosBar->Picture->Bitmap->TransparentMode = tmFixed;

	// Left and right buttons
 	HPosLeftBtn = new TImage(JYEscope);
 	HPosLeftBtn->Parent = JYEscope;
 	HPosLeftBtn->OnMouseDown = JYEscope->HPosLeftMouseDown;
	HPosLeftBtn->OnMouseUp = JYEscope->HPosLeftMouseUp;
 	HPosLeftBtn->Top = Screeny0 + HPosIndicatorOfsy + 2;
 	HPosLeftBtn->Left = Screenx0 + PaneVposSizex + HPosIndicatorOfsx - 18;
 	HPosLeftBtn->Width = 15;        
 	HPosLeftBtn->Height = 15;      
 	HPosLeftBtn->Picture->LoadFromFile("HPosLeftBtn.bmp");
 	HPosLeftBtn->Transparent = True;
 	HPosLeftBtn->Picture->Bitmap->TransparentMode = tmFixed;
 	HPosLeftBtn->Picture->Bitmap->TransparentColor = HPosLeftBtn->Canvas->Pixels[0][0];
	
 	HPosRightBtn = new TImage(JYEscope);
 	HPosRightBtn->Parent = JYEscope;
 	HPosRightBtn->OnMouseDown = JYEscope->HPosRightMouseDown;
	HPosRightBtn->OnMouseUp = JYEscope->HPosRightMouseUp;
 	HPosRightBtn->Top = Screeny0 + HPosIndicatorOfsy + 2;
 	HPosRightBtn->Left = Screenx0 + PaneVposSizex + HPosIndicatorOfsx + HPosIndicatorSizex + 3;
 	HPosRightBtn->Width = 15;        
 	HPosRightBtn->Height = 15;      
 	HPosRightBtn->Picture->LoadFromFile("HPosRightBtn.bmp");
 	HPosRightBtn->Transparent = True;
 	HPosRightBtn->Picture->Bitmap->TransparentMode = tmFixed;
 	HPosRightBtn->Picture->Bitmap->TransparentColor = HPosRightBtn->Canvas->Pixels[0][0];

	// HPos readout
	JYEscope->edHPosReadout->Top = Screeny0 + 3 + 18;
	JYEscope->edHPosReadout->Left = Screenx0 + PaneVposSizex + HPosIndicatorOfsx + HPosIndicatorSizex + 20;

 	HPosBarBitmap = new Graphics::TBitmap;

// -------------------------------------------------------------------------------
// Capturer state indicator
	JYEscope->lbOscState->Top = Screeny0 + HPosIndicatorOfsy + 3;
	JYEscope->lbOscState->Left = JYEscope->edHPosReadout->Left + 85;

// -------------------------------------------------------------------------------
	// dT button
	 dT = new TImage(this);
	 dT->Parent = this;
	 dT->OnClick = dTClick;
//	 dT->OnMouseMove = CursorT1topMouseMove; 
//	 dT->OnMouseUp = CursorT1topMouseUp;
	 dT->Top = Screeny0  + 4 + 4;
	 dT->Left = PaneHposOfsx + 10;
	 dT->Width = 20;        
	 dT->Height = 13;     
	 dT->Picture->LoadFromFile("dT.bmp");
	 dT->Transparent = True;
	 dT->Picture->Bitmap->TransparentMode = tmFixed;
	 dT->Picture->Bitmap->TransparentColor = dT->Canvas->Pixels[0][0];

	// dT readout
	JYEscope->lbdT->Top = dT->Top;
	JYEscope->lbdT->Left = dT->Left + 23;
	
	// dV buttons
	 dV_1 = new TImage(this);
	 dV_1->Parent = this;
	 dV_1->OnClick = dV_1Click;
	 dV_1->Top = Screeny0  + 4 + 20;
	 dV_1->Left = dT->Left;
	 dV_1->Width = 27;        
	 dV_1->Height = 13;     
	 dV_1->Picture->LoadFromFile("dV1_Yellow.bmp");
	 dV_1->Transparent = True;
	 dV_1->Picture->Bitmap->TransparentMode = tmFixed;
	 dV_1->Picture->Bitmap->TransparentColor = dV_1->Canvas->Pixels[0][0];

	 dV_2 = new TImage(this);
	 dV_2->Parent = this;
	 dV_2->OnClick = dV_2Click;
	 dV_2->Top = Screeny0  + 4 + 20;
	 dV_2->Left = dV_1->Left + 90;
	 dV_2->Width = 27;        
	 dV_2->Height = 13;     
	 dV_2->Picture->LoadFromFile("dV2_Aqua.bmp");
	 dV_2->Transparent = True;
	 dV_2->Picture->Bitmap->TransparentMode = tmFixed;
	 dV_2->Picture->Bitmap->TransparentColor = dV_2->Canvas->Pixels[0][0];
// 	dV_2->Hide();

	// dV readout
	JYEscope->lbdV1->Top = dV_1->Top;
	JYEscope->lbdV1->Left = dV_1->Left + 30;
	JYEscope->lbdV2->Top = dV_2->Top;
	JYEscope->lbdV2->Left = dV_2->Left + 30;
	 
	// Cursor T top
	 CursorT1_Top = new TImage(this);
	 CursorT1_Top->Parent = this;
	 CursorT1_Top->OnMouseDown = CursorT1topMouseDown;
	 CursorT1_Top->OnMouseMove = CursorT1topMouseMove; 
	 CursorT1_Top->OnMouseUp = CursorT1topMouseUp;
	 CursorT1_Top->Top = Screeny0 + PaneHposSizey + 1;
	 CursorT1_Top->Left = Screenx0 + 100;
	 CursorT1_Top->Width = 7;        
	 CursorT1_Top->Height = 7;     
	 CursorT1_Top->Picture->LoadFromFile("CursorT_Top.bmp");
	 CursorT1_Top->Transparent = True;
	 CursorT1_Top->Picture->Bitmap->TransparentMode = tmFixed;
	 CursorT1_Top->Picture->Bitmap->TransparentColor = CursorT1_Top->Canvas->Pixels[0][3];

	 CursorT2_Top = new TImage(this);
	 CursorT2_Top->Parent = this;
	 CursorT2_Top->OnMouseDown = CursorT2topMouseDown;
	 CursorT2_Top->OnMouseMove = CursorT2topMouseMove; 
	 CursorT2_Top->OnMouseUp = CursorT2topMouseUp;
	 CursorT2_Top->Top = Screeny0 + PaneHposSizey + 1;
	 CursorT2_Top->Left = Screenx0 + 300;
	 CursorT2_Top->Width = 7;        
	 CursorT2_Top->Height = 7;     
	 CursorT2_Top->Picture->LoadFromFile("CursorT_Top.bmp");
	 CursorT2_Top->Transparent = True;
	 CursorT2_Top->Picture->Bitmap->TransparentMode = tmFixed;
	 CursorT2_Top->Picture->Bitmap->TransparentColor = CursorT2_Top->Canvas->Pixels[0][3];

	// Cursor T bottom
	 CursorT1_Bottom = new TImage(this);
	 CursorT1_Bottom->Parent = this;
	 CursorT1_Bottom->OnMouseDown = CursorT1bottomMouseDown;
	 CursorT1_Bottom->OnMouseMove = CursorT1bottomMouseMove; 
	 CursorT1_Bottom->OnMouseUp = CursorT1bottomMouseUp;
	 CursorT1_Bottom->Top = Screeny0 + PaneHposSizey + PaneWaveformSizey - 7;
	 CursorT1_Bottom->Left = Screenx0 + 100;
	 CursorT1_Bottom->Width = 7;        
	 CursorT1_Bottom->Height = 7;     
	 CursorT1_Bottom->Picture->LoadFromFile("CursorT_Bottom.bmp");
	 CursorT1_Bottom->Transparent = True;
	 CursorT1_Bottom->Picture->Bitmap->TransparentMode = tmFixed;
	 CursorT1_Bottom->Picture->Bitmap->TransparentColor = CursorT1_Bottom->Canvas->Pixels[0][3];

	 CursorT2_Bottom = new TImage(this);
	 CursorT2_Bottom->Parent = this;
	 CursorT2_Bottom->OnMouseDown = CursorT2bottomMouseDown;
	 CursorT2_Bottom->OnMouseMove = CursorT2bottomMouseMove; 
	 CursorT2_Bottom->OnMouseUp = CursorT2bottomMouseUp;
	 CursorT2_Bottom->Top = Screeny0 + PaneHposSizey + PaneWaveformSizey - 7;
	 CursorT2_Bottom->Left = Screenx0 + 300;
	 CursorT2_Bottom->Width = 7;        
	 CursorT2_Bottom->Height = 7;     
	 CursorT2_Bottom->Picture->LoadFromFile("CursorT_Bottom.bmp");
	 CursorT2_Bottom->Transparent = True;
	 CursorT2_Bottom->Picture->Bitmap->TransparentMode = tmFixed;
	 CursorT2_Bottom->Picture->Bitmap->TransparentColor = CursorT2_Bottom->Canvas->Pixels[0][3];

 	CursorT1_Top->Hide();
	CursorT1_Bottom->Hide();

 	CursorT2_Top->Hide();
	CursorT2_Bottom->Hide();

	// Cursor V left
	 Ch1Vcursor1_Left = new TImage(this);
	 Ch1Vcursor1_Left->Parent = this;
	 Ch1Vcursor1_Left->OnMouseDown = Ch1Vcursor1leftMouseDown;
	 Ch1Vcursor1_Left->OnMouseMove = Ch1Vcursor1leftMouseMove; 
	 Ch1Vcursor1_Left->OnMouseUp = Ch1Vcursor1leftMouseUp;
	 Ch1Vcursor1_Left->Top = Screeny0 + PaneHposSizey + 100;
	 Ch1Vcursor1_Left->Left = Screenx0 + PaneVposSizex + 1;
	 Ch1Vcursor1_Left->Width = 7;        
	 Ch1Vcursor1_Left->Height = 7;     
	 Ch1Vcursor1_Left->Picture->LoadFromFile("CursorV_Left_Yellow.bmp");
	 Ch1Vcursor1_Left->Transparent = True;
	 Ch1Vcursor1_Left->Picture->Bitmap->TransparentMode = tmFixed;
	 Ch1Vcursor1_Left->Picture->Bitmap->TransparentColor = Ch1Vcursor1_Left->Canvas->Pixels[3][0];
 	Ch1Vcursor1_Left->Hide();


	 Ch1Vcursor2_Left = new TImage(this);
	 Ch1Vcursor2_Left->Parent = this;
	 Ch1Vcursor2_Left->OnMouseDown = Ch1Vcursor2leftMouseDown;
	 Ch1Vcursor2_Left->OnMouseMove = Ch1Vcursor2leftMouseMove; 
	 Ch1Vcursor2_Left->OnMouseUp = Ch1Vcursor2leftMouseUp;
	 Ch1Vcursor2_Left->Top = Screeny0 + PaneHposSizey + 200;
	 Ch1Vcursor2_Left->Left = Screenx0 + PaneVposSizex + 1;
	 Ch1Vcursor2_Left->Width = 7;        
	 Ch1Vcursor2_Left->Height = 7;     
	 Ch1Vcursor2_Left->Picture->LoadFromFile("CursorV_Left_Yellow.bmp");
	 Ch1Vcursor2_Left->Transparent = True;
	 Ch1Vcursor2_Left->Picture->Bitmap->TransparentMode = tmFixed;
	 Ch1Vcursor2_Left->Picture->Bitmap->TransparentColor = Ch1Vcursor2_Left->Canvas->Pixels[3][0];
 	Ch1Vcursor2_Left->Hide();

	// Cursor V right
	 Ch1Vcursor1_Right = new TImage(this);
	 Ch1Vcursor1_Right->Parent = this;
	 Ch1Vcursor1_Right->OnMouseDown = Ch1Vcursor1rightMouseDown;
	 Ch1Vcursor1_Right->OnMouseMove = Ch1Vcursor1rightMouseMove; 
	 Ch1Vcursor1_Right->OnMouseUp = Ch1Vcursor1rightMouseUp;
	 Ch1Vcursor1_Right->Top = Screeny0 + PaneHposSizey + 100;
	 Ch1Vcursor1_Right->Left = Screenx0 + PaneVposSizex + WWindowSizex - 7;
	 Ch1Vcursor1_Right->Width = 7;        
	 Ch1Vcursor1_Right->Height = 7;     
	 Ch1Vcursor1_Right->Picture->LoadFromFile("CursorV_Right_Yellow.bmp");
	 Ch1Vcursor1_Right->Transparent = True;
	 Ch1Vcursor1_Right->Picture->Bitmap->TransparentMode = tmFixed;
	 Ch1Vcursor1_Right->Picture->Bitmap->TransparentColor = Ch1Vcursor1_Right->Canvas->Pixels[3][0];
 	Ch1Vcursor1_Right->Hide();

	 Ch1Vcursor2_Right = new TImage(this);
	 Ch1Vcursor2_Right->Parent = this;
	 Ch1Vcursor2_Right->OnMouseDown = Ch1Vcursor2rightMouseDown;
	 Ch1Vcursor2_Right->OnMouseMove = Ch1Vcursor2rightMouseMove; 
	 Ch1Vcursor2_Right->OnMouseUp = Ch1Vcursor2rightMouseUp;
	 Ch1Vcursor2_Right->Top = Screeny0 + PaneHposSizey + 200;
	 Ch1Vcursor2_Right->Left = Screenx0 + PaneVposSizex + WWindowSizex - 7;
	 Ch1Vcursor2_Right->Width = 7;        
	 Ch1Vcursor2_Right->Height = 7;     
	 Ch1Vcursor2_Right->Picture ->LoadFromFile("CursorV_Right_Yellow.bmp");
	 Ch1Vcursor2_Right->Transparent = True;
	 Ch1Vcursor2_Right->Picture->Bitmap->TransparentMode = tmFixed;
	 Ch1Vcursor2_Right->Picture->Bitmap->TransparentColor = Ch1Vcursor2_Right->Canvas->Pixels[3][0];
 	Ch1Vcursor2_Right->Hide();

	 Ch2Vcursor1_Left = new TImage(this);
	 Ch2Vcursor1_Left->Parent = this;
	 Ch2Vcursor1_Left->OnMouseDown = Ch2Vcursor1leftMouseDown;
	 Ch2Vcursor1_Left->OnMouseMove = Ch2Vcursor1leftMouseMove; 
	 Ch2Vcursor1_Left->OnMouseUp = Ch2Vcursor1leftMouseUp;
	 Ch2Vcursor1_Left->Top = Screeny0 + PaneHposSizey + 100;
	 Ch2Vcursor1_Left->Left = Screenx0 + PaneVposSizex + 1;
	 Ch2Vcursor1_Left->Width = 7;        
	 Ch2Vcursor1_Left->Height = 7;     
	 Ch2Vcursor1_Left->Picture->LoadFromFile("CursorV_Left_Aqua.bmp");
	 Ch2Vcursor1_Left->Transparent = True;
	 Ch2Vcursor1_Left->Picture->Bitmap->TransparentMode = tmFixed;
	 Ch2Vcursor1_Left->Picture->Bitmap->TransparentColor = Ch2Vcursor1_Left->Canvas->Pixels[3][0];
 	Ch2Vcursor1_Left->Hide();

	 Ch2Vcursor2_Left = new TImage(this);
	 Ch2Vcursor2_Left->Parent = this;
	 Ch2Vcursor2_Left->OnMouseDown = Ch2Vcursor2leftMouseDown;
	 Ch2Vcursor2_Left->OnMouseMove = Ch2Vcursor2leftMouseMove; 
	 Ch2Vcursor2_Left->OnMouseUp = Ch2Vcursor2leftMouseUp;
	 Ch2Vcursor2_Left->Top = Screeny0 + PaneHposSizey + 200;
	 Ch2Vcursor2_Left->Left = Screenx0 + PaneVposSizex + 1;
	 Ch2Vcursor2_Left->Width = 7;        
	 Ch2Vcursor2_Left->Height = 7;     
	 Ch2Vcursor2_Left->Picture->LoadFromFile("CursorV_Left_Aqua.bmp");
	 Ch2Vcursor2_Left->Transparent = True;
	 Ch2Vcursor2_Left->Picture->Bitmap->TransparentMode = tmFixed;
	 Ch2Vcursor2_Left->Picture->Bitmap->TransparentColor = Ch2Vcursor2_Left->Canvas->Pixels[3][0];
 	Ch2Vcursor2_Left->Hide();

	// Cursor V right
	 Ch2Vcursor1_Right = new TImage(this);
	 Ch2Vcursor1_Right->Parent = this;
	 Ch2Vcursor1_Right->OnMouseDown = Ch2Vcursor1rightMouseDown;
	 Ch2Vcursor1_Right->OnMouseMove = Ch2Vcursor1rightMouseMove; 
	 Ch2Vcursor1_Right->OnMouseUp = Ch2Vcursor1rightMouseUp;
	 Ch2Vcursor1_Right->Top = Screeny0 + PaneHposSizey + 100;
	 Ch2Vcursor1_Right->Left = Screenx0 + PaneVposSizex + WWindowSizex - 7;
	 Ch2Vcursor1_Right->Width = 7;        
	 Ch2Vcursor1_Right->Height = 7;     
	 Ch2Vcursor1_Right->Picture->LoadFromFile("CursorV_Right_Aqua.bmp");
	 Ch2Vcursor1_Right->Transparent = True;
	 Ch2Vcursor1_Right->Picture->Bitmap->TransparentMode = tmFixed;
	 Ch2Vcursor1_Right->Picture->Bitmap->TransparentColor = Ch2Vcursor1_Right->Canvas->Pixels[3][0];
 	Ch2Vcursor1_Right->Hide();

	 Ch2Vcursor2_Right = new TImage(this);
	 Ch2Vcursor2_Right->Parent = this;
	 Ch2Vcursor2_Right->OnMouseDown = Ch2Vcursor2rightMouseDown;
	 Ch2Vcursor2_Right->OnMouseMove = Ch2Vcursor2rightMouseMove; 
	 Ch2Vcursor2_Right->OnMouseUp = Ch2Vcursor2rightMouseUp;
	 Ch2Vcursor2_Right->Top = Screeny0 + PaneHposSizey + 200;
	 Ch2Vcursor2_Right->Left = Screenx0 + PaneVposSizex + WWindowSizex - 7;
	 Ch2Vcursor2_Right->Width = 7;        
	 Ch2Vcursor2_Right->Height = 7;     
	 Ch2Vcursor2_Right->Picture->LoadFromFile("CursorV_Right_Aqua.bmp");
	 Ch2Vcursor2_Right->Transparent = True;
	 Ch2Vcursor2_Right->Picture->Bitmap->TransparentMode = tmFixed;
	 Ch2Vcursor2_Right->Picture->Bitmap->TransparentColor = Ch2Vcursor2_Right->Canvas->Pixels[3][0];
 	Ch2Vcursor2_Right->Hide();
	 

// -------------------------------------------------------------------------------
 // Create event for waveform buffer swaping
 WaveformUpdateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	// Automatic reset

 WaveformUploadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	// Automatic reset

 // Create event for connect waiting
 ConnectWaitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);	// Automatic reset
 
 // start thread. The thread will be ended when DispRefresh() is returned.
 ThreadHandle = (HANDLE) _beginthreadex(NULL, 0,
                  			(PBEGINTHREADEX_THREADFUNC) DisplayRefreshThreadStart,
                   			this, 0, &ThreadID_Disp);

// -------------------------------------------------------------------------------
 DeviceConnected = new TImage(this);
 DeviceConnected->Parent = this;
 DeviceConnected->Width = 16;        
 DeviceConnected->Height = 16;      
 DeviceConnected->Picture->LoadFromFile("UnitState_Connected.bmp");

 DeviceDisconnected = new TImage(this);
 DeviceDisconnected->Parent = this;
 DeviceDisconnected->Width = 16;        
 DeviceDisconnected->Height = 16;      
 DeviceDisconnected->Picture->LoadFromFile("UnitState_Disconnected.bmp");
 DeviceConnectionInd->Picture = DeviceDisconnected->Picture;

// -------------------------------------------------------------------------------
 // Trigger position indicator
 	TrigPos = new TImage(JYEscope);
 	TrigPos->Parent = JYEscope;
 	TrigPos->OnMouseDown = JYEscope->TrigPosMouseDown;
 	TrigPos->OnMouseMove = JYEscope->TrigPosMouseMove;
 	TrigPos->OnMouseUp = JYEscope->TrigPosMouseUp;
 	TrigPos->Top = Screeny0 + TrigPosOfsy;
// 	TrigPos->Left = Screenx0 + TrigPosOfsx;
 	TrigPos->Width = 9;        
 	TrigPos->Height = 11;      
 	TrigPos->Picture->LoadFromFile("TriggerPosInd.bmp");
 TrigPos->Transparent = True;
 TrigPos->Picture->Bitmap->TransparentMode = tmFixed;
 TrigPos->Picture->Bitmap->TransparentColor = TrigPos->Canvas->Pixels[0][9];

	
// -------------------------------------------------------------------------------
 // Trig point indicator
 TrigPoint = new TImage(this);
 TrigPoint->Parent = this;
 TrigPoint->OnMouseDown = TrigPointMouseDown;
 TrigPoint->OnMouseUp = TrigPointMouseUp;
 TrigPoint->OnDblClick = JYEscope->TrigPointDblClick; 
 TrigPoint->Top = Screeny0 + TrigPointOfsy;
 TrigPoint->Left = Screenx0 + TrigPointOfsx;
 TrigPoint->Width = 7;        
 TrigPoint->Height = 7;     
 TrigPoint->Picture->LoadFromFile("TrigPointInd.bmp");
 TrigPoint->Transparent = True;
 TrigPoint->Picture->Bitmap->TransparentMode = tmFixed;
 TrigPoint->Picture->Bitmap->TransparentColor = TrigPoint->Canvas->Pixels[0][0];

 TPOutLeft = new TImage(this);
 TPOutLeft->Parent = this;
 TPOutLeft->Top = Screeny0 + TrigPointOfsy;
 TPOutLeft->Left = Screenx0 + WWindowOfsx - 3;
 TPOutLeft->Width = 7;        
 TPOutLeft->Height = 7;     
 TPOutLeft->Picture->LoadFromFile("OutLeft.bmp");
 TPOutLeft->Transparent = True;
 TPOutLeft->Picture->Bitmap->TransparentMode = tmFixed;
 TPOutLeft->Picture->Bitmap->TransparentColor = TPOutLeft->Canvas->Pixels[0][0];
 TPOutLeft->Hide();

 TPOutRight = new TImage(this);
 TPOutRight->Parent = this;
 TPOutRight->Top = Screeny0 + TrigPointOfsy;
 TPOutRight->Left = Screenx0 + WWindowOfsx + WWindowSizex - 3;
 TPOutRight->Width = 7;        
 TPOutRight->Height = 7;     
 TPOutRight->Picture->LoadFromFile("OutRight.bmp");
 TPOutRight->Transparent = True;
 TPOutRight->Picture->Bitmap->TransparentMode = tmFixed;
 TPOutRight->Picture->Bitmap->TransparentColor = TPOutRight->Canvas->Pixels[0][0];
 TPOutRight->Hide();

// -------------------------------------------------------------------------------
 // Draw panes
 JYEscope->Canvas->Draw(Screenx0 + PaneVposOfsx, Screeny0 + PaneVposOfsy, PaneVpos);
 JYEscope->Canvas->Draw(Screenx0 + PaneHposOfsx, Screeny0 + PaneHposOfsy, PaneHpos);
 JYEscope->Canvas->Draw(Screenx0 + PaneReadoutOfsx, Screeny0 + PaneReadoutOfsy, PaneReadout);
 JYEscope->Canvas->Draw(Screenx0 + PaneTrigLvlOfsx, Screeny0 + PaneTrigLvlOfsy, PaneTrigLvl);
// JYEscope->Canvas->Draw(Screenx0 + PaneWaveformOfsx, Screeny0 + PaneWaveformOfsy, PaneWaveform);

// -------------------------------------------------------------------------------
 // Generating test waveforms
 ptmp = Capturer->oscCh[0].BufA;
 tmp0 = 0;
 tmp1 = 1;
 tmp2 = 384;
 while(tmp0 < Capturer->GetRecLen()) {
 	if(tmp1) {
		tmp2 += 2;
		 *ptmp = tmp2;
		 if(tmp2 > (384 + 50)) {
		 	tmp1 = 0;
		 	}
 		}
	else {
		tmp2 -= 2;
		 *ptmp = tmp2;
		 if(tmp2 < (384 - 50)) {
		 	tmp1 = 1;
		 	}
 		}
	ptmp++;
	tmp0++;
 	}
		

 ptmp = Capturer->oscCh[1].BufA;
 tmp0 = 0;
 while(tmp0 < Capturer->GetRecLen()) {
	*ptmp = 60 * sin((3.1415926/50.0) * (float)tmp0) + 384;
	ptmp++;
    	tmp0++;
    	}

  
/*
 ptmp = Capturer->oscCh[0].BufA;
 ptmp1 = Capturer->oscCh[0].BufDupA;
 tmp0 = 0;
 while(tmp0 < Capturer->oscRecLen) {
	*ptmp1++ = *ptmp++;
	tmp0++;
 	}

 // Copy Ch2 data to BufDupA
 ptmp = Capturer->oscCh[1].BufA;
 ptmp1 = Capturer->oscCh[1].BufDupA;
 tmp0 = 0;
 while(tmp0 < Capturer->oscRecLen) {
	*ptmp1++ = *ptmp++;
	tmp0++;
 	}
*/

// -------------------------------------------------------------------------------
 // Read ini file
 TIniFile *inifile = new TIniFile(ExtractFilePath(Application->ExeName) + "jyeLab.ini");

 Capturer->SetTimeBase(inifile->ReadInteger("DSO", "Timebase", TB_1ms));
 Capturer->SetHPos(inifile->ReadInteger("DSO", "Hpos", 0));
 Capturer->SetVSen(Channel_1, inifile->ReadInteger("DSO", "Ch1Vsen", VS_1V));
 Capturer->SetVPos(Channel_1, inifile->ReadInteger("DSO", "Ch1Vpos", 60));
 Capturer->SetVPosOfs(Channel_1, inifile->ReadInteger("DSO", "Ch1VposOfs", 0));
 Capturer->SetRef(Channel_1, inifile->ReadInteger("DSO", "Ch1Ref", 128 * 3));
 Capturer->SetCpl(Channel_1, inifile->ReadInteger("DSO", "Ch1Cpl", CP_DC));
 Capturer->SetVSen(Channel_2, inifile->ReadInteger("DSO", "Ch2Vsen", VS_1V));
 Capturer->SetVPos(Channel_2, inifile->ReadInteger("DSO", "Ch2Vpos", 60));
 Capturer->SetVPosOfs(Channel_2, inifile->ReadInteger("DSO", "Ch2VposOfs", 0));
 Capturer->SetRef(Channel_2, inifile->ReadInteger("DSO", "Ch2Ref", 128 * 3));
 Capturer->SetCpl(Channel_2, inifile->ReadInteger("DSO", "Ch2Cpl", CP_DC));

 Capturer->SetTrigMode(inifile->ReadInteger("DSO", "TrigMode", TM_Auto));
 Capturer->SetTrigSlope(inifile->ReadInteger("DSO", "TrigSlope", TE_Falling));
 Capturer->SetTrigLvl(inifile->ReadInteger("DSO", "TrigLvl", 0));
 Capturer->SetTrigSrc(inifile->ReadInteger("DSO", "TrigSrc", TS_Ch1));
 Capturer->SetTrigSen(inifile->ReadInteger("DSO", "TrigSen", 4));
 Capturer->SetTrigPos(inifile->ReadInteger("DSO", "TrigPos", 25));
 Capturer->SetRecLen(inifile->ReadInteger("DSO", "RecLen", 750));

 Capturer->oscFlags = inifile->ReadInteger("DSO", "Flags", 0);
 Capturer->SetCursorT1(inifile->ReadInteger("DSO", "CursorT1", 100));
 Capturer->SetCursorT2(inifile->ReadInteger("DSO", "CursorT2", 200));
 Capturer->SetCursorV1(Channel_1, inifile->ReadInteger("DSO", "Ch1Vcursor1", -50));
 Capturer->SetCursorV2(Channel_1, inifile->ReadInteger("DSO", "Ch1Vcursor2", 50));
 Capturer->SetCursorV1(Channel_2, inifile->ReadInteger("DSO", "Ch2Vcursor1", -50));
 Capturer->SetCursorV2(Channel_2, inifile->ReadInteger("DSO", "Ch2Vcursor2", 50));

 PortName = inifile->ReadString("COMM", "Port", "COM4");
 serial_port->baudrate = inifile->ReadInteger("COMM", "Baudrate", 38400);
 
 delete inifile;

// -------------------------------------------------------------------------------
 // Copy Ch1 data to BufDupA
 Capturer->oscBufSelector = 1;
 BackupWaveform(Capturer);
// Transform(Capturer, Trans_Copy);

// -------------------------------------------------------------------------------
 // Made Capturer displayed
 OnScreen = Capturer;

 VSenRefresh();
 CplRefresh();
 VPosRefresh();
 TBRefresh();
 HPosRefresh();
 TrigModeRefresh();
 TrigSlopeRefresh();
 TrigSrcRefresh();
 TrigLvlRefresh();
 TrigPosRefresh();
 TrigSenRefresh();
 CursorT_Refresh();
 CursorV_Refresh();
 OscStateRefresh();
 RepaintWave(OnScreen);

}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::FormPaint(TObject *Sender)
{
// JYEscope->Canvas->Draw(Screenx0 + PaneWaveformOfsx, Screeny0 + PaneWaveformOfsy, PaneWaveform);
 JYEscope->Canvas->Draw(Screenx0 + PaneVposOfsx, Screeny0 + PaneVposOfsy, PaneVpos);
 JYEscope->Canvas->Draw(Screenx0 + PaneHposOfsx, Screeny0 + PaneHposOfsy, PaneHpos);
 JYEscope->Canvas->Draw(Screenx0 + PaneReadoutOfsx, Screeny0 + PaneReadoutOfsy, PaneReadout);
 JYEscope->Canvas->Draw(Screenx0 + PaneTrigLvlOfsx, Screeny0 + PaneTrigLvlOfsy, PaneTrigLvl);
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT1topMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousex0 = X;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT1topMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCursorT1;
 tmp0 +=  (X - Mousex0);
 OnScreen->SetCursorT1(tmp0);

 if(BitTest(OnScreen->oscFlags, OF_CursorT_Tied_On)) {
	// Move cursor 2 as weill
	 tmp0 = OnScreen->oscCursorT2;
	 tmp0 +=  (X - Mousex0);
	 OnScreen->SetCursorT2(tmp0);
 	}


 CursorT_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT1topMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }


//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT2topMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousex0 = X;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT2topMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCursorT2;
 tmp0 +=  (X - Mousex0);
 OnScreen->SetCursorT2(tmp0);

 if(BitTest(OnScreen->oscFlags, OF_CursorT_Tied_On)) {
	// Move cursor 1 as weill
	 tmp0 = OnScreen->oscCursorT1;
	 tmp0 +=  (X - Mousex0);
	 OnScreen->SetCursorT1(tmp0);
 	}

 CursorT_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT2topMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }


//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT1bottomMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousex0 = X;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT1bottomMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCursorT1;
 tmp0 +=  (X - Mousex0);
 OnScreen->SetCursorT1(tmp0);

 if(BitTest(OnScreen->oscFlags, OF_CursorT_Tied_On)) {
	// Move cursor 2 as weill
	 tmp0 = OnScreen->oscCursorT2;
	 tmp0 +=  (X - Mousex0);
	 OnScreen->SetCursorT2(tmp0);
 	}

 CursorT_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT1bottomMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }


//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT2bottomMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousex0 = X;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT2bottomMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCursorT2;
 tmp0 +=  (X - Mousex0);
 OnScreen->SetCursorT2(tmp0);

 if(BitTest(OnScreen->oscFlags, OF_CursorT_Tied_On)) {
	// Move cursor 1 as weill
	 tmp0 = OnScreen->oscCursorT1;
	 tmp0 +=  (X - Mousex0);
	 OnScreen->SetCursorT1(tmp0);
 	}
 CursorT_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::CursorT2bottomMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor1leftMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor1leftMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCh1Vcursor1;
 tmp0 +=  (Y - Mousey0);
 OnScreen->SetCursorV1(Channel_1, tmp0);

 CursorV_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor1leftMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor2leftMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor2leftMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCh1Vcursor2;
 tmp0 +=  (Y - Mousey0);
 OnScreen->SetCursorV2(Channel_1, tmp0);
 		

 CursorV_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor2leftMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }


//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor1rightMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor1rightMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCh1Vcursor1;
 tmp0 +=  (Y - Mousey0);
 OnScreen->SetCursorV1(Channel_1, tmp0);

 CursorV_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor1rightMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor2rightMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor2rightMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCh1Vcursor2;
 tmp0 +=  (Y - Mousey0);
 OnScreen->SetCursorV2(Channel_1, tmp0);

 CursorV_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1Vcursor2rightMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }


//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor1leftMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor1leftMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCh2Vcursor1;
 tmp0 +=  (Y - Mousey0);
 OnScreen->SetCursorV1(Channel_2, tmp0);

 CursorV_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor1leftMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor2leftMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor2leftMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCh2Vcursor2;
 tmp0 +=  (Y - Mousey0);
 OnScreen->SetCursorV2(Channel_2, tmp0);
 		

 CursorV_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor2leftMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }


//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor1rightMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor1rightMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCh2Vcursor1;
 tmp0 +=  (Y - Mousey0);
 OnScreen->SetCursorV1(Channel_2, tmp0);

 CursorV_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor1rightMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor2rightMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor2rightMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->oscCh2Vcursor2;
 tmp0 +=  (Y - Mousey0);
 OnScreen->SetCursorV2(Channel_2, tmp0);

 CursorV_Refresh();
 RepaintWave(OnScreen);

 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2Vcursor2rightMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
 }

void __fastcall TJYEscope::dTClick(TObject *Sender)
{
 BitXor(OnScreen->oscFlags, OF_CursorT_On);
 BitClr(OnScreen->oscFlags, OF_CursorT_Tied_On);
 CursorT_Refresh();
 RepaintWave(OnScreen);
}

void __fastcall TJYEscope::lbdTClick(TObject *Sender)
{
 BitXor(OnScreen->oscFlags, OF_CursorT_On);
 BitClr(OnScreen->oscFlags, OF_CursorT_Tied_On);
 CursorT_Refresh();
 RepaintWave(OnScreen);
}

void __fastcall TJYEscope::dV_1Click(TObject *Sender)
{
 BitXor(OnScreen->oscFlags, OF_CursorV1_On);
 CursorV_Refresh();
 RepaintWave(OnScreen);
}

void __fastcall TJYEscope::dV_2Click(TObject *Sender)
{
 BitXor(OnScreen->oscFlags, OF_CursorV2_On);
 CursorV_Refresh();
 RepaintWave(OnScreen);
}

void __fastcall TJYEscope::lbdV1Click(TObject *Sender)
{
 BitXor(OnScreen->oscFlags, OF_CursorV1_On);
 CursorV_Refresh();
 RepaintWave(OnScreen);
 
}

void __fastcall TJYEscope::lbdV2Click(TObject *Sender)
{
 BitXor(OnScreen->oscFlags, OF_CursorV2_On);
 CursorV_Refresh();
 RepaintWave(OnScreen);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigLvlMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;

// TrigLvlLineOn = 1;
 BitSet(OnScreen->oscFlags, OF_TrigLvlLineOn);
 TrigLvlRefresh();
 RepaintWave(OnScreen);
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigLvlMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->GetTrigLvl();
 tmp0 +=  (Mousey0 - Y);
 tmp0 = OnScreen->SetTrigLvl(tmp0);

// OnScreen->TrigLvlRefresh();
// SetEvent(DispEvents[DispEvent_TrigLvl]);		

 TrigLvlRefresh();
 RepaintWave(OnScreen);
// TrigLvlLineOn = 1;
 BitSet(OnScreen->oscFlags, OF_TrigLvlLineOn);

// SendParamToDevice();
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigLvlMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 mousedown = 0;
// TrigLvlLineOn = 0;
 BitClr(OnScreen->oscFlags, OF_TrigLvlLineOn);
 Command(FID_DSOparam);
 TrigLvlRefresh();
 RepaintWave(OnScreen);
// SetEvent(DispEvents[DispEvent_Waveform]);		

 SendParamToDevice();
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigLvlDblClick(TObject *Sender)
{
// if(TrigLvlLineAlwaysOn == 0) {
 if(!BitTest(OnScreen->oscFlags, OF_TrigLvlLineAlwaysOn)) {
// 	TrigLvlLineAlwaysOn = 1;
	BitSet(OnScreen->oscFlags, OF_TrigLvlLineAlwaysOn);
 	}
 else {
// 	TrigLvlLineAlwaysOn = 0;
	BitClr(OnScreen->oscFlags, OF_TrigLvlLineAlwaysOn);
 	}
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigLvlUpMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 tmp0 = OnScreen->GetTrigLvl();
 tmp0 +=  1;
 tmp0 = OnScreen->SetTrigLvl(tmp0);

// TrigLvlLineOn = 1;
 BitSet(OnScreen->oscFlags, OF_TrigLvlLineOn);
 
// OnScreen->TrigLvlRefresh();
// SetEvent(DispEvents[DispEvent_TrigLvl]);		
 Command(FID_DSOparam);
 TrigLvlRefresh();


 SendParamToDevice();
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigLvlUpMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
// TrigLvlLineOn = 0;
 BitClr(OnScreen->oscFlags, OF_TrigLvlLineOn);
 
 TrigLvlRefresh();
 RepaintWave(OnScreen);
// SetEvent(DispEvents[DispEvent_Waveform]);
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigLvlDownMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 
 tmp0 = OnScreen->GetTrigLvl();
 tmp0 -=  1;
 tmp0 = OnScreen->SetTrigLvl(tmp0);

// SetEvent(DispEvents[DispEvent_TrigLvl]);		

// TrigLvlLineOn = 1;
 BitSet(OnScreen->oscFlags, OF_TrigLvlLineOn);
 Command(FID_DSOparam);
 TrigLvlRefresh();

 SendParamToDevice();
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigLvlDownMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
// TrigLvlLineOn = 0;
 BitClr(OnScreen->oscFlags, OF_TrigLvlLineOn);
 TrigLvlRefresh();
 RepaintWave(OnScreen);
// SetEvent(DispEvents[DispEvent_Waveform]);		
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigPointMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
// TrigPointLineOn = 1;
 BitSet(OnScreen->oscFlags, OF_TrigPointLineOn);
 RepaintWave(OnScreen);
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigPointMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
// TrigPointLineOn = 0;
 BitClr(OnScreen->oscFlags, OF_TrigPointLineOn);
 RepaintWave(OnScreen);
// SetEvent(DispEvents[DispEvent_Waveform]);		
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigPointDblClick(TObject *Sender)
{
// if(TrigPointLineAlwaysOn == 0) {
 if(!BitTest(OnScreen->oscFlags, OF_TrigPointLineAlwaysOn)) {
// 	TrigPointLineAlwaysOn = 1;
	BitSet(OnScreen->oscFlags, OF_TrigPointLineAlwaysOn);
 	}
 else {
// 	TrigPointLineAlwaysOn = 0;
	BitClr(OnScreen->oscFlags, OF_TrigPointLineAlwaysOn);
 	}
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1PosUpMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 U16 tmp1;

 tmp0 = OnScreen->GetVPos(Channel_1);
 tmp0 +=  1;
 tmp0 = OnScreen->SetVPos(Channel_1, tmp0);

 Command(FID_DSOparam);
 VPosRefresh();
// SetEvent(DispEvents[DispEvent_Vpos]);		
 RepaintWave(OnScreen);
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1PosUpMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 RepaintWave(OnScreen);
// SetEvent(DispEvents[DispEvent_Waveform]);		
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1PosDownMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 U16 tmp1;

 tmp0 = OnScreen->GetVPos(Channel_1);
 tmp0 -=  1;
 tmp0 = OnScreen->SetVPos(Channel_1, tmp0);

 Command(FID_DSOparam);
 VPosRefresh();
 RepaintWave(OnScreen);
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1PosDownMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 RepaintWave(OnScreen);
// SetEvent(DispEvents[DispEvent_Waveform]);		
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2PosUpMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 U16 tmp1;

 tmp0 = OnScreen->GetVPos(Channel_2);
 tmp0 +=  1;
 tmp0 = OnScreen->SetVPos(Channel_2, tmp0);

 Command(FID_DSOparam);
 VPosRefresh();
 RepaintWave(OnScreen);
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2PosUpMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 RepaintWave(OnScreen);
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2PosDownMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 U16 tmp1;

 tmp0 = OnScreen->GetVPos(Channel_2);
 tmp0 -=  1;
 tmp0 = OnScreen->SetVPos(Channel_2, tmp0);

 Command(FID_DSOparam);
 VPosRefresh();
 RepaintWave(OnScreen);
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2PosDownMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 RepaintWave(OnScreen);
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::HPosBarMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousex0 = X;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::HPosBarMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{

 S32	tmp;
 
 if(!mousedown) {
    return;
    }

// JYEscope->Memo1->Lines->Add(IntToStr(X));
// JYEscope->Memo2->Lines->Add(IntToStr(X - Mousex0));

 tmp = OnScreen->GetHPos();
 tmp += (X - Mousex0);
 OnScreen->SetHPos(tmp);

 HPosRefresh();
 RepaintWave(OnScreen);
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::HPosBarMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Command(FID_DSOparam);
 mousedown = 0;
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::HPosRightMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 S32 tmp0;

 tmp0 = OnScreen->GetHPos();
 tmp0 +=  1;
 tmp0 = OnScreen->SetHPos(tmp0);
  
 Command(FID_DSOparam);
 HPosRefresh();
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::HPosRightMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 RepaintWave(OnScreen);
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::HPosLeftMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 S32 tmp0;

 tmp0 = OnScreen->GetHPos();
 tmp0 -=  1;
 tmp0 = OnScreen->SetHPos(tmp0);

 Command(FID_DSOparam);
 HPosRefresh();
}


//---------------------------------------------------------------------------
void __fastcall TJYEscope::HPosLeftMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 RepaintWave(OnScreen);
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigPosMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousex0 = X;
 mousedown = 1;
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigPosMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp;
 
 if(!mousedown) {
    return;
    }

 tmp = OnScreen->GetTrigPos();
 tmp += ((X - Mousex0) * 100 )/(HPosIndicatorSizex - 10);
 OnScreen->SetTrigPos(tmp);

 Command(FID_DSOparam);
 TrigPosRefresh();
 RepaintWave(OnScreen);;;

}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::TrigPosMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Command(FID_DSOparam);
 mousedown = 0;
 SendParamToDevice();
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1PosMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;

}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1PosMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 U16 tmp1;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->GetVPos(Channel_1);
 tmp0 +=  (Mousey0 - Y);
 tmp0 = OnScreen->SetVPos(Channel_1, tmp0);

 Command(FID_DSOparam);
 VPosRefresh();
 RepaintWave(OnScreen);

}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1PosMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Command(FID_DSOparam);
 mousedown = 0;

}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch1PosDblClick(TObject *Sender)
{
 if(OnScreen->oscState != OS_Running) {
	// Warning for possible incorrect level reading change
	if((Application->MessageBox("This is offline waveform. VPos calibration may cause the amplitude \ndifferent from its correct value. Proceed?", "Warning", MB_YESNO | MB_ICONWARNING)) != IDYES) {
		return;
		}
	else {
 		OnScreen->SetVPosOfs(Channel_1, V0 - OnScreen->oscCh[0].Average);
		OnScreen->oscCh[0].Reference = OnScreen->oscCh[0].Average;
		}
 	}
 else if((Application->MessageBox("Set Ch 1 couple to GND and click OK", "VPos alignment - Ch 1", MB_OK | MB_ICONINFORMATION)) == IDOK) {
 	OnScreen->SetVPosOfs(Channel_1, V0 - OnScreen->oscCh[0].Average);
	OnScreen->oscCh[0].Reference = OnScreen->oscCh[0].Average;
 	}
 VPosRefresh();
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2PosMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Mousey0 = Y;
 mousedown = 1;

}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2PosMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
 S16 tmp0;
 U16 tmp1;
 
 if(!mousedown) {
    return;
    }

 tmp0 = OnScreen->GetVPos(Channel_2);
 tmp0 +=  (Mousey0 - Y);
 tmp0 = OnScreen->SetVPos(Channel_2, tmp0);

// Command(FID_DSOparam);
 VPosRefresh();
 RepaintWave(OnScreen);
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2PosMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Command(FID_DSOparam);
 mousedown = 0;
 }

//---------------------------------------------------------------------------
void __fastcall TJYEscope::Ch2PosDblClick(TObject *Sender)
{
 if(OnScreen->oscState != OS_Running) {
	// Warning for possible incorrect level reading change
	if((Application->MessageBox("This is offline waveform. VPos calibration may cause the amplitude \ndifferent from its correct value. Proceed?", "Warning", MB_YESNO | MB_ICONWARNING)) != IDYES) {
		return;
		}
	else {
 		OnScreen->SetVPosOfs(Channel_2, V0 - OnScreen->oscCh[1].Average);
		OnScreen->oscCh[1].Reference = OnScreen->oscCh[1].Average;
		}
 	}
 else if((Application->MessageBox("Set Ch 2 couple to GND and click OK", "VPos alignment - Ch 2", MB_OK | MB_ICONINFORMATION)) == IDOK) {
 	OnScreen->SetVPosOfs(Channel_2, V0 - OnScreen->oscCh[1].Average);
	OnScreen->oscCh[1].Reference = OnScreen->oscCh[1].Average;
 	}
 VPosRefresh();
}

//---------------------------------------------------------------------------

void __fastcall TJYEscope::TrigSlopeClick(TObject *Sender)
{
 if(OnScreen->oscTrigSlope == TE_Falling) {
 	OnScreen->SetTrigSlope(TE_Rising);
 	}
 else if(OnScreen->oscTrigSlope == TE_Rising) {
 	OnScreen->SetTrigSlope(TE_Falling);
 	}
 TrigSlopeRefresh();
 
 SendParamToDevice();
}

void __fastcall TJYEscope::rbCh1_10mVClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_10mV);
 VSenRefresh();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh1_20mVClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_20mV);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh1_50mVClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_50mV);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh1_01VClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_01V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh1_02VClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_02V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh1_05VClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_05V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh1_1VClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_1V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh1_2VClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_2V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh1_5VClick(TObject *Sender)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_1, VS_5V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

/*
void		TJYEscope::VSenRefresh(Oscilloscope *scope, U16 chn)
{
 if(chn == Channel_1) {
 	if(!Ch1SenBmp) {
		// Setup Ch1 VSen readout background bitmap
 		Ch1SenBmp = new Graphics::TBitmap;
 		Ch1SenBmp->Width = 90;        
 		Ch1SenBmp->Height = 20;      
 		Ch1SenBmp->Transparent = True;
 		Ch1SenBmp->TransparentMode = tmFixed;
 		Ch1SenBmp->TransparentColor = Ch1SenBmp->Canvas->Pixels[0][0];

 		Ch1SenBmp ->Canvas->Brush->Color = clWhite;
 		Ch1SenBmp ->Canvas->Pen->Color = clBlack;
 		Ch1SenBmp ->Canvas->Font->Style = TFontStyles()<< fsBold;
 		}
 	Ch1SenBmp->Canvas->FillRect(Rect(0, 0,  Ch1SenBmp->Width, Ch1SenBmp->Height));
 	Ch1SenBmp->Canvas->TextOut(0, 4, "Ch1: " + VSenString[scope->GetVSen(chn)] );

	if(!Ch1Sen) {
 		Ch1Sen = new TImage(this);
 		Ch1Sen->Parent = this;
		// Ch1Sen->OnMouseDown = Ch1SenMouseDown;
		// Ch1Sen->OnMouseMove = Ch1SenMouseMove;
		// Ch1Sen->OnMouseUp = Ch1SenMouseUp;
 		Ch1Sen->Top = Screeny0 + Ch1ReadOutOfsy + 2;
 		Ch1Sen->Left = Screenx0 + Ch1ReadOutOfsx + 3;
 		Ch1Sen->Width = Ch1SenBmp->Width;        
 		Ch1Sen->Height = Ch1SenBmp->Height;     
 		Ch1Sen->Transparent = True;
 		Ch1Sen->Picture->Bitmap->TransparentMode = tmFixed;
 		Ch1Sen->Picture->Bitmap->TransparentColor = Ch1Sen->Canvas->Pixels[0][0];
		}
 	Ch1Sen->Picture->Bitmap = Ch1SenBmp;
 	}

 if(chn == Channel_2) {
 	if(!Ch2SenBmp) {
		// Setup Ch2 VSen readout background bitmap
 		Ch2SenBmp = new Graphics::TBitmap;
 		Ch2SenBmp->Width = 90;        
 		Ch2SenBmp->Height = 20;      
 		Ch2SenBmp->Transparent = True;
 		Ch2SenBmp->TransparentMode = tmFixed;
 		Ch2SenBmp->TransparentColor = Ch2SenBmp->Canvas->Pixels[0][0];

 		Ch2SenBmp ->Canvas->Brush->Color = clWhite;
 		Ch2SenBmp ->Canvas->Pen->Color = clBlack;
 		Ch2SenBmp ->Canvas->Font->Style = TFontStyles()<< fsBold;
 		}
 	Ch2SenBmp->Canvas->FillRect(Rect(0, 0,  Ch2SenBmp->Width, Ch2SenBmp->Height));
	Ch2SenBmp->Canvas->TextOut(0, 4, "Ch2: " + VSenString[scope->GetVSen(chn)] );

	if(!Ch2Sen) {
 		Ch2Sen = new TImage(this);
 		Ch2Sen->Parent = this;
		// Ch2Sen->OnMouseDown = Ch2SenMouseDown;
		// Ch2Sen->OnMouseMove = Ch2SenMouseMove;
		// Ch2Sen->OnMouseUp = Ch2SenMouseUp;
 		Ch2Sen->Top = Screeny0 + Ch2ReadOutOfsy + 2;
 		Ch2Sen->Left = Screenx0 + Ch2ReadOutOfsx + 3;
 		Ch2Sen->Width = Ch2SenBmp->Width;        
 		Ch2Sen->Height = Ch2SenBmp->Height;     
 		Ch2Sen->Transparent = True;
 		Ch2Sen->Picture->Bitmap->TransparentMode = tmFixed;
 		Ch2Sen->Picture->Bitmap->TransparentColor = Ch2Sen->Canvas->Pixels[0][0];
		}
 	Ch2Sen->Picture->Bitmap = Ch2SenBmp;
 	}

 TrigLvlRefresh(scope);
}
// ------------------------------------------------------------------------

void		TJYEscope::VPosRefresh(Oscilloscope *scope)
{
 if(!Ch1Pos) {
 	 // Ch1 VPos indicator
 	Ch1Pos = new TImage(this);
 	Ch1Pos->Parent = this;
 	Ch1Pos->OnMouseDown = Ch1PosMouseDown;
 	Ch1Pos->OnMouseMove = Ch1PosMouseMove;
 	Ch1Pos->OnMouseUp = Ch1PosMouseUp;
 	Ch1Pos->Top = Screeny0 + WWindowOfsy + (WWindowSizey/2 - jyScope->GetVPos(Channel_1)) -  7;
 	Ch1Pos->Left = (Screenx0 + WWindowOfsx) - 37;
 	Ch1Pos->Width = 37;        
 	Ch1Pos->Height = 17;      
 	Ch1Pos->Picture->LoadFromFile("Ch1PosIndicator.bmp");
 	Ch1Pos->Transparent = True;
 	Ch1Pos->Picture->Bitmap->TransparentMode = tmFixed;
 	Ch1Pos->Picture->Bitmap->TransparentColor = Ch1Pos->Canvas->Pixels[0][0];

 	// Ch1 VPos up button
 	Ch1PosUp = new TImage(this);
 	Ch1PosUp->Parent = this;
 	Ch1PosUp->OnMouseDown = Ch1PosUpMouseDown;
 	Ch1PosUp->OnMouseUp = Ch1PosUpMouseUp;
 	Ch1PosUp->Top = Screeny0 + Ch1PosUpOfsy;
 	Ch1PosUp->Left = Screenx0 + Ch1PosUpOfsx;
 	Ch1PosUp->Width = 15;        
 	Ch1PosUp->Height = 15;     
 	Ch1PosUp->Picture->LoadFromFile("Ch1PosUp.bmp");

 	// Ch1 VPos down button
 	Ch1PosDown = new TImage(this);
 	Ch1PosDown->Parent = this;
 	Ch1PosDown->OnMouseDown = Ch1PosDownMouseDown;
 	Ch1PosDown->OnMouseUp = Ch1PosDownMouseUp;
 	Ch1PosDown->Top = Screeny0 + Ch1PosDownOfsy;
 	Ch1PosDown->Left = Screenx0 + Ch1PosDownOfsx;
 	Ch1PosDown->Width = 15;        
 	Ch1PosDown->Height = 15;     
 	Ch1PosDown->Picture->LoadFromFile("Ch1PosDown.bmp");
 	}

 if(!Ch2Pos) {
   	// Ch2 VPos indicator
 	Ch2Pos = new TImage(this);
 	Ch2Pos->Parent = this;
 	Ch2Pos->OnMouseDown = Ch2PosMouseDown;
 	Ch2Pos->OnMouseMove = Ch2PosMouseMove;
 	Ch2Pos->OnMouseUp = Ch2PosMouseUp;
 	Ch2Pos->Top = Screeny0 + WWindowOfsy + (WWindowSizey/2 - jyScope->GetVPos(Channel_2)) -  7;;
 	Ch2Pos->Left = (Screenx0 + WWindowOfsx) - 37;
 	Ch2Pos->Width = 37;        
 	Ch2Pos->Height = 17;      
 	Ch2Pos->Picture->LoadFromFile("Ch2PosIndicator.bmp");
 	Ch2Pos->Transparent = True;
 	Ch2Pos->Picture->Bitmap->TransparentMode = tmFixed;
 	Ch2Pos->Picture->Bitmap->TransparentColor = Ch2Pos->Canvas->Pixels[0][0];
	
 	 // Ch2 VPos up button
 	Ch2PosUp = new TImage(this);
 	Ch2PosUp->Parent = this;
 	Ch2PosUp->OnMouseDown = Ch2PosUpMouseDown;
 	Ch2PosUp->OnMouseUp = Ch2PosUpMouseUp;
 	Ch2PosUp->Top = Screeny0 + Ch2PosUpOfsy;
 	Ch2PosUp->Left = Screenx0 + Ch2PosUpOfsx;
 	Ch2PosUp->Width = 15;        
 	Ch2PosUp->Height = 15;
 	Ch2PosUp->Picture->LoadFromFile("Ch2PosUp.bmp");

 	// Ch2 VPos down button
 	Ch2PosDown = new TImage(this);
 	Ch2PosDown->Parent = this;
 	Ch2PosDown->OnMouseDown = Ch2PosDownMouseDown;
 	Ch2PosDown->OnMouseUp = Ch2PosDownMouseUp;
	Ch2PosDown->Top = Screeny0 + Ch2PosDownOfsy;
	Ch2PosDown->Left = Screenx0 + Ch2PosDownOfsx;
 	Ch2PosDown->Width = 15;        
 	Ch2PosDown->Height = 15;
 	Ch2PosDown->Picture->LoadFromFile("Ch2PosDown.bmp");
 	}

 Ch1Pos->Update();
 Ch2Pos->Update();

 TrigLvlRefresh(jyScope);
}

void		TJYEscope::CplRefresh(Oscilloscope *scope, U16 chn)
{
 if(chn == Channel_1) {
 	if(!Ch1CplBmp) {
		// Setup Ch1 VCpl readout background bitmap
 		Ch1CplBmp = new Graphics::TBitmap;
 		Ch1CplBmp->Width = 32;        
 		Ch1CplBmp->Height = 20;      
 		Ch1CplBmp->Transparent = True;
 		Ch1CplBmp->TransparentMode = tmFixed;
 		Ch1CplBmp->TransparentColor = Ch1CplBmp->Canvas->Pixels[0][0];

 		Ch1CplBmp ->Canvas->Brush->Color = clWhite;
 		Ch1CplBmp ->Canvas->Pen->Color = clBlack;
 		Ch1CplBmp ->Canvas->Font->Style = TFontStyles()<< fsBold;
 		}
 	Ch1CplBmp->Canvas->FillRect(Rect(0, 0,  Ch1CplBmp->Width, Ch1CplBmp->Height));
 	Ch1CplBmp->Canvas->TextOut(2, 4, CplString[scope->GetCpl(chn)] );

	if(!Ch1Cpl) {
 		Ch1Cpl = new TImage(this);
 		Ch1Cpl->Parent = this;
		// Ch1Cpl->OnMouseDown = Ch1CplMouseDown;
		// Ch1Cpl->OnMouseMove = Ch1CplMouseMove;
		// Ch1Cpl->OnMouseUp = Ch1CplMouseUp;
 		Ch1Cpl->Top = Screeny0 + Ch1ReadOutOfsy + 2;
 		Ch1Cpl->Left = Screenx0 + Ch1ReadOutOfsx + 90 + 12;
 		Ch1Cpl->Width = Ch1CplBmp->Width;        
 		Ch1Cpl->Height = Ch1CplBmp->Height;     
 		Ch1Cpl->Transparent = True;
 		Ch1Cpl->Picture->Bitmap->TransparentMode = tmFixed;
 		Ch1Cpl->Picture->Bitmap->TransparentColor = Ch1Cpl->Canvas->Pixels[0][0];
		}
 	Ch1Cpl->Picture->Bitmap = Ch1CplBmp;
 	}

 if(chn == Channel_2) {
 	if(!Ch2CplBmp) {
		// Setup Ch2 VCpl readout background bitmap
 		Ch2CplBmp = new Graphics::TBitmap;
 		Ch2CplBmp->Width = 32;        
 		Ch2CplBmp->Height = 20;      
 		Ch2CplBmp->Transparent = True;
 		Ch2CplBmp->TransparentMode = tmFixed;
 		Ch2CplBmp->TransparentColor = Ch2CplBmp->Canvas->Pixels[0][0];

 		Ch2CplBmp ->Canvas->Brush->Color = clWhite;
 		Ch2CplBmp ->Canvas->Pen->Color = clBlack;
 		Ch2CplBmp ->Canvas->Font->Style = TFontStyles()<< fsBold;
 		}
 	Ch2CplBmp->Canvas->FillRect(Rect(0, 0,  Ch2CplBmp->Width, Ch2CplBmp->Height));
	Ch2CplBmp->Canvas->TextOut(2, 4, CplString[scope->GetCpl(chn)] );

	if(!Ch2Cpl) {
 		Ch2Cpl = new TImage(this);
 		Ch2Cpl->Parent = this;
		// Ch2Cpl->OnMouseDown = Ch2CplMouseDown;
		// Ch2Cpl->OnMouseMove = Ch2CplMouseMove;
		// Ch2Cpl->OnMouseUp = Ch2CplMouseUp;
 		Ch2Cpl->Top = Screeny0 + Ch2ReadOutOfsy + 2;
 		Ch2Cpl->Left = Screenx0 + Ch2ReadOutOfsx + 90 + 12;
 		Ch2Cpl->Width = Ch2CplBmp->Width;        
 		Ch2Cpl->Height = Ch2CplBmp->Height;     
 		Ch2Cpl->Transparent = True;
 		Ch2Cpl->Picture->Bitmap->TransparentMode = tmFixed;
 		Ch2Cpl->Picture->Bitmap->TransparentColor = Ch2Cpl->Canvas->Pixels[0][0];
		}
 	Ch2Cpl->Picture->Bitmap = Ch2CplBmp;
 	}
}


void		TJYEscope::TBRefresh(Oscilloscope *scope)
{
 if(!TBBmp) {
	// Setup timebase readout background bitmap
 	TBBmp = new Graphics::TBitmap;
 	TBBmp->Width = 90;        
 	TBBmp->Height = 20;      
 	TBBmp->Transparent = True;
 	TBBmp->TransparentMode = tmFixed;
 	TBBmp->TransparentColor = TBBmp->Canvas->Pixels[0][0];

 	TBBmp ->Canvas->Brush->Color = clWhite;
 	TBBmp ->Canvas->Pen->Color = clBlack;
 	TBBmp ->Canvas->Font->Style = TFontStyles()<< fsBold;
 	}
 TBBmp->Canvas->FillRect(Rect(0, 0,  TBBmp->Width, TBBmp->Height));
 TBBmp->Canvas->TextOut(7, 4, "T: " + TBString[scope->GetTimeBase()] );

 if(!TB) {
 	TB = new TImage(this);
 	TB->Parent = this;
	// TB->OnMouseDown = TBMouseDown;
	// TB->OnMouseMove = TBMouseMove;
	// TB->OnMouseUp = TBMouseUp;
 	TB->Top = Screeny0 + TBReadOutOfsy + 2;
 	TB->Left = Screenx0 + TBReadOutOfsx + 5;
 	TB->Width = TBBmp->Width;        
 	TB->Height = TBBmp->Height;     
 	TB->Transparent = True;
 	TB->Picture->Bitmap->TransparentMode = tmFixed;
 	TB->Picture->Bitmap->TransparentColor = TB->Canvas->Pixels[0][0];
	}
 TB->Picture->Bitmap = TBBmp;

}
// ----------------------------------------------------------------------

void		TJYEscope::HPosRefresh(Oscilloscope *scope)
{
 S32 tmp0;
 U32 tmp1;
 U16 tmp2;

 tmp0 = jyScope->GetHPos();
 tmp1 = jyScope->GetRecLen();
 
 if(!HPosBar) {
 	// HPos bar
 	HPosBar = new TImage(this);
 	HPosBar->Parent = this;
 	HPosBar->OnMouseDown = HPosBarMouseDown;
 	HPosBar->OnMouseMove = HPosBarMouseMove;
 	HPosBar->OnMouseUp = HPosBarMouseUp;
 	HPosBar->Top = Screeny0 + 20 + 3;
 	HPosBar->Transparent = True;
 	HPosBar->Picture->Bitmap->TransparentMode = tmFixed;

	// Left and right buttons
 	HPosLeftBtn = new TImage(this);
 	HPosLeftBtn->Parent = this;
 	HPosLeftBtn->OnMouseDown = HPosLeftMouseDown;
	HPosLeftBtn->OnMouseUp = HPosLeftMouseUp;
 	HPosLeftBtn->Top = Screeny0 + HPosIndicatorOfsy + 2;
 	HPosLeftBtn->Left = Screenx0 + HPosIndicatorOfsx - 18;
 	HPosLeftBtn->Width = 15;        
 	HPosLeftBtn->Height = 15;      
 	HPosLeftBtn->Picture->LoadFromFile("HPosLeftBtn.bmp");
 	HPosLeftBtn->Transparent = True;
 	HPosLeftBtn->Picture->Bitmap->TransparentMode = tmFixed;
 	HPosLeftBtn->Picture->Bitmap->TransparentColor = HPosLeftBtn->Canvas->Pixels[0][0];
	
 	HPosRightBtn = new TImage(this);
 	HPosRightBtn->Parent = this;
 	HPosRightBtn->OnMouseDown = HPosRightMouseDown;
	HPosRightBtn->OnMouseUp = HPosRightMouseUp;
 	HPosRightBtn->Top = Screeny0 + HPosIndicatorOfsy + 2;
 	HPosRightBtn->Left = Screenx0 + HPosIndicatorOfsx + HPosIndicatorSizex + 3;
 	HPosRightBtn->Width = 15;        
 	HPosRightBtn->Height = 15;      
 	HPosRightBtn->Picture->LoadFromFile("HPosRightBtn.bmp");
 	HPosRightBtn->Transparent = True;
 	HPosRightBtn->Picture->Bitmap->TransparentMode = tmFixed;
 	HPosRightBtn->Picture->Bitmap->TransparentColor = HPosRightBtn->Canvas->Pixels[0][0];

	edHPosReadout->Top = Screeny0 + HPosIndicatorOfsy;
	edHPosReadout->Left = Screenx0 + HPosIndicatorOfsx + HPosIndicatorSizex + 20;
 	}

 // HPos bar bitmap
 if(!HPosBarBitmap) {
//	delete HPosBarBitmap;
 	HPosBarBitmap = new Graphics::TBitmap;
	}

 	// Calculate currently HPos bar width
 	tmp2 =  (U32)((U32)WWindowSizex * ((U32)HPosIndicatorSizex - 10)) /tmp1;   
 	if(tmp2 != HPosBarWidthPrev) {
  		HPosBarBitmap->Width = tmp2;        
 		HPosBarBitmap->Height = 9;      
 		HPosBarBitmap->Transparent = True;
 		HPosBarBitmap->TransparentMode = tmFixed;
 		HPosBarBitmap ->Canvas->Brush->Color = clBlack;
 		HPosBarBitmap ->Canvas->Pen->Color = clWhite;
 		HPosBarBitmap ->Canvas->RoundRect(0, 0, HPosBarBitmap->Width, HPosBarBitmap->Height, 4, 4);
 		HPosBarBitmap->TransparentColor = HPosBarBitmap->Canvas->Pixels[3][3];
		HPosBarWidthPrev = tmp2;
 		HPosBar->Picture->Bitmap->TransparentColor = HPosBar->Canvas->Pixels[2][2];
 		HPosBar->Width = HPosBarBitmap->Width;        
 		HPosBar->Height = HPosBarBitmap->Height;     
 		}

 if((tmp0 + WWindowSizex) > tmp1) {
 	// HPos too large. Adjust it to fit
 	tmp0 = tmp1 - WWindowSizex;
 	jyScope->SetHPos(tmp0);
 	}
 
 HPosBar->Left = Screenx0 + 50 + 210 + (tmp0 * (U32)(HPosIndicatorSizex - 10))/tmp1;
 HPosBar->Picture->Bitmap = HPosBarBitmap;

 JYEscope->edHPosReadout->Text = IntToStr(jyScope->GetHPos());
 
}
// -----------------------------------------------------------------------------------

void		TJYEscope::TrigModeRefresh(Oscilloscope *scope)
{
 if(!TrigModeBmp) {
	// Setup timebase readout background bitmap
 	TrigModeBmp = new Graphics::TBitmap;
 	TrigModeBmp->Width = 70;        
 	TrigModeBmp->Height = 20;      
 	TrigModeBmp->Transparent = True;
 	TrigModeBmp->TransparentMode = tmFixed;
 	TrigModeBmp->TransparentColor = TrigModeBmp->Canvas->Pixels[0][0];

 	TrigModeBmp ->Canvas->Brush->Color = clWhite;
 	TrigModeBmp ->Canvas->Pen->Color = clBlack;
 	TrigModeBmp ->Canvas->Font->Style = TFontStyles()<< fsBold;
 	}
 TrigModeBmp->Canvas->FillRect(Rect(0, 0,  TrigModeBmp->Width, TrigModeBmp->Height));
 TrigModeBmp->Canvas->TextOut(2, 4, TrigModeString[scope->GetTrigMode()] );

 if(!TrigMode) {
 	TrigMode = new TImage(this);
 	TrigMode->Parent = this;
	// TrigMode->OnMouseDown = TrigModeMouseDown;
	// TrigMode->OnMouseMove = TrigModeMouseMove;
	// TrigMode->OnMouseUp = TrigModeMouseUp;
 	TrigMode->Top = Screeny0 + TrigModeReadOutOfsy + 2;
 	TrigMode->Left = Screenx0 + TrigModeReadOutOfsx + 5;
 	TrigMode->Width = TrigModeBmp->Width;        
 	TrigMode->Height = TrigModeBmp->Height;     
 	TrigMode->Transparent = True;
 	TrigMode->Picture->Bitmap->TransparentMode = tmFixed;
 	TrigMode->Picture->Bitmap->TransparentColor = TrigMode->Canvas->Pixels[0][0];
	}
 TrigMode->Picture->Bitmap = TrigModeBmp;

}

void		TJYEscope::TrigSlopeRefresh(Oscilloscope *scope)
{
 // Pre-load bitmaps
 if(!PosSlopeBmp) {
	// Load bitmap from file
 	PosSlopeBmp = new TImage(this);
	PosSlopeBmp->Parent = this;
 	PosSlopeBmp->Width = 17;        
 	PosSlopeBmp->Height = 17;      
 	PosSlopeBmp->Transparent = True;
 	PosSlopeBmp->Picture->Bitmap->TransparentMode = tmFixed;
 	PosSlopeBmp->Picture->Bitmap->TransparentColor = PosSlopeBmp->Canvas->Pixels[0][0];
 	PosSlopeBmp->Picture->LoadFromFile("PosSlope.bmp");
 	}

 if(!NegSlopeBmp) {
	// Load bitmap from file
 	NegSlopeBmp = new TImage(this);
	NegSlopeBmp->Parent = this;
 	NegSlopeBmp->Width = 17;        
 	NegSlopeBmp->Height = 17;      
 	NegSlopeBmp->Transparent = True;
 	NegSlopeBmp->Picture->Bitmap->TransparentMode = tmFixed;
 	NegSlopeBmp->Picture->Bitmap->TransparentColor = NegSlopeBmp->Canvas->Pixels[0][0];
 	NegSlopeBmp->Picture->LoadFromFile("NegSlope.bmp");
 	}

 if(!TrigSlope) {
 	TrigSlope = new TImage(this);
 	TrigSlope->Parent = this;
	// TrigSlope->OnMouseDown = TrigSlopeMouseDown;
	// TrigSlope->OnMouseMove = TrigSlopeMouseMove;
	// TrigSlope->OnMouseUp = TrigSlopeMouseUp;
 	TrigSlope->Top = Screeny0 + TrigSlopeReadOutOfsy + 4;
 	TrigSlope->Left = Screenx0 + TrigSlopeReadOutOfsx + 5;
 	TrigSlope->Width = 17;        
 	TrigSlope->Height = 17;   
 	TrigSlope->Transparent = True;
 	TrigSlope->Picture->Bitmap->TransparentMode = tmFixed;
 	TrigSlope->Picture->Bitmap->TransparentColor = TrigSlope->Canvas->Pixels[0][0];
	}

 if(scope->GetTrigSlope() == TE_Rising) {
 	TrigSlope->Picture = PosSlopeBmp->Picture;
	return;
 	}
 if(scope->GetTrigSlope() == TE_Falling) {
 	TrigSlope->Picture = NegSlopeBmp->Picture;
	return;
 	}

}

void		TJYEscope::TrigSrcRefresh(Oscilloscope *scope)
{
 if(!TrigSrcBmp) {
	// Setup timebase readout background bitmap
 	TrigSrcBmp = new Graphics::TBitmap;
 	TrigSrcBmp->Width = 60;        
 	TrigSrcBmp->Height = 20;      
 	TrigSrcBmp->Transparent = True;
 	TrigSrcBmp->TransparentMode = tmFixed;
 	TrigSrcBmp->TransparentColor = TrigSrcBmp->Canvas->Pixels[0][0];

 	TrigSrcBmp ->Canvas->Brush->Color = clWhite;
 	TrigSrcBmp ->Canvas->Pen->Color = clBlack;
 	TrigSrcBmp ->Canvas->Font->Style = TFontStyles()<< fsBold;
 	}
 TrigSrcBmp->Canvas->FillRect(Rect(0, 0,  TrigSrcBmp->Width, TrigSrcBmp->Height));
 TrigSrcBmp->Canvas->TextOut(2, 4, "Src: " + TrigSrcString[scope->GetTrigSrc()] );

 if(!TrigSrc) {
 	TrigSrc = new TImage(this);
 	TrigSrc->Parent = this;
	// TrigSrc->OnMouseDown = TrigSrcMouseDown;
	// TrigSrc->OnMouseMove = TrigSrcMouseMove;
	// TrigSrc->OnMouseUp = TrigSrcMouseUp;
 	TrigSrc->Top = Screeny0 + TrigSrcReadOutOfsy + 2;
 	TrigSrc->Left = Screenx0 + TrigSrcReadOutOfsx + 5;
 	TrigSrc->Width = TrigSrcBmp->Width;        
 	TrigSrc->Height = TrigSrcBmp->Height;     
 	TrigSrc->Transparent = True;
 	TrigSrc->Picture->Bitmap->TransparentMode = tmFixed;
 	TrigSrc->Picture->Bitmap->TransparentColor = TrigSrc->Canvas->Pixels[0][0];
	}
 TrigSrc->Picture->Bitmap = TrigSrcBmp;

 TrigLvlRefresh(jyScope);

}

void		TJYEscope::TrigLvlRefresh(Oscilloscope *scope)
{
 U16 tmp0;
 U16 tmp1;
 S16 tmp2, tmp3;
 float tmp4;

 if(!TrigLvl) {
 	// Initialize
 	// Trigger level indicator
 	TrigLvl = new TImage(this);
 	TrigLvl->Parent = this;
 	TrigLvl->OnMouseDown = TrigLvlMouseDown;
 	TrigLvl->OnMouseMove = TrigLvlMouseMove;
 	TrigLvl->OnMouseUp = TrigLvlMouseUp;
 	TrigLvl->Top =  (Screeny0 + WWindowOfsy) + 50;
 	TrigLvl->Left = (Screenx0 + WWindowOfsx) + WWindowSizex + 1;
 	TrigLvl->Width = 28;        
 	TrigLvl->Height = 17;     
 	TrigLvl->Picture->LoadFromFile("TriggerLevelIndicator.bmp");

 	// Trigger level up button
 	TrigLvlUp = new TImage(this);
 	TrigLvlUp->Parent = this;
 	TrigLvlUp->OnMouseDown = TrigLvlUpMouseDown;
 	TrigLvlUp->OnMouseUp = TrigLvlUpMouseUp;
 	TrigLvlUp->Top = Screeny0 + TrigLvlUpOfsy;
 	TrigLvlUp->Left = Screenx0 + TrigLvlUpOfsx;
 	TrigLvlUp->Width = 15;        
 	TrigLvlUp->Height = 15;     
 	TrigLvlUp->Picture->LoadFromFile("TriggerLevelUp.bmp");

 	// Trigger level up button
 	TrigLvlDown = new TImage(this);
 	TrigLvlDown->Parent = this;
 	TrigLvlDown->OnMouseDown = TrigLvlDownMouseDown;
 	TrigLvlDown->OnMouseUp = TrigLvlDownMouseUp;
 	TrigLvlDown->Top = Screeny0 + TrigLvlDownOfsy;
 	TrigLvlDown->Left = Screenx0 + TrigLvlDownOfsx;
 	TrigLvlDown->Width = 15;        
 	TrigLvlDown->Height = 15;     
 	TrigLvlDown->Picture->LoadFromFile("TriggerLevelDown.bmp");

	// Trigger level unit display - mV
 	TrigLvlUnitBmpmV = new Graphics::TBitmap;
 	TrigLvlUnitBmpmV->Width = 20;        
 	TrigLvlUnitBmpmV->Height = 20;      
 	TrigLvlUnitBmpmV->Transparent = True;
 	TrigLvlUnitBmpmV->TransparentMode = tmFixed;
 	TrigLvlUnitBmpmV->TransparentColor = TrigLvlUnitBmpmV->Canvas->Pixels[0][0];
 	TrigLvlUnitBmpmV ->Canvas->Brush->Color = clWhite;
 	TrigLvlUnitBmpmV ->Canvas->Pen->Color = clBlack;
 	TrigLvlUnitBmpmV ->Canvas->Font->Style = TFontStyles()<< fsBold;
 	TrigLvlUnitBmpmV->Canvas->FillRect(Rect(0, 0,  TrigLvlUnitBmpmV->Width, TrigLvlUnitBmpmV->Height));
 	TrigLvlUnitBmpmV ->Canvas->TextOut(0, 4, "mV");

	// Trigger level unit display - V
 	TrigLvlUnitBmpV = new Graphics::TBitmap;
 	TrigLvlUnitBmpV->Width = 20;        
 	TrigLvlUnitBmpV->Height = 20;      
 	TrigLvlUnitBmpV->Transparent = True;
 	TrigLvlUnitBmpV->TransparentMode = tmFixed;
 	TrigLvlUnitBmpV->TransparentColor = TrigLvlUnitBmpV->Canvas->Pixels[0][0];
 	TrigLvlUnitBmpV ->Canvas->Brush->Color = clWhite;
 	TrigLvlUnitBmpV ->Canvas->Pen->Color = clBlack;
 	TrigLvlUnitBmpV ->Canvas->Font->Style = TFontStyles()<< fsBold;
 	TrigLvlUnitBmpV->Canvas->FillRect(Rect(0, 0,  TrigLvlUnitBmpV->Width, TrigLvlUnitBmpV->Height));
 	TrigLvlUnitBmpV ->Canvas->TextOut(0, 4, "V");
	
 	TrigLvlUnit = new TImage(this);
 	TrigLvlUnit->Parent = this;
	// TrigLvlUnit->OnMouseDown = TrigLvlUnitMouseDown;
	// TrigLvlUnit->OnMouseMove = TrigLvlUnitMouseMove;
	// TrigLvlUnit->OnMouseUp = TrigLvlUnitMouseUp;
 	TrigLvlUnit->Top = Screeny0 + TrigLvlReadOutOfsy + 2;
 	TrigLvlUnit->Left = Screenx0 + TrigLvlReadOutOfsx + 70;
 	TrigLvlUnit->Width = 20;        
 	TrigLvlUnit->Height = 16;     
 	TrigLvlUnit->Transparent = True;
 	TrigLvlUnit->Picture->Bitmap->TransparentMode = tmFixed;
 	TrigLvlUnit->Picture->Bitmap->TransparentColor = TrigLvlUnit->Canvas->Pixels[0][0];
	
 	GridBackground->Canvas->Brush->Color = 0xFF99FF;
 	GridBackground->Canvas->Pen->Color = clBlack;
 	GridBackground ->Canvas->Font->Style = TFontStyles()<< fsBold;
 	GridBackground->Canvas->TextOut(TrigLvlReadOutOfsx, TrigLvlReadOutOfsy + 6, "TL: ");

 	this->edTrigLvlReadout->Top = Screeny0 + TrigLvlReadOutOfsy + 3;
 	this->edTrigLvlReadout->Left = Screenx0 + TrigLvlReadOutOfsx + 22;
 	this->edTrigLvlReadout->Height = 17;
 	}

 // Get VSen and VPos based on trigger source
 tmp1 = scope->GetTrigSrc();
 switch(tmp1) {
	case TS_Ch1:
	default:
	 	tmp0 = scope->GetVSen(Channel_1);
		tmp2 = scope->GetVPos(Channel_1);
		break;

	case TS_Ch2:
	 	tmp0 = scope->GetVSen(Channel_2);
		tmp2 = scope->GetVPos(Channel_2);
		break;
 	}

 if(tmp0 <= VS_01V) {
 	TrigLvlUnit->Picture->Bitmap = TrigLvlUnitBmpV;
 	}
 else {
 	TrigLvlUnit->Picture->Bitmap = TrigLvlUnitBmpmV;
 	}

 // Refresh trig level indicator
 tmp3 = scope->GetTrigLvl();
 TrigLvl->Top = Screeny0 + WWindowOfsy + (WWindowSizey/2 - (tmp3 + tmp2)) -  7;
// TrigLvl->Top += (Y - Mousey0);
 if(TrigLvl->Top < (Screeny0 + WWindowOfsy) - 8) {
    TrigLvl->Top = (Screeny0 + WWindowOfsy) - 8;
    }

 if(TrigLvl->Top > ((Screeny0 + WWindowOfsy) + WWindowSizey - 1 - 8)) {
    TrigLvl->Top = (Screeny0 + WWindowOfsy) + WWindowSizey - 1 - 8;
    }

 // Display Trig Level readout
 tmp4 = ((float)(tmp3)/(float)GridSizey) * ((float)VSenRate[tmp0]);
 
 JYEscope->edTrigLvlReadout->Text = (FloatToStrF(tmp4, ffFixed, 3, 1));
 
}
// ---------------------------------------------------------------------


void		TJYEscope::TrigPosRefresh(Oscilloscope *scope)
{
 if(!TrigPos) {
 	// Trigger position indicator
 	TrigPos = new TImage(this);
 	TrigPos->Parent = this;
 	TrigPos->OnMouseDown = TrigPosMouseDown;
 	TrigPos->OnMouseMove = TrigPosMouseMove;
 	TrigPos->OnMouseUp = TrigPosMouseUp;
 	TrigPos->Top = Screeny0 + TrigPosOfsy;
// 	TrigPos->Left = Screenx0 + TrigPosOfsx;
 	TrigPos->Width = 9;        
 	TrigPos->Height = 11;      
 	TrigPos->Picture->LoadFromFile("TriggerPosInd.bmp");
 	}

 TrigPos->Left = Screenx0 + HPosIndicatorOfsx + 5 - 4 + (jyScope->GetTrigPos() * (HPosIndicatorSizex - 10))/100;
 JYEscope->edTrigPos->Text = (IntToStr(scope->GetTrigPos()));
 
// if(TrigPos->Left < (Screenx0 + HPosIndicatorOfsx + 5)) {
//    	TrigPos->Left = Screenx0 + HPosIndicatorOfsx + 5;
//    	}

// if(TrigPos->Left > (Screenx0 + HPosIndicatorOfsx + HPosIndicatorSizex - 5 - TrigPos->Width)) {
//    	TrigPos->Left = (Screenx0 + HPosIndicatorOfsx + HPosIndicatorSizex - 5 - TrigPos->Width);
//    	}
 
}
// ---------------------------------------------------------------


void		TJYEscope::OscStateRefresh(Oscilloscope *scope)
{
 if(!OscStateInd) {
 	 // Oscilloscope state indication
 	OscStateInd = new TImage(this);
 	OscStateInd->Parent = this;
// 	OscStateInd->OnMouseDown = OscStateMouseDown;
//	OscStateInd->OnMouseMove = OscStateMouseMove;
// 	OscStateInd->OnMouseUp = OscStateMouseUp;
 	OscStateInd->Top = Screeny0 + 20;
 	OscStateInd->Left = Screenx0 + 620;
 	OscStateInd->Width = 70;        
 	OscStateInd->Height = 16;      
 	OscStateInd->Transparent = True;
 	OscStateInd->Picture->Bitmap->TransparentMode = tmFixed;
 	OscStateInd->Picture->Bitmap->TransparentColor = OscStateInd->Canvas->Pixels[0][0];

 	OscStateRunning = new TImage(this);
 	OscStateRunning->Parent = this;
	OscStateRunning->Width = 70;        
	OscStateRunning->Height = 16;      
 	OscStateRunning->Picture->LoadFromFile("Running.bmp");
 	OscStateRunning->Transparent = True;
 	OscStateRunning->Picture->Bitmap->TransparentMode = tmFixed;
 	OscStateRunning->Picture->Bitmap->TransparentColor = OscStateRunning->Canvas->Pixels[0][0];

 	OscStateStopped= new TImage(this);
 	OscStateStopped->Parent = this;
	OscStateStopped->Width = 70;        
	OscStateStopped->Height = 16;      
 	OscStateStopped->Picture->LoadFromFile("Stopped.bmp");
 	OscStateStopped->Transparent = True;
 	OscStateStopped->Picture->Bitmap->TransparentMode = tmFixed;
 	OscStateStopped->Picture->Bitmap->TransparentColor = OscStateStopped->Canvas->Pixels[0][0];

 	OscStateWaiting= new TImage(this);
 	OscStateWaiting->Parent = this;
	OscStateWaiting->Width = 70;        
	OscStateWaiting->Height = 16;      
 	OscStateWaiting->Picture->LoadFromFile("Waiting.bmp");
 	OscStateWaiting->Transparent = True;
 	OscStateWaiting->Picture->Bitmap->TransparentMode = tmFixed;
 	OscStateWaiting->Picture->Bitmap->TransparentColor = OscStateWaiting->Canvas->Pixels[0][0];
 	}

 switch(scope->GetOscState()) {
 	case OS_Running:
 		OscStateInd->Picture = OscStateRunning->Picture;
		break;
		
 	case OS_Stopped:
 		OscStateInd->Picture = OscStateStopped->Picture;
		break;
		
 	case OS_Waiting:
	default:
 		OscStateInd->Picture = OscStateWaiting->Picture;
		break;
 	}
  OscStateInd->Update();

}
*/

void __fastcall TJYEscope::rbCh2_10mVClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_10mV);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh2_20mVClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_20mV);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh2_50mVClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_50mV);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh2_01VClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_01V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh2_02VClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_02V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh2_05VClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_05V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh2_1VClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_1V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh2_2VClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_2V);
 VSenRefresh();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbCh2_5VClick(TObject *Sender)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}
 OnScreen->SetVSen(Channel_2, VS_5V);
 VSenRefresh();

}
//---------------------------------------------------------------------------


void __fastcall TJYEscope::cbCh1CplChange(TObject *Sender)
{
 if(!OnScreen->Ch1CplEnabled) {
 	return;
 	}
 if(cbCh1Cpl->ItemIndex != -1) {
    OnScreen->SetCpl(Channel_1, cbCh1Cpl->ItemIndex);
    }
 CplRefresh();
// SetEvent(DispEvents[DispEvenU16]);		
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::cbCh2CplChange(TObject *Sender)
{
 if(!OnScreen->Ch2CplEnabled) {
 	return;
 	}
 if(cbCh2Cpl->ItemIndex != -1) {
    OnScreen->SetCpl(Channel_2, cbCh2Cpl->ItemIndex);
    }
 CplRefresh();
// SetEvent(DispEvents[DispEvenU16]);		
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB02usClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_02us);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB05usClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_05us);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB1usClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_1us);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB2usClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_2us);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB5usClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_5us);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB10usClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_10us);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB20usClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_20us);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB50usClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_50us);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB01msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_01ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB02msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_02ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB05msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_05ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB1msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_1ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB2msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_2ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB5msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_5ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB10msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_10ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB20msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_20ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB50msClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_50ms);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB01sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_01s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB02sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_02s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB05sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_05s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB1sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_1s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB2sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_2s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB5sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_5s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB10sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_10s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB20sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_20s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB50sClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_50s);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB1minClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_1m);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB2minClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_2m);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB5minClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_5m);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbTB10minClick(TObject *Sender)
{
 OnScreen->SetTimeBase(TB_10m);
 TBRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::cbTriggerModeChange(TObject *Sender)
{
 if(cbTriggerMode->ItemIndex != -1) {
    	OnScreen->SetTrigMode(cbTriggerMode->ItemIndex);
 	SendParamToDevice();
	
    }
 TrigModeRefresh();
// SetEvent(DispEvents[DispEvenU16]);		

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::cbTriggerSlopeChange(TObject *Sender)
{
if(cbTriggerSlope->ItemIndex != -1) {
    OnScreen->SetTrigSlope(cbTriggerSlope->ItemIndex);
    }
 TrigSlopeRefresh();
// SetEvent(DispEvents[DispEvenU16]);		
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::cbTriggerSrcChange(TObject *Sender)
{
if(cbTriggerSrc->ItemIndex != -1) {
    OnScreen->SetTrigSrc(cbTriggerSrc->ItemIndex);
    }
 TrigSrcRefresh();
 Command(FID_DSOparam);

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::edTrigPosKeyPress(TObject *Sender, char &Key)
{
 if(Key == 13) {
    	OnScreen->SetTrigPos((edTrigPos->Text).ToInt());
    	TrigPosRefresh();

 	SendParamToDevice();
    	}
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::cbRecordLengthChange(TObject *Sender)
{
 OnScreen->SetRecLen(cbRecordLength->Text.ToInt());
 SendParamToDevice();
 HPosRefresh();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::edTrigLvlReadoutKeyPress(TObject *Sender, char &Key)
{
 if(Key == 13) {
    	OnScreen->SetTrigLvl((edTrigLvlReadout->Text).ToInt());
    	TrigLvlRefresh();

 	SendParamToDevice();
    	}
}

//---------------------------------------------------------------------------
void __fastcall TJYEscope::edHPosReadoutKeyPress(TObject *Sender, char &Key)
{
if(Key == 13) {
   	OnScreen->SetHPos((edHPosReadout->Text).ToInt());

   	HPosRefresh();
  	RepaintWave(OnScreen);
    	}
}

void __fastcall TJYEscope::FormClose(TObject *Sender, TCloseAction &Action)
{
 // Clear up Rx buffer
// InitRxBuf();
 RxQueueInit(&MyComm, TxBuf, TxBufSize);
 Command(FID_Disconnect);
// while(!TxBufEmpty()) {
 while(!TxQueueEmpty(&MyComm)) {
	// Wait for data in Tx buffer to be sent
	}
 // Delay 2 seconds
 WaitForSingleObject(ConnectWaitEvent, 2000);

// serial_port->disconnect();
// delete serial_port;
 
 TIniFile *inifile = new TIniFile(ExtractFilePath(Application->ExeName) + "jyeLab.ini");

 inifile->WriteInteger("DSO", "Timebase", OnScreen->GetTimeBase());
 inifile->WriteInteger("DSO", "Hpos", OnScreen->GetHPos());
 inifile->WriteInteger("DSO", "Ch1Vsen", OnScreen->GetVSen(Channel_1));
 inifile->WriteInteger("DSO", "Ch1Vpos", OnScreen->GetVPos(Channel_1));
 inifile->WriteInteger("DSO", "Ch1VposOfs", OnScreen->GetVPosOfs(Channel_1));
 inifile->WriteInteger("DSO", "Ch1Ref", OnScreen->oscCh[0].Reference);
 inifile->WriteInteger("DSO", "Ch1Cpl", OnScreen->GetCpl(Channel_1));
 inifile->WriteInteger("DSO", "Ch2Vsen", OnScreen->GetVSen(Channel_2));
 inifile->WriteInteger("DSO", "Ch2Vpos", OnScreen->GetVPos(Channel_2));
 inifile->WriteInteger("DSO", "Ch2VposOfs", OnScreen->GetVPosOfs(Channel_2));
 inifile->WriteInteger("DSO", "Ch2Ref", OnScreen->oscCh[1].Reference);
 inifile->WriteInteger("DSO", "Ch2Cpl", OnScreen->GetCpl(Channel_2));


 inifile->WriteInteger("DSO", "TrigMode", OnScreen->GetTrigMode());
 inifile->WriteInteger("DSO", "TrigSlope", OnScreen->GetTrigSlope());
 inifile->WriteInteger("DSO", "TrigLvl", OnScreen->GetTrigLvl());
 inifile->WriteInteger("DSO", "TrigSrc", OnScreen->GetTrigSrc());
 inifile->WriteInteger("DSO", "TrigSen", OnScreen->GetTrigSen());
 inifile->WriteInteger("DSO", "TrigPos", OnScreen->GetTrigPos());

 inifile->WriteInteger("DSO", "Flags", OnScreen->oscFlags);
 inifile->WriteInteger("DSO", "CursorT1", OnScreen->oscCursorT1);
 inifile->WriteInteger("DSO", "CursorT2", OnScreen->oscCursorT2);
 inifile->WriteInteger("DSO", "Ch1Vcursor1", OnScreen->oscCh1Vcursor1);
 inifile->WriteInteger("DSO", "Ch1Vcursor2", OnScreen->oscCh1Vcursor2);
 inifile->WriteInteger("DSO", "Ch2Vcursor1", OnScreen->oscCh2Vcursor1);
 inifile->WriteInteger("DSO", "Ch2Vcursor2", OnScreen->oscCh2Vcursor2);


 inifile->WriteInteger("DSO", "RecLen", OnScreen->GetRecLen());

 inifile->WriteString("COMM", "Port", PortName);
 inifile->WriteInteger("COMM", "Baudrate", serial_port->baudrate);
 
 delete inifile;
    
}

//---------------------------------------------------------------------------



void __fastcall TJYEscope::edTrigSenKeyPress(TObject *Sender, char &Key)
{
 if(Key == 13) {
    	OnScreen->SetTrigSen((edTrigSen->Text).ToInt());
    	TrigSenRefresh();
 	Command(FID_DSOparam);
// 	JYEscope->Memo1->Lines->Append("Trig Sen changed");
    	}
}
//---------------------------------------------------------------------------

/*
void __fastcall TJYEscope::btnPortScanClick(TObject *Sender)
{
 ScanPorts();    
 this->cbPortList->ItemIndex = 0;
 PortName = this->cbPortList->Items->Strings[this->cbPortList->ItemIndex];    
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::btnPortSetupClick(TObject *Sender)
{
// fmPortSetup->lbPortName->Caption = PortName;
 fmPortSetup->Visible = true;
}
//---------------------------------------------------------------------------

*/

void __fastcall TJYEscope::actPortSetupExecute(TObject *Sender)
{
// fmPortSetup->lbPortName->Caption = PortName;
 fmPortSetup->Visible = true;
}
//---------------------------------------------------------------------------


void __fastcall TJYEscope::btnCh1OnOffClick(TObject *Sender)
{
 if(BitTest(OnScreen->oscChnCfg, CF_Ch1)) {
	// Turn Ch1 off
	BitClr(OnScreen->oscChnCfg, CF_Ch1);
	JYEscope->btnCh1OnOff->Caption = "ON";
 	}
 else {
	// Turn Ch1 on
	BitSet(OnScreen->oscChnCfg, CF_Ch1);
	JYEscope->btnCh1OnOff->Caption = "OFF";
 	}
 VSenRefresh();
 CplRefresh();
 VPosRefresh();
 RepaintWave(OnScreen);
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::btnCh2OnOffClick(TObject *Sender)
{
 if(BitTest(OnScreen->oscChnCfg, CF_Ch2)) {
	// Turn Ch2 off
	BitClr(OnScreen->oscChnCfg, CF_Ch2);
	JYEscope->btnCh2OnOff->Caption = "ON";
 	}
 else {
	// Turn Ch2 on
	BitSet(OnScreen->oscChnCfg, CF_Ch2);
	JYEscope->btnCh2OnOff->Caption = "OFF";
 	}
 VSenRefresh();
 CplRefresh();
 VPosRefresh();
 RepaintWave(OnScreen);
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::lbCh1SenMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(!OnScreen->Ch1SenEnabled) {
 	return;
 	}
 	
 if(Button == mbLeft) {
	OnScreen->SetVSen(Channel_1, OnScreen->oscCh[0].Sen + 1);
 	}
 if(Button == mbRight) {
	OnScreen->SetVSen(Channel_1, OnScreen->oscCh[0].Sen - 1);
 	}
 VSenRefresh();
 Command(FID_DSOparam);
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::lbCh2SenMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(!OnScreen->Ch2SenEnabled) {
 	return;
 	}

 if(Button == mbLeft) {
	OnScreen->SetVSen(Channel_2, OnScreen->oscCh[1].Sen + 1);
 	}
 if(Button == mbRight) {
	OnScreen->SetVSen(Channel_2, OnScreen->oscCh[1].Sen - 1);
 	}
 VSenRefresh();
 Command(FID_DSOparam);
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::lbTimebaseMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button == mbLeft) {
	OnScreen->SetTimeBase(OnScreen->oscTimebase + 1);
 	}
 if(Button == mbRight) {
	OnScreen->SetTimeBase(OnScreen->oscTimebase - 1);
 	}
 TBRefresh();

 SendParamToDevice();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::lbTrigModeCurrMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(Button == mbLeft) {
	OnScreen->SetTrigMode(OnScreen->oscTrigMode + 1);
 	}
 if(Button == mbRight) {
	OnScreen->SetTrigMode(OnScreen->oscTrigMode - 1);
 	}
 TrigModeRefresh();

 SendParamToDevice();
}
//---------------------------------------------------------------------------


void __fastcall TJYEscope::lbTrigSrcCurrMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 if(!OnScreen->TrigSrcEnabled) {
 	return;
 	}

 if(Button == mbLeft) {
	OnScreen->SetTrigSrc(OnScreen->oscTrigSrc + 1);
 	}
 if(Button == mbRight) {
	OnScreen->SetTrigSrc(OnScreen->oscTrigSrc - 1);
 	}
 TrigSrcRefresh();
 Command(FID_DSOparam);
}
//---------------------------------------------------------------------------


void __fastcall TJYEscope::HelpAboutExecute(TObject *Sender)
{
 AboutBox->ShowModal();   
}
//---------------------------------------------------------------------------


void __fastcall TJYEscope::FileOpen1Accept(TObject *Sender)
{
 U8	tmp0;

 // Check for available room
 tmp0 = 0;
 while(tmp0 < 4) {
 	if(Recall[tmp0] == NULL) {
		break;
 		}
	tmp0++;
 	}

 if(tmp0 > 3) {
 	// No room
 	Application->MessageBox("No room for new recall. Close one or more opened recalls and try again.", "Message", MB_OK);
	return;
 	}

 // tmp0 is the index of available room
 Recall[tmp0] = new JRecall();
 
// Memo1->Lines->LoadFromFile (FileOpen1->Dialog->FileName);
 FileName = FileOpen1->Dialog->FileName;

 if((Recall[tmp0]->Recall = RecallWaveFromFile(FileName))) {
 	// File reading good
 	// Assign button
	switch(tmp0) {
		case 0:
			Recall[0]->Btn = rbRecall0;
			break;
			
		case 1:
			Recall[1]->Btn = rbRecall1;
			break;
			
		case 2:
			Recall[2]->Btn = rbRecall2;
			break;
			
		case 3:
			Recall[3]->Btn = rbRecall3;
		default:			
			break;
		}
	
	Recall[tmp0]->Btn->Caption = ExtractFileName(FileName);
	Recall[tmp0]->Btn->Enabled = true;

	// 
	Recall[tmp0]->Recall->Caption = Recall[tmp0]->Btn->Caption;
	Recall[tmp0]->Recall->oscState = OS_Recall;
	Recall[tmp0]->Recall->RecallIndex = tmp0;
	CurrRecallIndex = tmp0;

	// Enable file close action
	JYEscope->FileClose->Enabled = true;

	// Switch to the recall 
	OnScreen = Recall[tmp0]->Recall;
	
	// This will make the waveform drawn
	Recall[tmp0]->Btn->Checked = true;
	
	 Capturer->oscState = OS_Stopped;
	// JYEscope->btnRunStop->Enabled = false;
	 
	 // Enable file close action
	 JYEscope->FileClose->Enabled = true;
	 JYEscope->btnDispCapture->Enabled = true;
	 JYEscope->CaptureDisplay->Enabled = true;
	 	
	 if(ConnectState) {
		 if(FrontEndType == 'O') {
		 	JYEscope->CaptureRun->Enabled = true;
		 	}
		 else {
		 	JYEscope->CaptureRun->Enabled = false;
		 	}
		JYEscope->CaptureStop->Enabled = false;
	 	}
	 else {
		JYEscope->CaptureRun->Enabled = false;
		JYEscope->CaptureStop->Enabled = false;
	 	}

	 RepaintScreen();
	// Report progress
//      JYEscope->Memo1->Lines->Append(FileName + AnsiString(" open successful"));
	}
 else {
 	JYEscope->Memo1->Lines->Append(AnsiString("Load waveform failed"));
// 	Application->MessageBox("Load waveform failed", "Message", MB_OK);
	 delete Recall[tmp0];
 	}


}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::FileSaveExecute(TObject *Sender)
{
 FILE *file;
 
 if (FileName == "Untitled.txt") {
      FileSaveAs1->Execute();
      }
 else {
 	SaveWaveToFile(FileName);
      }
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::FileSaveAs1BeforeExecute(TObject *Sender)
{
FileSaveAs1->Dialog->InitialDir = ExtractFilePath (FileName);
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::FileSaveAs1Accept(TObject *Sender)
{
 SaveWaveToFile(FileSaveAs1->Dialog->FileName);
 FileName = FileSaveAs1->Dialog->FileName;
 
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::FileCloseExecute(TObject *Sender)
{
 // Report progress
 JYEscope->Memo1->Lines->Append(Recall[OnScreen->RecallIndex]->Recall->Caption  + AnsiString(" closed"));
 
 // Delete the Oscilloscope object and release associated button pointed by current Recall structure 
 delete Recall[OnScreen->RecallIndex];
 Recall[OnScreen->RecallIndex] = NULL;	// Made this location available
 
 // Disable file close action because this locaton now is empty
 JYEscope->FileClose->Enabled = false;

 // Switch to capture display
 OnScreen = Capturer;
 Capturer->oscState = OS_Stopped;
 if(ConnectState == 1) {
	 if(FrontEndType == 'O') {
	 	JYEscope->btnRunStop->Enabled = true;
	 	JYEscope->CaptureRun->Enabled = true;
	 	}
	 else {
	 	JYEscope->btnRunStop->Enabled = false;
	 	JYEscope->CaptureRun->Enabled = false;
	 	}
 	}
 JYEscope->btnDispCapture->Enabled = false;
 RepaintScreen();
 

}
//---------------------------------------------------------------------------


void __fastcall TJYEscope::btnSaveClick(TObject *Sender)
{
// serial_port->sendData(JYEscope->Edit1->Text.c_str(), JYEscope->Edit1->Text.Length());
// TxProc(JYEscope->Edit1->Text.c_str(), JYEscope->Edit1->Text.Length());
// Command(DsoCmd_EnterVdso);
}

//---------------------------------------------------------------------------

void __fastcall TJYEscope::btnRecallClick(TObject *Sender)
{
 FileOpen1->Execute();
// HPosBar->Picture->Bitmap->TransparentColor = HPosBar->Canvas->Pixels[2][2];
// SetEvent(DispEvents[DispEvent_Waveform]);		
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::btnRunStopClick(TObject *Sender)
{
 U16 tmp;

 switch(Capturer->GetOscState()) {
	case OS_Running:
		Capturer->SetOscState(OS_Stopped);
		JYEscope->CaptureRun->Enabled = true;
		JYEscope->CaptureStop->Enabled = false;
		// Save original capture parameters
		BackupWaveform(Capturer);
		break;
	
	case OS_Stopped:
	case OS_Recall:
		if(Recall[CurrRecallIndex] != NULL) {
			// Recall is currently in display. Uncheck the button
			Recall[CurrRecallIndex]->Btn->Checked = false;			
			}
		JYEscope->FileClose->Enabled = false;
		JYEscope->btnDispCapture->Enabled = false;
 		JYEscope->CaptureDisplay->Enabled = false;
		JYEscope->CaptureRun->Enabled = false;
		JYEscope->CaptureStop->Enabled = true;
 		Capturer->SetOscState(OS_Running);
		OnScreen = Capturer;
		RepaintScreen();
		break;

	case OS_Offline:
              JYEscope->Memo1->Lines->Append(AnsiString("\nPlease connect device first"));
		break;
		
	default:
		break;
		 	}
 
 OscStateRefresh();
}
//---------------------------------------------------------------------------


void __fastcall TJYEscope::btnConnectClick(TObject *Sender)
{
 U32		tmp0;
 
 if(ConnectState == 0) {
 	if(fmPortSetup->cbPortList->ItemIndex == -1) {
              JYEscope->Memo1->Lines->Append(AnsiString("\nPlease select a port"));
		return;
 		}
    	 // Open port
    	JYEscope->Memo1->Lines->Append(AnsiString("\n--- Opening " + PortName + " with baudrate " + IntToStr(serial_port->baudrate) + " bps"));
//	serial_port->baudrate = 38400;
    	if(serial_port->connect(PortName.c_str(), false) != 0) {
		// Connection failed
 		JYEscope->Memo1->Lines->Append(PortName + " open FAILED !");
		return;
		}
  	JYEscope->Memo1->Lines->Append(PortName + " open successful");
	// Connect DSO
	Command(FID_Connect);
	JYEscope->Memo1->Lines->Append(AnsiString("Connecting ..."));
	tmp0 = WaitForSingleObject(ConnectWaitEvent, 2000);
	if(tmp0 == WAIT_OBJECT_0) {
		// Connect successful
		ConnectSuccessful();
		}
	else {
//--------------------------		
		// Try one more time
		serial_port->disconnect();

		// Delete existing serial port object and re-create it
		delete serial_port;
		 serial_port = new Tserial_event();
		 serial_port->setManager(SerialEventManager);

		// Re-connect
    		if(serial_port->connect(PortName.c_str(), false) != 0) {
			// Connection failed
	 		JYEscope->Memo1->Lines->Append(PortName + " open FAILED !");
			return;
			}
			
//	  	JYEscope->Memo1->Lines->Append(PortName + " open successful");
		// Connect DSO
		Command(FID_Connect);
		JYEscope->Memo1->Lines->Append(AnsiString("Re-trying ..."));
		tmp0 = WaitForSingleObject(ConnectWaitEvent, 2000);
		if(tmp0 == WAIT_OBJECT_0) {
			// Connect successful
			ConnectSuccessful();
			}
		else {
// ---------------------------------			
	 		JYEscope->Memo1->Lines->Append("*** No device detected ! ***");
			serial_port->disconnect();
	 		JYEscope->Memo1->Lines->Append(PortName + " closed");
			}
		}
 	}
 else {
	Command(FID_Disconnect);
	while(!TxQueueEmpty(&MyComm)) {
		// Wait for data in Tx buffer to be sent
		}
	ConnectFailed();
	serial_port->disconnect();

/*	
    	ConnectState = 0;
    	UnitState->Caption = "Disconnected";
    	btnConnect->Caption = "Connect";
    	DeviceConnectionInd->Picture = DeviceDisconnected->Picture;
	JYEscope->UnitName->Caption = "N/A";
	JYEscope->btnRunStop->Enabled = false;
//	JYEscope->btnDispCapture->Enabled = false;
//	JYEscope->FileClose->Enabled = false;
	Capturer->SetOscState(OS_Offline);
	OscStateRefresh();
	// Transform waveform to Duplicated buffer because the duplicated buffer is displayed when capture is not in running state.
	BackupWaveform(Capturer);
*/	
    	}

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::btnSettingsClick(TObject *Sender)
{
 TrigPoint->Left += 10;    
 
// std::auto_ptr<TPageControl> pgSettingPort(new TPageControl(this));
// pgSettingPort->Parent = this;
// pgSettingPort->Parent = fmSettings;
// pgSettingPort->Align = alClient;
// pgSettingPort->Caption = "Port ";

 std::auto_ptr<TForm> fmSettings(new TForm(this));

 fmSettings->Caption = "Info";
 fmSettings->Left = 1400;
 fmSettings->Top = 600;


// std::auto_ptr<TTabSheet> tsTab1(new TTabSheet(pgSettingPort));
// tsTab1->PageControl = pgSettingPortl;
// tsTab1->Caption = "Port";

 /*
 std::auto_ptr<TPageControl> pgSettingScope(new TPageControl(this));
 pgSettingPort->Parent = fmSettings;
 pgSettingPort->Align = alClient;
 pgSettingPort->Caption = "Scope ";
*/

 fmSettings->ShowModal();

}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::btnDispCaptureClick(TObject *Sender)
{
 Recall[OnScreen->RecallIndex]->Btn->Checked = false;
 OnScreen = Capturer;
 Capturer->oscState = OS_Stopped;
 if(ConnectState == 1) {
	 if(FrontEndType == 'O') {
	 	JYEscope->btnRunStop->Enabled = true;
	 	JYEscope->CaptureRun->Enabled = true;
	 	}
	 else {
	 	JYEscope->btnRunStop->Enabled = false;
	 	JYEscope->CaptureRun->Enabled = false;
	 	}
 	}
 // Disable file close action because the currently displayed is capture
 JYEscope->FileClose->Enabled = false;
 JYEscope->btnDispCapture->Enabled = false;
 JYEscope->CaptureDisplay->Enabled = false;
 RepaintScreen();
}
//---------------------------------------------------------------------------


void __fastcall TJYEscope::btnSelectBufferOKClick(TObject *Sender)
{
 U8 	tmp0;
 U8 	*buf, buf0[10];
 long	Request;
 
 frUploadWaveform->Close();
 JYEscope->Memo1->Lines->Append("Buffer " + IntToStr(SelectedBuffer) + " Selected");

 Capturer->oscState = OS_Recall;
 if(ConnectState == 1) {
	 if(FrontEndType == 'O') {
	 	JYEscope->btnRunStop->Enabled = true;
	 	JYEscope->CaptureRun->Enabled = true;
	 	}
	 else {
	 	JYEscope->btnRunStop->Enabled = false;
	 	JYEscope->CaptureRun->Enabled = false;
	 	}
 	}
 // Disable file close action because the currently displayed is capture
 JYEscope->FileClose->Enabled = false;
 JYEscope->btnDispCapture->Enabled = false;
 JYEscope->CaptureDisplay->Enabled = false;

 // Send ReadWaveform request
 buf = buf0;
 *buf++ = FID_ReadWaveform;
 *(U16 *)buf = 6;	// Frame size
 buf += 2;
 *buf = SelectedBuffer;
 
 JYEscope->Memo1->Lines->Append("Sending upload request ...");
 // Send 
 TxProc(buf0, *((U16 *)(buf0 + 1)));    

 // Waiting for waveform data
 Request = WaitForSingleObject(WaveformUploadEvent, 5000);
 if(Request == WAIT_OBJECT_0) {
 	// Waveform received. 
 	
	 // Copy data to Dup buffer for possible transformation
	 // First set BufA (which contains the newly loaded data) to be the "back" buffer.
	 //	This is because transformations onle be done to inactive buffer. The active buffer is for capture.
	 //	Data from file are always loaded to BufA
	Capturer->oscBufSelector = 1;

	// Copy data to Dup buffer
	Transform(Capturer, Trans_Copy);

	
	Capturer->Caption = "Upload [ " + IntToStr(SelectedBuffer) + " ]";
	Capturer->oscState = OS_Recall;
	
 	JYEscope->Memo1->Lines->Append("Waveform upload successful");
	
	// Switch to the recall 
	OnScreen = Capturer;
	
//	Capturer->oscState = OS_Stopped;
	// JYEscope->btnRunStop->Enabled = false;
	 
//	JYEscope->btnDispCapture->Enabled = true;
//	JYEscope->CaptureDisplay->Enabled = true;
	 
	RepaintScreen();
 	}
 else {
	// Waveform upload failed
 	JYEscope->Memo1->Lines->Append("Waveform upload failed");
 	}
}

void __fastcall TJYEscope::btnSelectBufferOKCancel(TObject *Sender)
{
 frUploadWaveform->Close();
}

void __fastcall TJYEscope::rbBuffers0Click(TObject *Sender)
{
 SelectedBuffer = 0;
}

void __fastcall TJYEscope::rbBuffers1Click(TObject *Sender)
{
 SelectedBuffer = 1;
}

void __fastcall TJYEscope::rbBuffers2Click(TObject *Sender)
{
 SelectedBuffer = 2;
}

void __fastcall TJYEscope::rbBuffers3Click(TObject *Sender)
{
 SelectedBuffer = 3;
}


void __fastcall TJYEscope::rbRecall0Click(TObject *Sender)
{
 OnScreen = Recall[0]->Recall;
 CurrRecallIndex = 0;
 DisplayRecall();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbRecall1Click(TObject *Sender)
{
 OnScreen = Recall[1]->Recall;
 CurrRecallIndex = 1;
 DisplayRecall();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbRecall2Click(TObject *Sender)
{
 OnScreen = Recall[2]->Recall;
 CurrRecallIndex = 2;
 DisplayRecall();
}
//---------------------------------------------------------------------------

void __fastcall TJYEscope::rbRecall3Click(TObject *Sender)
{
 OnScreen = Recall[3]->Recall;
 CurrRecallIndex = 3;
 DisplayRecall();
}

//---------------------------------------------------------------------------
// Send data to FG085 EEPROM
//

void __fastcall TJYEscope::GenDownload085Execute(TObject *Sender)
{
 U8 	*buf, buf0[300];
 U16 tmp0;
 sSIZE *wptr;

 buf = buf0;

 *buf++ = FID_WriteWaveform;
 *(U16 *)buf = 14 + 256;	// Frame size
 buf += 3;
 *(U16 *)buf = 256;		// Data size
 buf += 2;
 buf += 8;		// 8 dummy bytes to cover the offset in 085 receiving
 
 // Copy data from waveform buffer
 wptr = OnScreen->oscCh[0].BufDupA;
 tmp0 = 0;
 while(tmp0 < 256) {
	*buf++ = (U8)*wptr++;
	tmp0++;
	}

 JYEscope->Memo1->Lines->Append(AnsiString("Sending data"));
 // Send 
 tmp0 = TxProc(buf0, *((U16 *)(buf0 + 1)));    
 
 JYEscope->Memo1->Lines->Append(IntToStr(tmp0) + " bytes sent");
// JYEscope->Memo1->Lines->Append(AnsiString("Sending ends"));

}
//---------------------------------------------------------------------------


 // Upload saved waveform from frontend
void __fastcall TJYEscope::CaptureUploadExecute(TObject *Sender)
{
 U8	tmp0;
 
 // Open a window for buffer index selecting
 if(frUploadWaveform == 0) {
 	frUploadWaveform = new TForm(this);
	// frUploadWaveform->Parent = this;
	// TForm *Test = new TForm(this);
	 
	 frUploadWaveform->Top = 300;
	 frUploadWaveform->Left = 400;
	 frUploadWaveform->Height = 220;
	 frUploadWaveform->Width = 180;
	 frUploadWaveform->Constraints->MaxHeight = 220;
	 frUploadWaveform->Constraints->MinHeight = 220;
	 frUploadWaveform->Constraints->MaxWidth = 180;
	 frUploadWaveform->Constraints->MinWidth = 180;
	 frUploadWaveform->Caption = "Select buffer";

	 rgSelectBuffer = new TRadioGroup(frUploadWaveform);
	 rgSelectBuffer->Parent = frUploadWaveform;
	 rgSelectBuffer->Caption = "Select buffer";
	 rgSelectBuffer->Font->Style = TFontStyles()<< fsBold;
	 rgSelectBuffer->Top = 20;
	 rgSelectBuffer->Left = 38;
	 rgSelectBuffer->Height = 110;
	 rgSelectBuffer->Width = 100;
	 rgSelectBuffer->Show();

	 tmp0 = 0;
	 while(tmp0 < 4) {
		rbBuffers[tmp0] = new TRadioButton(rgSelectBuffer);
		rbBuffers[tmp0]->Parent = rgSelectBuffer;
		rbBuffers[tmp0]->Top = 20 + tmp0 * 20;
		rbBuffers[tmp0]->Left = 10;
		rbBuffers[tmp0]->Width = 60;
		rbBuffers[tmp0]->Caption = "Buffer " + IntToStr(tmp0);
		rbBuffers[tmp0]->Font->Style = TFontStyles();
		rbBuffers[tmp0]->Show();
		tmp0++;
	 	}
	 rbBuffers[0]->Checked = true;
	 SelectedBuffer = 0;
	 rbBuffers[0]->OnClick = rbBuffers0Click;
	 rbBuffers[1]->OnClick = rbBuffers1Click;
	 rbBuffers[2]->OnClick = rbBuffers2Click;
	 rbBuffers[3]->OnClick = rbBuffers3Click;
	 
	 btnSelectBufferCancel = new TButton(frUploadWaveform);
	 btnSelectBufferCancel->Parent = frUploadWaveform;
	 btnSelectBufferCancel->Top = 150;
	 btnSelectBufferCancel->Left = 93;
	 btnSelectBufferCancel->Height = 20;
	 btnSelectBufferCancel->Width = 60;
	 btnSelectBufferCancel->Caption = "Cancel";
	 btnSelectBufferCancel->Enabled = true;
	 btnSelectBufferCancel->OnClick = btnSelectBufferOKCancel;;
	 btnSelectBufferCancel->Show();
	 
	 btnSelectBufferOK = new TButton(frUploadWaveform);
	 btnSelectBufferOK->Parent = frUploadWaveform;
	 btnSelectBufferOK->Top = 150;
	 btnSelectBufferOK->Left = 23;
	 btnSelectBufferOK->Height = 20;
	 btnSelectBufferOK->Width = 60;
	 btnSelectBufferOK->Caption = "OK";
	 btnSelectBufferOK->Enabled = true;
	 btnSelectBufferOK->OnClick = btnSelectBufferOKClick;
	 btnSelectBufferOK->Show();
 	}

 
 frUploadWaveform->Show();

 
// Test->ShowModal();
 
 // Create oscilloscope object to hold the waveform

 // Send command

 // Wait for job done
}
//---------------------------------------------------------------------------


JRecall::JRecall(void)
{
 Recall = NULL;
 Btn = NULL;

}

JRecall::~JRecall(void)
{
 if(Recall !=  NULL) {
 	delete Recall;
	Recall = NULL;
 	}

 if(Btn != NULL) {
 	Btn->Caption = "N/A";
	Btn->Enabled = false;
	Btn = NULL;
 	}

}

void DisplayRefreshThreadStart(void *arg)
{

// DispRefresh();
 SwapBuffer();
 
/*
 class Oscilloscope *scopeunit;

 scopeunit = (Oscilloscope*) arg;

 if(scopeunit != 0) {
 	scopeunit->DispRefresh();
 	}
*/

}

void	SwapBuffer(void)
{
 bool	done;
 long	Request;

 done = false;

 GetLastError();		// Clear any pending error

 while(!done) {
	Request = WaitForSingleObject(WaveformUpdateEvent, INFINITE);
	
// 	if(Capturer->oscState == OS_Running) {
		if(Request == WAIT_OBJECT_0) {
/*			
			// Swap buffer and update display
			if(jyScope->oscBufSelector == 0) {
				// Select Buffer A
				Buf1Curr = jyScope->oscCh[0].BufA;
				Buf2Curr = jyScope->oscCh[1].BufA;
				}
			else {
				// Select Buffer B
				Buf1Curr = jyScope->oscCh[0].BufB;
				Buf2Curr = jyScope->oscCh[1].BufB;
				}
*/
			// Display captured waveform (Buffer swap has been done in serial data receiving)
//			OnScreen = Capturer;
 			WaitForSingleObject(ShowWaveMutex, INFINITE);
 			ShowWave();
 			ReleaseMutex(ShowWaveMutex);
 			}
//		}
 	}

}

// --------------------------------------------------------------------
// Display the instance pointed by OnScreen
//
void ShowWave(void)
{
 S16 tmp0, tmp1, tmp2, tmp3, tmp4;
 Graphics::TBitmap *TempWaveform = 0;

 // TempWaveform area
 TempWaveform = new Graphics::TBitmap;
 TempWaveform->Canvas->Lock();
 
 TempWaveform->Width = WWindowSizex + 1;       
 TempWaveform->Height = WWindowSizey + 1;      
 TempWaveform->Transparent = True;
 TempWaveform->TransparentMode = tmFixed;
 TempWaveform->TransparentColor = TempWaveform->Canvas->Pixels[20][20];
// Waveform->TransparentColor = clBlack;

 TempWaveform->Canvas->Brush->Color = BGColor;
 TempWaveform->Canvas->FillRect(Rect(0, 0, WWindowSizex + 1, WWindowSizey + 1));

/*
 // Erase previous waveform
 DrawWave(Buf1Prev, Buffer1EndPrev, Ch1VPosPrev, clBlack,  TempWaveform);
 DrawWave(Buf2Prev, Buffer2EndPrev, Ch2VPosPrev, clBlack,  TempWaveform);

 // Erase trigger level line 
 TempWaveform->Canvas->Pen->Color = clBlack;
 TempWaveform->Canvas->MoveTo(0, TrigLvlPrev - (Screeny0 + WWindowOfsy) + 8);
 TempWaveform->Canvas->LineTo(WWindowSizex, TrigLvlPrev - (Screeny0 + WWindowOfsy) + 8);

 // Erase trig point line
// TempWaveform->Canvas->Pen->Color = clBlack;
 TempWaveform->Canvas->MoveTo(TrigPoint->Left - (Screenx0 + WWindowOfsx) + 3, 0);
 TempWaveform->Canvas->LineTo(TrigPoint->Left - (Screenx0 + WWindowOfsx) + 3, WWindowSizey);
*/

// -------------------------------------
 // Draw grid
// PaneWaveform->Canvas->Pen->Color = 0x6a6e73; 
 TempWaveform->Canvas->Pen->Color = 0x333333; 
 tmp0 = 0;
 tmp1 =  0;
 tmp2 = tmp0 + WWindowSizex;
 tmp3 = tmp1 + WWindowSizey;

 tmp4 = 1;
 while(tmp4 < (unsigned char) (WWindowSizey/GridSizey)) {
	TempWaveform->Canvas->MoveTo(tmp0, tmp1 + GridSizey * tmp4);	
	TempWaveform->Canvas->LineTo(tmp2,  tmp1 + GridSizey * tmp4);
	tmp4++;
 	}

 tmp4 = 1;
 while(tmp4 < (unsigned char)(WWindowSizex/GridSizex)) {
 	TempWaveform->Canvas->MoveTo(tmp0  + GridSizex * tmp4, tmp1);	
	TempWaveform->Canvas->LineTo(tmp0 + GridSizex * tmp4,  tmp3);
	tmp4++;
 	}

 TempWaveform->Canvas->Pen->Color = clSilver; 
 TempWaveform->Canvas->MoveTo(tmp0, tmp1);	
 TempWaveform->Canvas->LineTo(tmp0, tmp3);
 TempWaveform->Canvas->LineTo(tmp2, tmp3);
 TempWaveform->Canvas->LineTo(tmp2, tmp1);
 TempWaveform->Canvas->LineTo(tmp0, tmp1);

 tmp4 = 0;
 while(tmp4 < (WWindowSizex/GridSizex) * 5) {
	TempWaveform->Canvas->MoveTo(tmp0 + (GridSizex/5) * tmp4 , tmp1 + WWindowSizey/2 - (GridSizex/10));	
	TempWaveform->Canvas->LineTo(tmp0 + (GridSizex/5) * tmp4, tmp1 + WWindowSizey/2 + (GridSizex/10) + 1);	
	tmp4++;
 	}

 tmp4 = 0;
 while(tmp4 < (WWindowSizey/GridSizey) * 5) {
	TempWaveform->Canvas->MoveTo(tmp0 + WWindowSizex/2 - (GridSizey/10), tmp1 + (GridSizex/5) * tmp4 );	
	TempWaveform->Canvas->LineTo(tmp0 + WWindowSizex/2 + (GridSizey/10) + 1, tmp1 + (GridSizex/5) * tmp4 );	
	tmp4++;
 	}

// -------------------------------------
 // Draw trigger level line if requested
// if((TrigLvlLineOn == 1) || (TrigLvlLineAlwaysOn == 1)) {
 if(BitTest(OnScreen->oscFlags, OF_TrigLvlLineOn) || BitTest(OnScreen->oscFlags, OF_TrigLvlLineAlwaysOn)) {
 	// Draw
	 switch(OnScreen->oscTrigSrc) {
		case TS_Ch1:
		default:
		 	tmp0 = TrigLvl_Ch1->Top - (Screeny0 + WWindowOfsy) + 8;
			break;

		case TS_Ch2:
		 	tmp0 = TrigLvl_Ch2->Top - (Screeny0 + WWindowOfsy) + 8;
			break;
	 	}
 	TempWaveform->Canvas->Pen->Color = clTrigger;
 	TempWaveform->Canvas->MoveTo(0, tmp0);
 	TempWaveform->Canvas->LineTo(WWindowSizex, tmp0);
// 	TrigLvlPrev = TrigLvl->Top;
 	}

// -------------------------------------
 // Draw trig point line if requested
// if((TrigPointLineOn == 1) || (TrigPointLineAlwaysOn == 1)) {
 if(BitTest(OnScreen->oscFlags, OF_TrigPointLineOn) || BitTest(OnScreen->oscFlags, OF_TrigPointLineAlwaysOn)) {
 	// Draw
 	TempWaveform->Canvas->Pen->Color = clTrigger;
 	TempWaveform->Canvas->MoveTo(TrigPoint->Left - (Screenx0 + WWindowOfsx) + 3, 0);
 	TempWaveform->Canvas->LineTo(TrigPoint->Left - (Screenx0 + WWindowOfsx) + 3, WWindowSizey);
 	}

// -------------------------------------
 // Draw cursor T
 if(BitTest(OnScreen->oscFlags, OF_CursorT_On)) {
 	// Draw cursor T1
 	tmp0 = OnScreen->oscCursorT1 - OnScreen->oscHPos;
	if(tmp0 < 0) {
		tmp0 = 0;
		}
 	if(tmp0 > WWindowSizex) {
		tmp0 = WWindowSizex;
 		}
		
 	CursorT1_Top->Left = tmp0 + WWindowOfsx - 1;
 	CursorT1_Bottom->Left = tmp0 + WWindowOfsx - 1;
 	CursorT1_Top->Show();
	CursorT1_Bottom->Show();

 	TempWaveform->Canvas->Pen->Color = clLime;
	tmp1 = 0;
	while(tmp1 < 13) {
 		TempWaveform->Canvas->MoveTo(tmp0, tmp1 * 24);
 		TempWaveform->Canvas->LineTo(tmp0, tmp1 * 24 + 12);
		tmp1++;
		}
	
	
 	// Draw cursor T2
 	tmp0 = OnScreen->oscCursorT2 - OnScreen->oscHPos;
	if(tmp0 < 0) {
		tmp0 = 0;
		}
 	if(tmp0 > WWindowSizex) {
		tmp0 = WWindowSizex;
 		}
	
 	CursorT2_Top->Left =  tmp0 + WWindowOfsx - 1;
 	CursorT2_Bottom->Left =  tmp0 + WWindowOfsx - 1;
 	CursorT2_Top->Show();
	CursorT2_Bottom->Show();
	
 	TempWaveform->Canvas->Pen->Color = clLime;
	tmp1 = 0;
	while(tmp1 < 13) {
 		TempWaveform->Canvas->MoveTo(tmp0, tmp1 * 24);
 		TempWaveform->Canvas->LineTo(tmp0, tmp1 * 24 + 12);
		tmp1++;
		}
	}

// -------------------------------------
 // Draw cursor V
 if(BitTest(OnScreen->oscFlags, OF_CursorV1_On)) {
 	// Draw Ch1 delta V cursor 1
 	tmp0 = OnScreen->oscCh1Vcursor1 + WWindowSizey/2 - OnScreen->oscCh[0].VPos;
 	if(tmp0 < 0) {
		tmp0 = 0;
 		}
 	if(tmp0 > WWindowSizey) {
		tmp0 = WWindowSizey;
 		}
		
 	Ch1Vcursor1_Left->Top = tmp0 + Screeny0 + WWindowOfsy - 3;
 	Ch1Vcursor1_Right->Top = tmp0 + Screeny0 + WWindowOfsy - 3;
 	Ch1Vcursor1_Left->Show();
	Ch1Vcursor1_Right->Show();

 	TempWaveform->Canvas->Pen->Color = clYellow;
	tmp1 = 0;
	while(tmp1 < 28) {
 		TempWaveform->Canvas->MoveTo(tmp1 * 24, tmp0);
 		TempWaveform->Canvas->LineTo(tmp1 * 24 + 12, tmp0);
		tmp1++;
		}
 
 	// Draw Ch1 delta V cursor 2
 	tmp0 = OnScreen->oscCh1Vcursor2 + WWindowSizey/2 - OnScreen->oscCh[0].VPos;
 	if(tmp0 < 0) {
		tmp0 = 0;
 		}
 	if(tmp0 > WWindowSizey) {
		tmp0 = WWindowSizey;
 		}
		
 	Ch1Vcursor2_Left->Top = tmp0 + Screeny0 + WWindowOfsy - 3;
 	Ch1Vcursor2_Right->Top = tmp0 + Screeny0 + WWindowOfsy - 3;
 	Ch1Vcursor2_Left->Show();
	Ch1Vcursor2_Right->Show();

	tmp1 = 0;
	while(tmp1 < 28) {
 		TempWaveform->Canvas->MoveTo(tmp1 * 24, tmp0);
 		TempWaveform->Canvas->LineTo(tmp1 * 24 + 12, tmp0);
		tmp1++;
		}
 	}
 else {

 	}

 if(BitTest(OnScreen->oscFlags, OF_CursorV2_On)) {
 	// Draw Ch2 delta V cursor 1
 	tmp0 = OnScreen->oscCh2Vcursor1 + WWindowSizey/2 - OnScreen->oscCh[1].VPos;
 	if(tmp0 < 0) {
		tmp0 = 0;
 		}
 	if(tmp0 > WWindowSizey) {
		tmp0 = WWindowSizey;
 		}
		
 	Ch2Vcursor1_Left->Top = tmp0 + Screeny0 + WWindowOfsy - 3;
 	Ch2Vcursor1_Right->Top = tmp0 + Screeny0 + WWindowOfsy - 3;
 	Ch2Vcursor1_Left->Show();
	Ch2Vcursor1_Right->Show();

 	TempWaveform->Canvas->Pen->Color = clAqua;
	tmp1 = 0;
	while(tmp1 < 28) {
 		TempWaveform->Canvas->MoveTo(tmp1 * 24, tmp0);
 		TempWaveform->Canvas->LineTo(tmp1 * 24 + 12, tmp0);
		tmp1++;
		}
 
 	// Draw Ch2delta V cursor 2
 	tmp0 = OnScreen->oscCh2Vcursor2 + WWindowSizey/2 - OnScreen->oscCh[1].VPos;
 	if(tmp0 < 0) {
		tmp0 = 0;
 		}
 	if(tmp0 > WWindowSizey) {
		tmp0 = WWindowSizey;
 		}
		
 	Ch2Vcursor2_Left->Top = tmp0 + Screeny0 + WWindowOfsy - 3;
 	Ch2Vcursor2_Right->Top = tmp0 + Screeny0 + WWindowOfsy - 3;
 	Ch2Vcursor2_Left->Show();
	Ch2Vcursor2_Right->Show();

	tmp1 = 0;
	while(tmp1 < 28) {
 		TempWaveform->Canvas->MoveTo(tmp1 * 24, tmp0);
 		TempWaveform->Canvas->LineTo(tmp1 * 24 + 12, tmp0);
		tmp1++;
		}
 	}

// -------------------------------------
 // Draw new waveform
 if(BitTest(OnScreen->oscChnCfg, CF_Ch1)) {
 	// Draw Ch1 waveform
 	OnScreen->oscCh[0].Average = DrawWave(OnScreen, 0,  clYellow,  TempWaveform);
 	}
 
 if(BitTest(OnScreen->oscChnCfg, CF_Ch2)) {
 	// Draw Ch2 waveform
 	OnScreen->oscCh[1].Average = DrawWave(OnScreen, 1, clAqua,  TempWaveform);
 	}

 PaneWaveform->Picture->Bitmap->Assign(TempWaveform);
 TempWaveform->Canvas->Unlock();

 delete TempWaveform;
}

//---------------------------------------------------------------------------

//S32 DrawWave(U16 *WaveBuf, U16 *bufferend, S16  VPos, TColor Color, Graphics::TBitmap *BitmapCanvas)
S32 DrawWave(Oscilloscope *osc, U16 chn,  TColor Color, Graphics::TBitmap *BitmapCanvas)
{
 S16	tmp0, tmp1, tmp2;
 U16	tmp3;
 S32 average;
 sSIZE	*WaveBuf;
 sSIZE	*Bufferend;

 // Select bufffer to be displayed
 if(osc->oscState == OS_Running) {
 	// Select buffer A or B
 	if(osc->oscBufSelector == 0) {
		// Select buffer B (Note: 0 for A, 1 for B. The selector indicates the buffer used for active data capture. So use the other buffer for dispaly)
		WaveBuf = osc->oscCh[chn].BufB;
 		}
	else {
		// Buffer A
		WaveBuf = osc->oscCh[chn].BufA;
 		}
 	}
 else {
	// Select the duplicated data
	WaveBuf = osc->oscCh[chn].BufDupA;
 	}
// JYEscope->Memo1->Lines->Append("Buffer selector: " + IntToStr(osc->oscBufSelector));
// JYEscope->Memo1->Lines->Append("OSC state: " + IntToStr(osc->oscState));
  
 // Set buffer end
 Bufferend = WaveBuf + osc->oscRecLen;
 // Start address of drawing
 WaveBuf += osc->oscHPos;
 // Round it if it's out of selected buffer
 if(WaveBuf >= Bufferend) {
	// Round to buffer start
	WaveBuf -= osc->oscRecLen;
 	}

 tmp2 = osc->oscCh[chn].VPos;
 
 // Draw new waveform
 BitmapCanvas->Canvas->Pen->Color = Color;
 average = *WaveBuf;
 tmp1 = WWindowSizey/2 + 1 + V0 - (tmp2 + *WaveBuf++) ;	// Waveform should be at middle when [(256 * 3)/2 -  (VPos + *WaveBuf++)] is 0
 if(WaveBuf >= Bufferend) {
	// Round to buffer start
	WaveBuf -= osc->oscRecLen;
 	}
 if(tmp1 < 0 ) {
 	tmp1 = 0;
 	}
 if(tmp1 > WWindowSizey) {
 	tmp1 = WWindowSizey;
 	}
 
 BitmapCanvas->Canvas->MoveTo(0, tmp1);
 tmp3 = WWindowSizex;
 if(osc->oscRecLen < WWindowSizex) {
 	tmp3 = osc->oscRecLen;
 	}
 
 tmp0 = 1;
 while(tmp0 < tmp3) {
 //    	JYEscope->Memo1->Lines->Append(IntToStr(tmp0));
 	average += *WaveBuf;
 	tmp1 = WWindowSizey/2 + 1 + V0 - (tmp2 + *WaveBuf++);	// Waveform should be at middle when [(256 * 3)/2 -  (VPos + *WaveBuf++)] is 0
 	if(WaveBuf >= Bufferend) {
		// Round to buffer start
		WaveBuf -= osc->oscRecLen;
 		}
 	if(tmp1 < 0 ) {
 		tmp1 = 0;
 		}
 	if(tmp1 > WWindowSizey) {
 		tmp1 = WWindowSizey;
 		}
 	BitmapCanvas->Canvas->LineTo(tmp0, tmp1);
    	tmp0++;
    	}

 return (average/tmp3);
}

//---------------------------------------------------------------------------

void	RepaintWave(Oscilloscope *osc)
{
 WaitForSingleObject(ShowWaveMutex, INFINITE);

// Waveform->Canvas->Lock();	
 OnScreen = osc;
 ShowWave();
// RepaintScreen();
// Waveform->Canvas->Unlock();	
 

 ReleaseMutex(ShowWaveMutex);
}

void VSenRefresh(void)
{
 if(BitTest(OnScreen->oscChnCfg, CF_Ch1)) {
 	JYEscope->lbCh1Sen->Caption = VSenString[OnScreen->GetVSen(Channel_1)];
 	}
 else {
 	JYEscope->lbCh1Sen->Caption = " OFF ";
 	}
 if(BitTest(OnScreen->oscChnCfg, CF_Ch2)) {
 	JYEscope->lbCh2Sen->Caption = VSenString[OnScreen->GetVSen(Channel_2)];
 	}
 else {
 	JYEscope->lbCh2Sen->Caption = " OFF ";
 	}

 // Select the corresponding radio button
 if(ptr1 = (TRadioButton *)JYEscope->Ch1->FindChildControl(Ch1VsenButtonNames[OnScreen->GetVSen(Channel_1)])) {
	ptr1->Checked = true;
 	}

 if(ptr1 = (TRadioButton *)JYEscope->Ch2->FindChildControl(Ch2VsenButtonNames[OnScreen->GetVSen(Channel_2)])) {
	ptr1->Checked = true;
 	}

 // Backup sensitivity if in running mode
 if(OnScreen->oscState == OS_Running) {
 	OnScreen->oscCh[0].SenOrig = OnScreen->oscCh[0].Sen;
 	OnScreen->oscCh[1].SenOrig = OnScreen->oscCh[1].Sen;
 	}
 else {
	Transform(OnScreen, Trans_Vertical);
	RepaintWave(OnScreen);
 	}

 if(OnScreen->Model == DSO_Model_094) {
 	Command(FID_DSOparam);
 	}

 TrigLvlRefresh();
 CursorV_Refresh();
}
// ------------------------------------------------------------------------

void VPosRefresh(void)
{
 S16	tmp0;

 if(BitTest(OnScreen->oscChnCfg, CF_Ch1)) {
 	// Ch1 is on
//	 tmp0 = Screeny0 + WWindowOfsy + (WWindowSizey/2 - (OnScreen->oscCh[0].VPos - OnScreen->oscCh[0].VPosOfs)) - 7;
	 tmp0 = Screeny0 + WWindowOfsy + (WWindowSizey/2 - (OnScreen->oscCh[0].VPos - (V0 - OnScreen->oscCh[0].Reference))) - 7;
	 if(tmp0 > Screeny0 + WWindowOfsy + WWindowSizey - 7) {
		tmp0 = Screeny0 + WWindowOfsy + WWindowSizey - 7;
	 	}
	 if(tmp0 < Screeny0 + WWindowOfsy - 7) {
		tmp0 = Screeny0 + WWindowOfsy - 7;
	 	}
	 Ch1Pos->Top = tmp0;
	 Ch1Pos->Show();
 	}
 else {
 	Ch1Pos->Hide();
 	}
 
 if(BitTest(OnScreen->oscChnCfg, CF_Ch2)) {
 	// Ch2 is on
//	 tmp0 = Screeny0 + WWindowOfsy + (WWindowSizey/2 - (OnScreen->oscCh[1].VPos - OnScreen->oscCh[1].VPosOfs)) - 7;
	 tmp0 = Screeny0 + WWindowOfsy + (WWindowSizey/2 - (OnScreen->oscCh[1].VPos - (V0 - OnScreen->oscCh[1].Reference))) - 7;
	 if(tmp0 > Screeny0 + WWindowOfsy + WWindowSizey - 7) {
		tmp0 = Screeny0 + WWindowOfsy + WWindowSizey - 7;
	 	}
	 if(tmp0 < Screeny0 + WWindowOfsy - 7) {
		tmp0 = Screeny0 + WWindowOfsy - 7;
	 	}
	 Ch2Pos->Top = tmp0;
	 Ch2Pos->Show();
 	}
 else {
 	Ch2Pos->Hide();
 	}

 TrigLvlRefresh();
}

void CplRefresh(void)
{
 if((OnScreen->oscChnCfg & 0x01) != 0) {
 	JYEscope->lbCh1CplCurr->Caption = CplString[OnScreen->oscCh[0].Cpl];
 	}
 else {
 	JYEscope->lbCh1CplCurr->Caption = "  ";
 	}
 JYEscope->cbCh1Cpl->ItemIndex = OnScreen->oscCh[0].Cpl;
 
 if((OnScreen->oscChnCfg & 0x02) != 0) {
 	JYEscope->lbCh2CplCurr->Caption = CplString[OnScreen->oscCh[1].Cpl];
 	}
 else {
 	JYEscope->lbCh2CplCurr->Caption = "  ";
 	}
 JYEscope->cbCh2Cpl->ItemIndex = OnScreen->oscCh[1].Cpl;

 if(OnScreen->Model == DSO_Model_094) {
 	Command(FID_DSOparam);
 	}
}


void TBRefresh(void)
{
 // Update label
 JYEscope->lbTimebase->Caption = TBString[OnScreen->GetTimeBase()];

 // Check the selected radio button
 if(ptr1 = (TRadioButton *)JYEscope->gbTimeBase->FindChildControl(TBButtonNames[OnScreen->oscTimebase])) {
	ptr1->Checked = true;
 	}
 
 // Backup timebase if in running mode
 if(OnScreen->oscState == OS_Running) {
 	OnScreen->oscTBOrig = OnScreen->oscTimebase;
 	}
 else {
 	// Do transform if not in running
 	Transform(OnScreen, Trans_Horizon);
	RepaintWave(OnScreen);
 	}

 CursorT_Refresh();
}
// ----------------------------------------------------------------------    

void HPosRefresh(void)
{
 S32 tmp0;
 U32 tmp1;
 U16 tmp2;

 tmp0 = (OnScreen->GetHPos());
 tmp1 = (OnScreen->GetRecLen());

 // Calculate currently HPos bar width
 tmp2 =  (U32)((U32)(WWindowSizex) * ((U32)HPosIndicatorSizex - 10)) /tmp1;   
 if(tmp2 > (HPosIndicatorSizex - 10)) {
 	// RecLen < WWindowSizex. 
 	tmp2 = HPosIndicatorSizex - 10;
 	}
 if(tmp2 != HPosBarWidthPrev) {
 	HPosBarBitmap->Width = tmp2;        
 	HPosBarBitmap->Height = 9;      
 	HPosBarBitmap->Transparent = True;
 	HPosBarBitmap->TransparentMode = tmFixed;
 	HPosBarBitmap ->Canvas->Brush->Color = clBlack;
 	HPosBarBitmap ->Canvas->Pen->Color = clWhite;
 	HPosBarBitmap ->Canvas->RoundRect(0, 0, HPosBarBitmap->Width, HPosBarBitmap->Height, 4, 4);
 	HPosBarBitmap->TransparentColor = HPosBarBitmap->Canvas->Pixels[3][3];
	HPosBarWidthPrev = tmp2;
 	HPosBar->Picture->Bitmap->TransparentColor = HPosBar->Canvas->Pixels[2][2];
 	HPosBar->Width = HPosBarBitmap->Width;        
 	HPosBar->Height = HPosBarBitmap->Height;     
 	}

 if((tmp1 >= WWindowSizex) && ((tmp0 + (WWindowSizex)) > tmp1)) {
 	// HPos too large. Adjust it to fit
 	tmp0 = tmp1 - (WWindowSizex);
 	OnScreen->SetHPos(tmp0);
 	}
	 
 HPosBar->Left = Screenx0 + 50 + 210 + (tmp0 * (U32)(HPosIndicatorSizex - 10))/tmp1;
 HPosBar->Picture->Bitmap = HPosBarBitmap;

 JYEscope->edHPosReadout->Text = IntToStr(OnScreen->GetHPos());

// -------------------------
 // Update trig point
 TrigPoint->Left = Screenx0 + WWindowOfsx + ((OnScreen->oscRecLen * OnScreen->oscTrigPos)/100 - OnScreen->oscHPos) - 3;
 TrigPoint->Show();
 TPOutLeft->Hide();
 TPOutRight->Hide();
 if(TrigPoint->Left < Screenx0 + WWindowOfsx - 3) {
	// Trig point out at left
	TrigPoint->Hide();
	TPOutLeft->Show();
 	}
 if(TrigPoint->Left > Screenx0 + WWindowOfsx + WWindowSizex - 3) {
	// Trig point out at right
	TrigPoint->Hide();
	TPOutRight->Show();
 	}

// ---------------------------
 // Update record length display
 // [2014.02.03 Start --]
 // Do not insert new values to the record lenght list. Use a fixed list for each model.
 //	Load the list when device is connected.

 tmp0 = JYEscope->cbRecordLength->Items->IndexOf(IntToStr(OnScreen->oscRecLen));
 if(tmp0 != -1) {
 	JYEscope->cbRecordLength->ItemIndex = tmp0;
 	}
 else {
// 	JYEscope->cbRecordLength->Items->Insert(JYEscope->cbRecordLength->Items->Count, IntToStr(OnScreen->oscRecLen));
// 	JYEscope->cbRecordLength->ItemIndex = JYEscope->cbRecordLength->Items->Count - 1;	
 	JYEscope->cbRecordLength->ItemIndex = 0;		// Display the first item
 	}
 // [2014.02.03 -- End]

}
// -----------------------------------------------------------------------------------

void TrigModeRefresh(void)
{
 JYEscope->lbTrigModeCurr->Caption = TrigModeString[OnScreen->GetTrigMode()];

 JYEscope->cbTriggerMode->ItemIndex = OnScreen->GetTrigMode();

}

void TrigSlopeRefresh(void)
{
 if(OnScreen->GetTrigSlope() == TE_Rising) {
 	TrigSlope->Picture = PosSlopeBmp->Picture;
//	return;
 	}
 if(OnScreen->GetTrigSlope() == TE_Falling) {
 	TrigSlope->Picture = NegSlopeBmp->Picture;
//	return;
 	}

 JYEscope->cbTriggerSlope->ItemIndex = OnScreen->GetTrigSlope();

}

void TrigSrcRefresh(void)
{
 JYEscope->lbTrigSrcCurr->Caption = TrigSrcString[OnScreen->GetTrigSrc()];
 JYEscope->cbTriggerSrc->ItemIndex = OnScreen->GetTrigSrc();
// BitClr(OnScreen->oscFlags, OF_TrigLvlLineAlwaysOn);
 TrigLvlRefresh();
 RepaintWave(OnScreen);
}

void TrigLvlRefresh(void)
{
 U16 tmp0;
 U16 tmp1;
 S16 tmp2, tmp3, tmp4;
 float ftmp4;

 // Get VSen and VPos based on trigger source
 tmp1 = OnScreen->oscTrigSrc;
 switch(tmp1) {
	case TS_Ch1:
	default:
	 	tmp0 = OnScreen->oscCh[0].Sen;
		tmp2 = OnScreen->oscCh[0].VPos - (V0 - OnScreen->oscCh[0].Reference);
		break;

	case TS_Ch2:
	 	tmp0 = OnScreen->oscCh[1].Sen;
		tmp2 = OnScreen->oscCh[1].VPos - (V0 - OnScreen->oscCh[1].Reference);
		break;
 	}


 // Refresh trig level indicator
 tmp3 = OnScreen->oscTrigLvl;
 tmp4 = Screeny0 + WWindowOfsy + (WWindowSizey/2 - (tmp3 + tmp2)) -  7;
// tmp1 += (Y - Mousey0);
 if(tmp4 < (Screeny0 + WWindowOfsy) - 8) {
    tmp4 = (Screeny0 + WWindowOfsy) - 8;
    }
 if(tmp4 > ((Screeny0 + WWindowOfsy) + WWindowSizey - 1 - 8)) {
    tmp4 = (Screeny0 + WWindowOfsy) + WWindowSizey - 1 - 8;
    }

 switch(tmp1) {
	case TS_Ch1:
	default:
		TrigLvl_Ch1->Top = tmp4;
		TrigLvl_Ch1->Show();
		TrigLvl_Ch2->Hide();
		break;

	case TS_Ch2:
		TrigLvl_Ch2->Top = tmp4;
		TrigLvl_Ch2->Show();
		TrigLvl_Ch1->Hide();
		break;
 	}
 
 // Display Trig Level readout
 ftmp4 = ((float)(tmp3)/(float)GridSizey) * ((float)VSenRate[tmp0]);
 if(tmp0 <= VS_01V) {
 	JYEscope->lbTrigLvlUnit->Caption = "V";
 	JYEscope->edTrigLvlReadout->Text = (FloatToStrF(ftmp4, ffFixed, 3, 2));
 	}
 else {
 	JYEscope->lbTrigLvlUnit->Caption = "mV";
 	JYEscope->edTrigLvlReadout->Text = (FloatToStrF(ftmp4, ffFixed, 3, 1));
 	}

}
// ---------------------------------------------------------------------


void TrigPosRefresh(void)
{
 TrigPos->Left = Screenx0 + PaneVposSizex + HPosIndicatorOfsx + 5 - 4 + (OnScreen->GetTrigPos() * (HPosIndicatorSizex - 10))/100;
 JYEscope->edTrigPos->Text = (IntToStr(OnScreen->GetTrigPos()));
  
 TrigPoint->Left = Screenx0 + WWindowOfsx + ((OnScreen->oscRecLen * OnScreen->oscTrigPos)/100 - OnScreen->oscHPos) - 3;

 TrigPoint->Left = Screenx0 + WWindowOfsx + ((OnScreen->oscRecLen * OnScreen->oscTrigPos)/100 - OnScreen->oscHPos) - 3;
 TrigPoint->Show();
 TPOutLeft->Hide();
 TPOutRight->Hide();
 if(TrigPoint->Left < Screenx0 + WWindowOfsx - 3) {
	// Trig point out at left
	TrigPoint->Hide();
	TPOutLeft->Show();
 	}
 if(TrigPoint->Left > Screenx0 + WWindowOfsx + WWindowSizex - 3) {
	// Trig point out at right
	TrigPoint->Hide();
	TPOutRight->Show();
 	}

}
// ---------------------------------------------------------------

void TrigSenRefresh(void)
{
 JYEscope->edTrigSen->Text = (IntToStr(OnScreen->GetTrigSen()));
  
}
// ---------------------------------------------------------------

void CursorT_Refresh(void)
{
 float ftmp1, ftmp2;
 
 if(BitTest(OnScreen->oscFlags, OF_CursorT_On)) {
	ftmp1 = (float)(((float)OnScreen->oscCursorT2 - (float)OnScreen->oscCursorT1)/30.0) * TBUnitRate[OnScreen->oscTimebase];
	ftmp2 = ftmp1;
	if(ftmp1 < 0.0) {
		ftmp2 = -ftmp1;
		}
	if(ftmp2 != 0.0) {
		ftmp2 = (1.0/(ftmp2 * FreqUnitRatio[OnScreen->oscTimebase]));
 		JYEscope->lbdT->Caption = FloatToStrF(ftmp1, ffFixed, 6, 2) + " " + TBUnitStrings[OnScreen->oscTimebase]
						+ " [ " + FloatToStrF(ftmp2, ffFixed, 6, 3) + " " + FreqUnitStrings[OnScreen->oscTimebase] + " ]";
		}
	else {
 		JYEscope->lbdT->Caption = FloatToStrF(ftmp1, ffFixed, 6, 2) + TBUnitStrings[OnScreen->oscTimebase] + " [ ---------- ]";
		}
 	}
 else {
 	CursorT1_Top->Hide();
	CursorT1_Bottom->Hide();
 	CursorT2_Top->Hide();
	CursorT2_Bottom->Hide();
	
//  	JYEscope->lbdT->Enabled = false;
 	JYEscope->lbdT->Caption = "-----------";
 	}
  
}
// ---------------------------------------------------------------

void CursorV_Refresh(void)
{
 if(BitTest(OnScreen->oscFlags, OF_CursorV1_On)) {
 	JYEscope->lbdV1->Caption = FloatToStrF((float)((OnScreen->oscCh1Vcursor2 - OnScreen->oscCh1Vcursor1)/30.0) * VSenRate[OnScreen->oscCh[0].Sen], 
 									ffFixed, 6, 2) + " " + VSenUnitString[OnScreen->oscCh[0].Sen];
 	}
 else {
 	JYEscope->lbdV1->Caption = "-----------";
 	Ch1Vcursor1_Left->Hide();
 	Ch1Vcursor1_Right->Hide();
 	Ch1Vcursor2_Left->Hide();
 	Ch1Vcursor2_Right->Hide();
 	}

 if(BitTest(OnScreen->oscFlags, OF_CursorV2_On)) {
 	JYEscope->lbdV2->Caption = FloatToStrF((float)((OnScreen->oscCh2Vcursor2 - OnScreen->oscCh2Vcursor1)/30.0) * VSenRate[OnScreen->oscCh[1].Sen], 
 									ffFixed, 6, 2) + " " + VSenUnitString[OnScreen->oscCh[1].Sen];
 	}
 else {
 	JYEscope->lbdV2->Caption = "-----------";
 	Ch2Vcursor1_Left->Hide();
 	Ch2Vcursor1_Right->Hide();
 	Ch2Vcursor2_Left->Hide();
 	Ch2Vcursor2_Right->Hide();
 	}


}
// ---------------------------------------------------------------

void OscStateRefresh(void)
{

 switch(OnScreen->GetOscState()) {
 	case OS_Running:
		JYEscope->lbOscState->Caption = "Running";
		JYEscope->lbOscState->Font->Color = clLime;
		JYEscope->btnRunStop->Caption = "Stop";
//		JYEscope->CaptureRun->Enabled = false;
//		JYEscope->CaptureStop->Enabled = true;
		break;
		
 	case OS_Stopped:
		JYEscope->lbOscState->Caption = "Stopped";
		JYEscope->lbOscState->Font->Color = clRed;
		JYEscope->btnRunStop->Caption = "Run";
//		JYEscope->CaptureRun->Enabled = true;
//		JYEscope->CaptureStop->Enabled = false;
		break;

	case OS_Offline:
	default:
		JYEscope->lbOscState->Caption = "Disconnected";
		JYEscope->lbOscState->Font->Color = clFuchsia;
		JYEscope->btnRunStop->Caption = "Run";
//		JYEscope->CaptureRun->Enabled = false;
//		JYEscope->CaptureStop->Enabled = false;
		break;
		
	case OS_Recall:
		JYEscope->lbOscState->Caption = OnScreen->Caption;
		JYEscope->lbOscState->Font->Color = clSilver;
		JYEscope->btnRunStop->Caption = "Run";
//		JYEscope->CaptureRun->Enabled = false;
//		JYEscope->CaptureStop->Enabled = false;
		break;
		
 	}
//  OscStateInd->Update();

}

void	ScanPorts(void)
{
 U16	tmp0;
 AnsiString sPortName;

 // List available comm ports
// JYEscope->cbPortList->Items->Clear();
 fmPortSetup->cbPortList->Items->Clear();
 tmp0 = 1;
 while(tmp0 < 256) {
 	sPortName = "COM" + IntToStr(tmp0);
	JYEscope->Memo1->Lines->Append(sPortName);
	if(PortAvailable(sPortName.c_str()) != 0) {
		// Add to port list
		fmPortSetup->cbPortList->Items->Add(sPortName);
		// Report
		JYEscope->Memo1->Lines->Append(sPortName + " found");
		}
	tmp0++;
 	}

}

// -------------------------------------------------------------------------
// Save the waveform in the current displayed buffer to file
//
void SaveWaveToFile(AnsiString filename)
{
 FILE *file;
 TDateTime today;
 U32	tmp0;
 sSIZE	*ptmp1, *ptmp2;
 
 today = Now();
 LongTimeFormat = "hh:nn:ss"; 

 // Open waveform file. Create it if not exist
 if(!(file = fopen(filename.c_str(), "w"))) {
 	// File open failed
 	Application->MessageBox("Can not open file.", "Message", MB_OK);
	};

 // Write file header

 // 1st line - File signature, data type, date, time, model, manufacturer
 fprintf(file, "%s,%s,%s,%s,%s,%s\n", 
 		"JYDZ", 
 		"Waveform", 
 		DateToStr(today), 
 		TimeToStr(today),
 		OnScreen->Model,
 		OnScreen->Manufacturer
 		);
 
 // 2nd line - Number of parameters in line 4, horizontal and vertical screen resolution
 fprintf(file, "%i,%i,%i\n", NumOfParam, 30, 30);
 
 // 3rd line - names of capture parameters
 fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
 		"ChnNum",
 		"RecLen",
 		"ChnCfg",
 		"SampleRate",
 		"Resolution",
 		"Timebase",
 		"HPos",
 		"TrigMode",
 		"TrigSlope",
 		"TrigLvl",
 		"TrigSrc",
 		"TrigPos",
 		"TrigSen",
 		"TBcopy"
 		);
 
 // 4th line - Capture parameters
 fprintf(file, "%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\n", 	
 		OnScreen->oscNumOfChn,
		OnScreen->oscRecLen,
		OnScreen->oscChnCfg,
		OnScreen->SampleRate,
		OnScreen->Resolution,
		OnScreen->oscTimebase,
		OnScreen->oscHPos,
		OnScreen->oscTrigMode,
		OnScreen->oscTrigSlope,
		OnScreen->oscTrigLvl,
		OnScreen->oscTrigSrc,
		OnScreen->oscTrigPos,
		OnScreen->oscTrigSen,
		OnScreen->oscTBOrig
 		);
 
 // 5th line - Sensitivities
 tmp0 = 0;
 while(tmp0 < NumOfChannel) {
	fprintf(file, "%i", OnScreen->oscCh[tmp0].Sen);
	fprintf(file, "%c", ',');
	tmp0++;
 	}
 fprintf(file, "%c", '\n');
  
 // 6th line - Coupling
 tmp0 = 0;
 while(tmp0 < NumOfChannel) {
	fprintf(file, "%i", OnScreen->oscCh[tmp0].Cpl);
	fprintf(file, "%c", ',');
	tmp0++;
 	}
 fprintf(file, "%c", '\n');
  
 // 7th line - Vertical position
 tmp0 = 0;
 while(tmp0 < NumOfChannel) {
	fprintf(file, "%i", OnScreen->oscCh[tmp0].VPos);
	fprintf(file, "%c", ',');
	tmp0++;
 	}
 fprintf(file, "%c", '\n');
  
 // 8th line - Vertical position offset
 tmp0 = 0;
 while(tmp0 < NumOfChannel) {
	fprintf(file, "%i", OnScreen->oscCh[tmp0].VPosOfs);
	fprintf(file, "%c", ',');
	tmp0++;
 	}
 fprintf(file, "%c", '\n');

 // 9th line - Sensitivity in uV
 tmp0 = 0;
 while(tmp0 < NumOfChannel) {
	fprintf(file, "%s", VSenInMicroVolt[OnScreen->oscCh[tmp0].Sen]);
	fprintf(file, "%c", ',');
	tmp0++;
 	}
 fprintf(file, "%c", '\n');

 // 10th line - Sensitivities (original)
 tmp0 = 0;
 while(tmp0 < NumOfChannel) {
	fprintf(file, "%i", OnScreen->oscCh[tmp0].SenOrig);
	fprintf(file, "%c", ',');
	tmp0++;
 	}
 fprintf(file, "%c", '\n');

 // 11th line - Reference
 tmp0 = 0;
 while(tmp0 < NumOfChannel) {
	fprintf(file, "%i", OnScreen->oscCh[tmp0].Reference);
	fprintf(file, "%c", ',');
	tmp0++;
 	}
 fprintf(file, "%c", '\n');

 // 12 - 16th reserved
 fprintf(file, "%s,%s\n", "reserved", "reserved");
 fprintf(file, "%s,%s\n", "reserved", "reserved");
 fprintf(file, "%s,%s\n", "reserved", "reserved");
 fprintf(file, "%s,%s\n", "reserved", "reserved");
 fprintf(file, "%s,%s\n", "reserved", "reserved");

 // 17th line - Data start
 // Waveform data start with 17th line
 // Write diaplayed waveform data
 if(OnScreen->oscBufSelector == 0) {
	ptmp1 = OnScreen->oscCh[0].BufB;
	ptmp2 = OnScreen->oscCh[1].BufB;
 	}
 else {
	ptmp1 = OnScreen->oscCh[0].BufA;
	ptmp2 = OnScreen->oscCh[1].BufA;
 	}
 
 tmp0 = 0;
 while(tmp0 < OnScreen->oscRecLen) {
	fprintf(file, "%i,%i\n", *ptmp1++, *ptmp2++);
	tmp0++;
 	}
 
 // Close file
 fclose(file);
 
  // Report progress
 JYEscope->Memo1->Lines->Append(filename + AnsiString(" saved"));

}

// ---------------------------------------------------------------
// Recall waveform from file. Return a pointer to 'Oscilloscope' if success. Otherwise
//	return NULL.
//
Oscilloscope	*RecallWaveFromFile(AnsiString filename)
{
 FILE *file;
 char s[1000];
 U16	tmp0;
 U8	tmp1, tmp2;
 char	*Fields[20];
 Oscilloscope *osc;
 U32	tmp3;
 sSIZE	*ptmp[NumOfChannel];
 U32	LineCount;
 U16	NumOfParameters;

 
 if(!(file = fopen(filename.c_str(), "r"))) {
 	// File open failed
 	Application->MessageBox("Can not open file.", "Message", MB_OK);
	return NULL;
	};
 JYEscope->Memo1->Lines->Append("\nOpening file ...");
 JYEscope->Memo1->Lines->Append("Filename: " + filename);

 // Create oscilloscope object
 if(!(osc = new Oscilloscope(2, 25000))) {
 	Application->MessageBox("Create waveform object failed", "Message", MB_OK);
	return NULL;
 	}
// osc->MyParent = JYEscope;		// 

 
 // Set point to file start
 fseek(file, 0, SEEK_SET);

 LineCount = 1;
 
 // 1st line
 if(!ValidLine(file, s, 1000, Fields, 6, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 if((strcmp(Fields[0], "JYDZ") != 0) ||( strcmp(Fields[1], "Waveform") != 0)) {
 	Application->MessageBox("This does not look like a valid waveform file", "Message", MB_OK);
	return NULL;
 	}
 osc->Date = Fields[2];
 osc->Time = Fields[3];
 osc->ModelName = Fields[4];
 osc->Manufacturer = Fields[5];
 JYEscope->Memo1->Lines->Append("Data type: " + AnsiString(Fields[1]));
 JYEscope->Memo1->Lines->Append("Date: " + osc->Date);
 JYEscope->Memo1->Lines->Append("Time: " + osc->Time);

 // 2nd line. Number of parameters
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, 3, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 NumOfParameters = AnsiString((char *)Fields[0]).ToInt();
 
 // 3rd line - field names of capture parameters. Skip
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, 1, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}

 // 4th line - Capture parameters
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, NumOfParameters, true)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}

 osc->oscNumOfChn = AnsiString((char *)Fields[0]).ToInt();
 if(osc->oscNumOfChn > 2) {
 	osc->oscNumOfChn = 2;
 	}
 osc->oscRecLen = AnsiString((char *)Fields[1]).ToInt();
 osc->oscChnCfg = AnsiString((char *)Fields[2]).ToInt();
 osc->SampleRate = AnsiString((char *)Fields[3]).ToInt();
 osc->Resolution = AnsiString((char *)Fields[4]).ToInt();
 osc->oscTimebase = AnsiString((char *)Fields[5]).ToInt();
 osc->oscHPos = AnsiString((char *)Fields[6]).ToInt();
 osc->oscTrigMode = AnsiString((char *)Fields[7]).ToInt();
 osc->oscTrigSlope = AnsiString((char *)Fields[8]).ToInt();
 osc->oscTrigLvl = AnsiString((char *)Fields[9]).ToInt();
 osc->oscTrigSrc = AnsiString((char *)Fields[10]).ToInt();
 osc->oscTrigPos = AnsiString((char *)Fields[11]).ToInt();
 osc->oscTrigSen = AnsiString((char *)Fields[12]).ToInt();
 osc->oscTBOrig = AnsiString((char *)Fields[13]).ToInt();

 
 // 5th line - Sensitivities
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, osc->oscNumOfChn, true)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 
 tmp0 = 0;
 while(tmp0 < osc->oscNumOfChn) {
 	osc->oscCh[tmp0].Sen = AnsiString((char *)Fields[tmp0]).ToInt();
	tmp0++;
 	}
  
 // 6th line - Coupling
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, osc->oscNumOfChn, true)) {
  	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 
 tmp0 = 0;
 while(tmp0 < osc->oscNumOfChn) {
 	osc->oscCh[tmp0].Cpl = AnsiString((char *)Fields[tmp0]).ToInt();
	tmp0++;
 	}
  
 // 7th line - Vertical position
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, osc->oscNumOfChn, true)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 
 tmp0 = 0;
 while(tmp0 < osc->oscNumOfChn) {
 	osc->oscCh[tmp0].VPos = AnsiString((char *)Fields[tmp0]).ToInt();
	tmp0++;
 	}
  
 // 8th line - Vertical position offset
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, osc->oscNumOfChn, true)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 
 tmp0 = 0;
 while(tmp0 < osc->oscNumOfChn) {
 	osc->oscCh[tmp0].VPosOfs = AnsiString((char *)Fields[tmp0]).ToInt();
	tmp0++;
 	}

 // 9th line - Sensitivity in uV - Skip
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, osc->oscNumOfChn, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}

 // 10th line - Sensitivities (original)
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, osc->oscNumOfChn, true)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 tmp0 = 0;
 while(tmp0 < osc->oscNumOfChn) {
 	osc->oscCh[tmp0].SenOrig = AnsiString((char *)Fields[tmp0]).ToInt();
	tmp0++;
 	}

 // 11th line - Reference
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields, osc->oscNumOfChn, true)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 
 tmp0 = 0;
 while(tmp0 < osc->oscNumOfChn) {
 	osc->oscCh[tmp0].Reference = AnsiString((char *)Fields[tmp0]).ToInt();
	tmp0++;
 	}

 // 12 - 16th line reserved - skip
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields,0, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields,0, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields,0, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields,0, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 LineCount++;
 if(!ValidLine(file, s, 1000, Fields,0, false)) {
 	JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
 	Application->MessageBox("Bad line found", "Message", MB_OK);
	return NULL;
 	}
 
 // Read waveform data start from 17th line
 // Prepare buffers
 // Allocate memories for waveform buffers. X 3 because data will be expanded 3 times horizontally
 if(!(osc->oscCh[0].BufA = new sSIZE[(osc->oscRecLen) + 1])) {
 	Application->MessageBox("Allocation memory failed", "Message", MB_OK);
	return NULL;
 	}

 if(!(osc->oscCh[1].BufA = new sSIZE[(osc->oscRecLen) + 1])) {
 	Application->MessageBox("Allocation memory failed", "Message", MB_OK);
	return NULL;
 	}

 // Read data from file to buffer
 tmp0 = 0;
 while(tmp0 < osc->oscNumOfChn) {
 	ptmp[tmp0] = osc->oscCh[tmp0].BufA;
	tmp0++;
 	}
 tmp3 = 0;
 while(tmp3 < osc->oscRecLen) {
 	LineCount++;
	 if(!ValidLine(file, s, 1000, Fields, osc->oscNumOfChn, true)) {
 		JYEscope->Memo1->Lines->Append("Bad line: " + IntToStr(LineCount));
	 	Application->MessageBox("Bad line found", "Message", MB_OK);
		return NULL;
	 	}
	 tmp0 = 0;
	 while(tmp0 < osc->oscNumOfChn) {
	 	*ptmp[tmp0]++ = AnsiString((char *)Fields[tmp0]).ToInt();
		tmp0++;
	 	}
	 tmp3++;
 	}
 
 // Close file
 fclose(file);

 // Copy data to Dup buffer for possible transformation
 // First set BufA (which contains the newly loaded data) to be the "back" buffer.
 //	This is because transformations onle be done to inactive buffer. The active buffer is for capture.
 //	Data from file are always loaded to BufA
 osc->oscBufSelector = 1;

 // Allocate memory for the Dup buffers
 if(!(osc->oscCh[0].BufDupA = new sSIZE[(osc->oscRecLen) + 1])) {
 	Application->MessageBox("Allocation memory failed", "Message", MB_OK);
	return NULL;
 	}

 if(!(osc->oscCh[1].BufDupA = new sSIZE[(osc->oscRecLen) + 1])) {
 	Application->MessageBox("Allocation memory failed", "Message", MB_OK);
	return NULL;
 	}
 
 JYEscope->Memo1->Lines->Append("Data load successful\n");

 // Copy data
 Transform(osc, Trans_Copy);
 
 return osc;
}

// --------------------------------------------------------------------------------
// Parse the 'line'. Replace ',' with '\0' to make each field a string. Assign their addresses to the members of 'fields' in turn
//	'fieldnum' is the required minimum fields
//
bool	LineParser(char * line, char **fields, U16 fieldnum, bool checkdigit) 
{
 U16 tmp0, tmp1, tmp2, tmp3;
 bool newlinefound;
 char ch;
 
 tmp0 = 0;
 tmp1 = 0;
 tmp2 = 0;
 tmp3 = 0;
 newlinefound = false;	// Indicator for '\n' found. 
 
 fields[tmp3] = line;	// The first string
 tmp3++;
 while((ch = line[tmp0]) != 0) {
 	if(ch == '\n') {
		// The line terminates
		if(tmp3 < fieldnum) {
			// Not enough fields
		 	JYEscope->Memo1->Lines->Append("Not enough fields");
			return false;
			}
		// Otherwise a good line
		newlinefound = true; 	// '\n' found
		break; 
 		}
	if(ch == ',') {
		line[tmp0] = 0;	// This is the end of one field
		tmp0++;			// Point to the start of next field
		fields[tmp3] = &line[tmp0]; // Point to the start of next field
		tmp3++;			// Next member of fields
		}
	else {
		if(checkdigit && (!isdigit(ch) && (ch != '-') && (ch != '+')) ) {
			// Non-digit found
		 	JYEscope->Memo1->Lines->Append("Non-digit character found");
			return false;
			}
		tmp0++;
		}
	
 	}
 line[tmp0] = 0;	// The end of last found field
 if(newlinefound) {
 	return true;
 	}
 else {
 	return false;
 	}
}

// --------------------------------------------------------------------------------
// Transform waveform data from original buffer to duplicated buffer with required 'method'.
//	The orignal buffer is determined by the buffer selector in the object.
//
void	Transform(Oscilloscope *osc, U16 method) 
{
 U32 tmp0, tmp1;
 sSIZE *Orig0, *Orig1, *Dup0, *Dup1;
 double ddtmp1, ddtmp2;
 float	ftmp, ftmp1;
 sSIZE tmp2, tmp3;

 // Select buffer A or B
 if(osc->oscBufSelector == 0) {
	// Select buffer B (Note: 0 for A, 1 for B. The selector indicates the buffer used for active data capture. So use the other buffer for dispaly)
	Orig0 = osc->oscCh[0].BufB;
	Orig1 = osc->oscCh[1].BufB;
 	}
 else {
	// Buffer A
	Orig0 = osc->oscCh[0].BufA;
	Orig1 = osc->oscCh[1].BufA;
 	}
 Dup0 = osc->oscCh[0].BufDupA;
 Dup1 = osc->oscCh[1].BufDupA;
 
 switch(method) {
	case Trans_Vertical:
		// Do horizontal transform even for vertical transform request. 
		//	This ensures data are from original buffer every time so as original
		//	waveform can be shown when transform scale is 1.
	case Trans_Horizon:
		if(osc->oscTimebase > osc->oscTBOrig) {
			// Only do copy (using original timebase)
			ddtmp1 = AnsiString(TBinNanoSecond[osc->oscTBOrig]).ToDouble();
			}
		else {
			ddtmp1 = AnsiString(TBinNanoSecond[osc->oscTimebase]).ToDouble();
			}
		ddtmp2 = AnsiString(TBinNanoSecond[osc->oscTBOrig]).ToDouble();
		ftmp = ddtmp1/ddtmp2;
// 		JYEscope->Memo1->Lines->Append(FloatToStrF(ftmp, ffFixed, 3, 1));

		if(ftmp != 2.5) {
			// Regular
			tmp1 = osc->oscRecLen/ftmp;
			tmp0 = 0;
			while(tmp0 < tmp1) {
				*Dup0++ = *Orig0;
				*Dup1++ = *Orig1;
				Orig0 += (U16)ftmp;
				Orig1 += (U16)ftmp;
				tmp0++;
				}
			
			// Fill the rest with avarage 
			tmp1 = osc->oscRecLen - tmp1;
			tmp0 = 0;
			while(tmp0 < tmp1) {
				*Dup0++ = osc->oscCh[0].Average;
				*Dup1++ = osc->oscCh[1].Average;
				tmp0++;
				}
			}
		else {
			// Special processing for step size of 2.5
			tmp1 = osc->oscRecLen/ftmp;
			tmp1 /= 2;
			tmp0 = 0;
			while(tmp0 < tmp1) {
				*Dup0++ = *Orig0;
				*Dup1++ = *Orig1;
				tmp2 = *(Orig0 + 2);
				tmp3 = *(Orig0 + 3);
				*Dup0++ = (tmp2 + tmp3)/2;
				tmp2 = *(Orig1 + 2);
				tmp3 = *(Orig1 + 3);
				*Dup1++ = (tmp2 + tmp3)/2;
				Orig0 += 5;
				Orig1 += 5;
				tmp0++;
				}
			
			// Fill the rest with avarage 
			tmp1 = osc->oscRecLen - 2 * tmp1;
			tmp0 = 0;
			while(tmp0 < tmp1) {
				*Dup0++ = osc->oscCh[0].Average;
				*Dup1++ = osc->oscCh[1].Average;
				tmp0++;
				}
			}

		// Vertical transform if requested
//		if(method == Trans_Vertical) {
			 Dup0 = osc->oscCh[0].BufDupA;
			 Dup1 = osc->oscCh[1].BufDupA;
//			 tmp2 = osc->oscCh[0].VPosOfs;
//			 tmp3 = osc->oscCh[1].VPosOfs;
			tmp2 = osc->oscCh[0].Reference;
			tmp3 = osc->oscCh[1].Reference;
			// Determine transform factor for Ch 1 and Ch2
			ddtmp1 = AnsiString(VSenInMicroVolt[osc->oscCh[0].Sen]).ToDouble();
			ddtmp2 = AnsiString(VSenInMicroVolt[osc->oscCh[0].SenOrig]).ToDouble();
			ftmp = ddtmp1/ddtmp2;
			ddtmp1 = AnsiString(VSenInMicroVolt[osc->oscCh[1].Sen]).ToDouble();
			ddtmp2 = AnsiString(VSenInMicroVolt[osc->oscCh[1].SenOrig]).ToDouble();
			ftmp1 = ddtmp1/ddtmp2;
			tmp0 = 0;
			while(tmp0 < osc->oscRecLen) {
//				*Dup0 = (sSIZE)((float)(*Dup0 + tmp2 - V0)/ftmp) + V0 - tmp2;
//				*Dup1 = (sSIZE)((float)(*Dup1 + tmp3 - V0)/ftmp1) + V0 - tmp3;
				*Dup0 = (sSIZE)((float)(*Dup0 - tmp2)/ftmp) + tmp2;
				*Dup1 = (sSIZE)((float)(*Dup1 - tmp3)/ftmp1) + tmp3;
				Dup0++;
				Dup1++;
				tmp0++;
				}
//			}
		break;

	case Trans_Copy:
		tmp0 = 0;
		while(tmp0 < osc->oscRecLen) {
			*Dup0++ = *Orig0++;
			*Dup1++ = *Orig1++;
			tmp0++;
			}
	default:
		break;
 	}

}

void	BackupWaveform(Oscilloscope *osc)
{
 osc->oscCh[0].SenOrig = osc->oscCh[0].Sen;
 osc->oscCh[1].SenOrig = osc->oscCh[1].Sen;
 osc->oscTBOrig = osc->oscTimebase;
 Transform(osc, Trans_Copy);
}

void RepaintScreen(void)
{
/*
 if(Capturer->oscState == OS_Running) {
 	Capturer->oscState = OS_Stopped;
 	}
*/
 VSenRefresh();
 CplRefresh();
 VPosRefresh();
 TBRefresh();
 HPosRefresh();
 TrigModeRefresh();
 TrigSlopeRefresh();
 TrigSrcRefresh();
 TrigLvlRefresh();
 TrigPosRefresh();
 TrigSenRefresh();
 CursorT_Refresh();
 CursorV_Refresh();
 OscStateRefresh();
 RepaintWave(OnScreen);
}
//---------------------------------------------------------------------------

void ConnectSuccessful(void)
{
 ConnectState = 1;
 JYEscope->UnitState->Caption = "Connected";
 JYEscope->btnConnect->Caption = "Disconnect";
 if(FrontEndType != 'G') {
 	JYEscope->btnRunStop->Enabled = true;
 	JYEscope->CaptureRun->Enabled = true;
 	}
 else {
 	JYEscope->btnRunStop->Enabled = false;
 	JYEscope->CaptureRun->Enabled = false;
 	}
 
 JYEscope->CaptureConnect->Enabled = false;
 JYEscope->CaptureDisconnect->Enabled = true;
 JYEscope->CaptureStop->Enabled = false;
 
 JYEscope->DeviceConnectionInd->Picture = DeviceConnected->Picture;
 Capturer->SetOscState(OS_Stopped);
 JYEscope->btnDispCapture->Enabled = false;
 JYEscope->CaptureDisplay->Enabled = false;
 JYEscope->FileClose->Enabled = false;
 if(Recall[CurrRecallIndex] != NULL) {
	 // Recall is currently in display. Uncheck the button
	 Recall[CurrRecallIndex]->Btn->Checked = false;			
	}

 // Switch to capture
 OnScreen = Capturer;
 RepaintScreen();
}
//---------------------------------------------------------------------------

void ConnectFailed(void)
{
 ConnectState = 0;
 JYEscope->UnitState->Caption = "Disconnected";
 JYEscope->btnConnect->Caption = "Connect";
 JYEscope->btnRunStop->Enabled = false;
 
 JYEscope->CaptureConnect->Enabled = true;
 JYEscope->CaptureDisconnect->Enabled = false;
 JYEscope->CaptureRun->Enabled = false;
 JYEscope->CaptureStop->Enabled = false;
 
 JYEscope->DeviceConnectionInd->Picture = DeviceDisconnected->Picture;
 JYEscope->UnitName->Caption = "N/A";
 Capturer->SetOscState(OS_Offline);

 // Restore configuration to uninitialized state
 OnScreen->Restore();
 
// OscStateRefresh();
 // Transform waveform to Duplicated buffer because the duplicated buffer is displayed when capture is not in running state.
 BackupWaveform(Capturer);
 RepaintScreen();
}

void DisplayRecall(void)
{
 Capturer->oscState = OS_Stopped;
 // Enable file close action
 JYEscope->FileClose->Enabled = true;
 JYEscope->btnDispCapture->Enabled = true;
 JYEscope->CaptureDisplay->Enabled = true;
 if(ConnectState) {
	 if(FrontEndType == 'O') {
	 	JYEscope->CaptureRun->Enabled = true;
	 	}
	 else {
	 	JYEscope->CaptureRun->Enabled = false;
	 	}
	JYEscope->CaptureStop->Enabled = false;
 	}
 else {
	JYEscope->CaptureRun->Enabled = false;
	JYEscope->CaptureStop->Enabled = false;
 	}
 RepaintScreen();
}
//---------------------------------------------------------------------------

bool IsInt(const char *str)
{
 // Get length of string to check and prepare to loop
 U32 length = strlen(str);
 U32 tmp = 0;
	
 // Loop until we've gone through all the characters
 while(tmp< length) {
	// If the current character is not a digit (i.e. 0-9), then the string can't be an integer so return false
	if(!isdigit(str[tmp])) {
		return false;
		}
	tmp++;
     	}
	// If no problems have been found then the string must be an integer
	return true;
}

//---------------------------------------------------------------------------
// Read a line from 'file' to 'str' with maximun length 'len'. Return true if successful. Return false in one of
//	following cases:
//		1 ) fgets() returns NULL
//		2 ) EOF is encountered
//		3 ) The line does not end with '\n' (like the line is longer than 'len')
//		4 ) Fields in the line is less than the required 'fieldnum'
//		5 ) Non-digit characters are found in a field
//
bool ValidLine(FILE *file, char *str, U16 len, char **fields, U16 fieldnum, bool checkdigit)
{
 // Read one line
 if(!fgets(str, len, file)) {
 	// Read error or empty line
	JYEscope->Memo1->Lines->Append("Line read error or empty line");
 	return false;
 	}

 // Check for EOF
 if(feof(file)) {
 	// End-of-file encountered 
 	//fgets() set the end-of-file indicator associated with the file if EOF is reached in a read attemp.
	JYEscope->Memo1->Lines->Append("Premetured EOF found");
 	return false;
 	}

 // Parse the line
 if(!LineParser(str, fields, fieldnum, checkdigit))
 	{
	// Bad line
	return false;
 	}
}

void	DSO068_Setup(void)
{
 // Disable Sen and Cpl adjustment
 JYEscope->Ch1->Enabled = false;
 JYEscope->Ch2->Enabled = false;
 
 OnScreen->Ch1SenEnabled = false;
 OnScreen->Ch1CplEnabled = false;
 OnScreen->Ch2SenEnabled = false;
 OnScreen->Ch2CplEnabled = false;

 // Disable trigger source adjustment
 JYEscope->cbTriggerSrc->Enabled = false;
 OnScreen->TrigSrcEnabled = false;

 // Disable trigger sensitivity adjustment
 JYEscope->edTrigSen->Enabled = false;

 // Disable some timebase adjustment
 JYEscope->rbTB01us->Enabled = false;
 JYEscope->rbTB02us->Enabled = false;
 JYEscope->rbTBcustom->Enabled = false;
 
// JYEscope->lbCh1Sen->Enabled = false;
// JYEscope->lbCh2Sen->Enabled = false;
// JYEscope->lbTrigSrcCurr->Enabled = false;

// JYEscope->Ch2->Color = clGrayText;
}

void	DSO_CmdProc068(void)
{
 U16 tmp0, tmp1, tmp2, tmp3;
 sSIZE	*ptmp0, *ptmp1, *ptmp2;

 switch(FrameBuf[3]) {
 	case CMD_DSO_IsUSBscope:
		// Send GetConfig command
		CommandBuf[0] = FID_DSO;
		*(U16 *)&CommandBuf[1] = 0x0004;
		CommandBuf[3] = CMD_DSO_GetConfig;
		TxProc(CommandBuf, *(U16 *)&CommandBuf[1]);

		break;
		
 	case CMD_DSO_SendParam:
//		JYEscope->Memo1->Lines->Append("Parameter frame received");
		OnScreen->SetVSen(Channel_1, (S16)FrameBuf[4]);
		OnScreen->SetCpl(Channel_1, (S16)FrameBuf[5]);
 		RepaintScreen();
//		VSenRefresh();
//		CplRefresh();
		break;
		
 	case CMD_DSO_SendBlock:
//		JYEscope->Memo1->Lines->Append("Data block received");
		if(Capturer->oscNumOfChn < 1) {
			// No channel enabled
			return;
			}
		
		 if(Capturer->oscState != OS_Running) {
		 	break;
		 	}
	
		// Sample length
		tmp0 = *(U16 *)&FrameBuf[1] - 6;
		
		// Always set SampleStart to 0 for FlashMode data
		Capturer->oscSampleStart = 0;		

		if(Capturer->oscBufSelector == 0) {
			// Select Buffer B
			ptmp1 = Capturer->oscCh[0].BufB;
			ptmp2 = Capturer->oscCh[1].BufB;
			Capturer->oscBufSelector = 1;
			}
		else {
			// Select Buffer A
			ptmp1 = Capturer->oscCh[0].BufA;
			ptmp2 = Capturer->oscCh[1].BufA;
			Capturer->oscBufSelector = 0;
			}
		

		if(Capturer->oscNumOfChn == 2) {
			// There are two channels. Load channel 2 data
			tmp3 = (U16)FrameBuf[4];
			tmp1 = tmp0/2 + 1;
			tmp2 = 2;
			while(tmp1) {
				tmp0 = tmp3;						// Current sample
				tmp3 = (U16)FrameBuf[tmp2 + 4];	// Next sample
				// 1-for-3 Linear interpopulation
				// Vertically multiply by 3 since the pixel density of waveform window is 3 times higher than screen on front end device
				*ptmp2++ = 3 * tmp0;				// 
				*ptmp2++ = 2 * tmp0 + tmp3;
				*ptmp2++ = tmp0 + 2 * tmp3;
				tmp2 += 2;
				tmp1--;
				}
			}


		// Sample length
		tmp0 = *(U16 *)&FrameBuf[1] - 6;
		if(Capturer->oscNumOfChn == 2) {
			// Two channels
			// Save channel 1 data
			tmp1 = tmp0/2 + 1;
			tmp3 = (U16)FrameBuf[5];
			tmp2 = 2;
			while(tmp1) {
				tmp0 = tmp3;						// Current sample
				tmp3 = (U16)FrameBuf[tmp2 + 5];	// Next sample
				// 1-for-3 Linear interpopulation
				// Vertically multiply by 3 since the pixel density of waveform window is 3 times higher than screen on front end device
				*ptmp1++ = 3 * tmp0;				// 
				*ptmp1++ = 2 * tmp0 + tmp3;
				*ptmp1++ = tmp0 + 2 * tmp3;
				tmp2 += 2;
				tmp1--;
				}
			
			}
		else {
			tmp1 = tmp0;
			// Save channel 1 data
			tmp3 = (U16)FrameBuf[4];
			tmp2 = 1;
			while(tmp1) {
				tmp0 = tmp3;						// Current sample
				tmp3 = (U16)FrameBuf[tmp2 + 4];	// Next sample
				// 1-for-3 Linear interpopulation
				// Vertically multiply by 3 since the pixel density of waveform window is 3 times higher than screen on front end device
				*ptmp1++ = 3 * tmp0;				// 
				*ptmp1++ = 2 * tmp0 + tmp3;
				*ptmp1++ = tmp0 + 2 * tmp3;
				tmp2 += 1;
				tmp1--;
				}
			
			}
		
		if(Capturer->oscTrigMode == TM_Single) {
			// Enter Stopped state
			Capturer->oscState = OS_Stopped;
			OscStateRefresh();
			BackupWaveform(Capturer);
/*
			// Make the just received data be displayed
			if(Capturer->oscBufSelector == 0) {
				// Select Buffer B
				Capturer->oscBufSelector = 1;
				}
			else {
				// Select Buffer A
				Capturer->oscBufSelector = 0;
				}
*/			
// 			RepaintScreen();
			}
		
		SetEvent(WaveformUpdateEvent);

//		RepaintScreen();
		break;

	case CMD_DSO_SendSample:
		if(Capturer->oscState != OS_Running) {
		 	break;
		 	}
		 
		if(Capturer->oscBufSelector == 1) {
			// Use Buffer A
			ptmp1 = Capturer->oscCh[0].BufA;
			ptmp2 = Capturer->oscCh[1].BufA;
			}
		else {
			// Use Buffer B
			ptmp1 = Capturer->oscCh[0].BufB;
			ptmp2 = Capturer->oscCh[1].BufB;
			}

		// Move samples forward to make room for new sample at the end
		tmp0 = 0;
		while(tmp0 < Capturer->oscRecLen - 3) {
			*ptmp1 = *(ptmp1 + 3);
			*ptmp2 = *(ptmp2 + 3);
			ptmp1++;
			ptmp2++;
			tmp0++;
			}

		// Interpopulation
		// The last samples.
		tmp0 = *(ptmp1 + 2)/3;	
		tmp1 = *(ptmp2 + 2)/3;
		// New samples
		tmp2 = FrameBuf[4];		// Ch 1
		tmp3 = FrameBuf[6];		// Ch 2

		// Insert the first sample
		*ptmp1++ = 2 * tmp0 + tmp2;
		*ptmp2++ = 2 * tmp1 + tmp3;
		// Insert the second sample
		*ptmp1++ = tmp0 + 2 * tmp2;
		*ptmp2++ = tmp1 + 2 * tmp3;
		// Insert the third sample, i.e. the newly captured sample
		*ptmp1  = 3 * tmp2;
		*ptmp2  = 3 * tmp3;
		
		SetEvent(WaveformUpdateEvent);
		
// 		RepaintScreen();

		break;
		
	case CMD_DSO_SendConfig:
		// 
		OnScreen->oscNumOfChn = FrameBuf[4];
		switch(OnScreen->oscNumOfChn) {
			case 1:
			case 2:
				OnScreen->oscChnCfg = 0x01;
				break;
				
			case 3:
				OnScreen->oscChnCfg = 0x03;
			case 0:
			default:
				break;

			}
		
		
		OnScreen->Ch1SenEnabled = BitTest(FrameBuf[5], 0x01);
 		OnScreen->Ch1CplEnabled = BitTest(FrameBuf[5], 0x02);
		OnScreen->Ch2SenEnabled = BitTest(FrameBuf[5], 0x10);
 		OnScreen->Ch2CplEnabled = BitTest(FrameBuf[5], 0x20);
		
		OnScreen->TrigSrcEnabled = BitTest(FrameBuf[6], 0x01);
 		OnScreen->TrigSenEnabled = BitTest(FrameBuf[6], 0x02);

		if(!OnScreen->Ch1SenEnabled) {
			JYEscope->Ch1->Enabled = false;
			}
		if(!OnScreen->Ch2SenEnabled) {
			JYEscope->Ch2->Enabled = false;
			}

		OnScreen->VSenMax = FrameBuf[8];
		OnScreen->VSenMin = FrameBuf[9];
		OnScreen->CplMax = FrameBuf[10];
		OnScreen->CplMin = FrameBuf[11];
		// VPos is not relevent to device operation so can be ignored
//		OnScreen->VPosMax = *(S16 *)&FrameBuf[12];
//		OnScreen->VPosMin = *(S16 *)&FrameBuf[14];

		OnScreen->TB_Max = FrameBuf[24];
		OnScreen->TB_Min = FrameBuf[25];
		
		OnScreen->TrigModeMax = FrameBuf[30];
		OnScreen->TrigModeMin = FrameBuf[31];
		OnScreen->TrigSlopeMax = FrameBuf[32];
		OnScreen->TrigSlopeMin = FrameBuf[33];

		// Keep trigger level limits independent of device.
		//	Only make level modification at transferring it to device
		//	This will allow jyeLab use the same range for all devices
//		OnScreen->TrigLvlMax = *(S16 *)&FrameBuf[34];
//		OnScreen->TrigLvlMin = *(S16 *)&FrameBuf[36];

		OnScreen->TrigPosMax = FrameBuf[38];
		OnScreen->TrigPosMin = FrameBuf[39];
		OnScreen->TrigSrcMax = FrameBuf[40];
		OnScreen->TrigSrcMin = FrameBuf[41];
		OnScreen->TrigSenMax = *(S16 *)&FrameBuf[42];
		OnScreen->TrigSenMin = *(S16 *)&FrameBuf[44];

		OnScreen->RecLenMax = (*(S16 *)&FrameBuf[46] * 3);
		OnScreen->RecLenMin = (*(S16 *)&FrameBuf[50] * 3);

		// Make sure parameters are in range
		OnScreen->SetVSen(Channel_1, OnScreen->GetVSen(Channel_1));
		OnScreen->SetCpl(Channel_1, OnScreen->GetCpl(Channel_1));
		OnScreen->SetVPos(Channel_1, OnScreen->GetVPos(Channel_1));
		OnScreen->SetVSen(Channel_2, OnScreen->GetVSen(Channel_2));
		OnScreen->SetCpl(Channel_2, OnScreen->GetCpl(Channel_2));
		OnScreen->SetVPos(Channel_2, OnScreen->GetVPos(Channel_2));

		OnScreen->SetTimeBase(OnScreen->GetTimeBase());
		
		OnScreen->SetTrigMode(OnScreen->GetTrigMode());
		OnScreen->SetTrigSlope(OnScreen->GetTrigSlope());
		OnScreen->SetTrigLvl(OnScreen->GetTrigLvl());
		OnScreen->SetTrigPos(OnScreen->GetTrigPos());
		OnScreen->SetTrigSrc(OnScreen->GetTrigSrc());
		OnScreen->SetTrigSen(OnScreen->GetTrigSen());

		OnScreen->SetRecLen(OnScreen->GetRecLen());
		
		JYEscope->Memo1->Lines->Append("Configuration successful");

		// Set valid record length
		JYEscope->cbRecordLength->Items->Text = "768\n1536\n3072";
 		JYEscope->cbRecordLength->ItemIndex = 0;
		
		// Send parameters
		SetParam();
 		RepaintScreen();
	default:
		break;

 	}
}

void	SetParam(void)
{
 CommandBuf[0] = FID_DSO;
 *(U16 *)&CommandBuf[1] = 30;
 CommandBuf[3] = CMD_DSO_SetParam;

 CommandBuf[4] = OnScreen->oscCh[0].Sen;
 CommandBuf[5] = OnScreen->oscCh[0].Cpl;
 *(S16 *)&CommandBuf[6] = OnScreen->oscCh[0].VPos;
 CommandBuf[8] = OnScreen->oscCh[1].Sen;
 CommandBuf[9] = OnScreen->oscCh[1].Cpl;
 *(S16 *)&CommandBuf[10] = OnScreen->oscCh[1].VPos;

 CommandBuf[12] = OnScreen->oscTimebase;

 *(S16 *)&CommandBuf[14] = OnScreen->oscHPos;

 CommandBuf[16] = OnScreen->oscTrigMode;
 CommandBuf[17] = OnScreen->oscTrigSlope;
 *(S16 *)&CommandBuf[18] = (OnScreen->oscTrigLvl - (V0 - OnScreen->oscCh[0].Reference))/3 + 128;
 CommandBuf[20] = OnScreen->oscTrigPos;
 CommandBuf[21] = OnScreen->oscTrigSrc;
 *(S16 *)&CommandBuf[22] = OnScreen->oscTrigSen;

 *(S16 *)&CommandBuf[24] = OnScreen->oscRecLen/3;
 
 TxProc(CommandBuf, *(U16 *)&CommandBuf[1]);

// JYEscope->Memo1->Lines->Append("Timebase = " + IntToStr(CommandBuf[12]));

}

void	SendParamToDevice(void)
{

 if(OnScreen->Model == DSO_Model_094) {
 	Command(FID_DSOparam);
 	}
 if(OnScreen->Model == DSO_Model_068) {
 	SetParam();
 	}
 
}

