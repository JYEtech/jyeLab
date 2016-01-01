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

typedef unsigned (WINAPI *PBEGINTHREADEX_THREADFUNC) (LPVOID lpThreadParameter);
typedef unsigned *PBEGINTHREADEX_THREADID;

//---------------------------------------------------------------------------
//#pragma package(smart_init)
//#pragma resource "*.dfm"
//TJYEscope *JYEscope;

//Oscilloscope *jyScope;

extern	Graphics::TBitmap *GridBackground;
extern	Graphics::TBitmap *Waveform;
extern	Graphics::TBitmap *PaneWaveform;
extern	Graphics::TBitmap *PaneReadout;
extern	TImage *Ch1Pos;
extern	TImage *Ch2Pos;


Oscilloscope::Oscilloscope(U16 channels, U32 recordlength)
{

	oscCh[0].Sen = VS_02V;
	oscCh[0].Cpl = CP_DC;
	oscCh[0].VPos = 30;
	oscCh[0].Average = V0;
	oscCh[0].Reference = V0;
	oscCh[1].Sen = VS_2V;
	oscCh[1].Cpl = CP_AC;
	oscCh[1].VPos = -30;
	oscCh[1].Average = V0;
	oscCh[1].Reference = V0;
	oscTimebase = TB_10us;
	oscRecLen = recordlength;
	oscHPos = oscRecLen/2 - WWindowSizex/2;
	oscTrigMode = TM_Normal;
	oscTrigSlope = TE_Falling;
	oscTrigSrc = TS_Ch1;
	oscTrigPos = 50;
	oscTrigLvl = 45;

	oscState = OS_Offline;
	oscNumOfChn = 2;
	oscCh[0].VPosOfs = 0;
	oscCh[1].VPosOfs = 0;

	SampleRate = 1000000L;
	Resolution = 8;
	
	oscSampleStart = 0;
	oscBufSelector = 0;

	oscFlags = 0x0000;
	
	oscCh[0].BufA = (sSIZE *)NULL;
	oscCh[0].BufB = (sSIZE *)NULL;
	oscCh[1].BufA = (sSIZE *)NULL;
	oscCh[1].BufB = (sSIZE *)NULL;
	oscCh[0].BufDupA = (sSIZE *)NULL;
	oscCh[0].BufDupB = (sSIZE *)NULL;
	oscCh[1].BufDupA = (sSIZE *)NULL;
	oscCh[1].BufDupB = (sSIZE *)NULL;

	oscCursorT1 = oscHPos + 100;
	oscCursorT2 = oscHPos + 200;
	oscCh1Vcursor1 = -50;
	oscCh1Vcursor2 = 50;
	oscCh2Vcursor1 = -50;
	oscCh2Vcursor2 = 50;

	Restore();
		
	ModelName = "N/A";
	Manufacturer = "JYE Tech Ltd.";
}

Oscilloscope::~Oscilloscope(void)
{
 if(oscCh[0].BufA != NULL) {
 	delete oscCh[0].BufA;
 	}
 if(oscCh[0].BufB!= NULL) {
 	delete oscCh[0].BufB;
 	}
 if(oscCh[0].BufDupA != NULL) {
 	delete oscCh[0].BufDupA;
 	}
 if(oscCh[0].BufDupB!= NULL) {
 	delete oscCh[0].BufDupB;
 	}
 
 if(oscCh[1].BufA != NULL) {
 	delete oscCh[1].BufA;
 	}
 if(oscCh[1].BufB!= NULL) {
 	delete oscCh[1].BufB;
 	}
 if(oscCh[1].BufDupA != NULL) {
 	delete oscCh[1].BufDupA;
 	}
 if(oscCh[1].BufDupB!= NULL) {
 	delete oscCh[1].BufDupB;
 	}
}

void 	Oscilloscope::Restore(void)
{
  	Model = 0;		// No device connected
	
	oscNumOfChn = 2;
	
	oscChnCfg = CF_Ch1Ch2;

	Ch1SenEnabled = true;
	Ch1CplEnabled = true;
	Ch2SenEnabled = true;
	Ch2CplEnabled = true;

	JYEscope->Ch1->Enabled = true;
	JYEscope->Ch2->Enabled = true;

	TrigSrcEnabled = true;
	TrigSenEnabled = true;

	VSenMax = VSenMax0;
	VSenMin = VSenMin0;
	VPosOfsMax = VPosOfsMax0;
	VPosOfsMin = VPosOfsMin0;
	CplMax = CplMax0;
	CplMin = CplMin0;
	VPosMax = VPosMax0;
	VPosMin = VPosMin0;
	TB_Max = TimeBaseMax0;
	TB_Min = TimeBaseMin0;

	TrigModeMax = TrigModeMax0;
	TrigModeMin = TrigModeMin0;

// Time slope limits
	TrigSlopeMax = TrigSlopeMax0;
	TrigSlopeMin = TrigSlopeMin0;

// Time source limits
	TrigSrcMax = TrigSrcMax0;
	TrigSrcMin = TrigSrcMin0;

// VPos limits
	TrigLvlMax = TrigLvlMax0;
	TrigLvlMin = TrigLvlMin0;

	TrigPosMax = TrigPosMax0;
	TrigPosMin = TrigPosMin0;

	TrigSenMax = TrigSenMax0;
	TrigSenMin = TrigSenMin0;

	RefMax = RefMax0;
	RefMin = RefMin0;

	RecLenMax = RecLenMax0;
	RecLenMin = RecLenMin0;
}


S16 	Oscilloscope::GetVSen(U16 chn)
{
 if(chn == Channel_1) {
 	return oscCh[0].Sen;
 	}

 if(chn == Channel_2) {
 	return oscCh[1].Sen;
 	}
 return -1;
}

S16 	Oscilloscope::SetVSen(U16 chn, S16 vsen)
{
 if(chn == Channel_1) {
	if(vsen > VSenMax) {
		vsen = VSenMax;
		}
	if(vsen < VSenMin) {
		vsen= VSenMin;
		}
 	oscCh[0].Sen = vsen;
 	return oscCh[0].Sen;
 	}

 if(chn == Channel_2) {
	if(vsen > VSenMax) {
		vsen = VSenMax;
		}
	if(vsen < VSenMin) {
		vsen= VSenMin;
		}
 	oscCh[1].Sen = vsen;
 	return oscCh[1].Sen;
 	}
 return -1;

}

S16	Oscilloscope::GetCpl(U16 chn)
{
 if(chn == Channel_1) {
 	return oscCh[0].Cpl;
 	}

 if(chn == Channel_2) {
 	return oscCh[1].Cpl;
 	}
 return -1;
}

S16	Oscilloscope::SetCpl(U16 chn, S16 cpl)
{
 if(chn == Channel_1) {
	if(cpl > CplMax) {
		cpl = CplMax;
		}
	if(cpl < CplMin) {
		cpl= CplMin;
		}
 	oscCh[0].Cpl = cpl;
 	return oscCh[0].Cpl;
 	}

 if(chn == Channel_2) {
	if(cpl > CplMax) {
		cpl = CplMax;
		}
	if(cpl < CplMin) {
		cpl= CplMin;
		}
 	oscCh[1].Cpl = cpl;
 	return oscCh[1].Cpl;
 	}
 return -1;

}

S16		Oscilloscope::GetVPos(U16 chn)
{
 if(chn == Channel_1) {
 	return oscCh[0].VPos;
 	}

 if(chn == Channel_2) {
 	return oscCh[1].VPos;
 	}
 return -1;
}

S16		Oscilloscope::SetVPos(U16 chn, S16 vpos)
{
// SetCpl(U16 chn, U16 cpl)
 if(chn == Channel_1) {
 	if(vpos < VPosMin) {
		vpos = VPosMin;
 		}
 	if(vpos > VPosMax) {
		vpos = VPosMax;
 		}
 	oscCh[0].VPos = vpos;
	return oscCh[0].VPos;
 	}

 if(chn == Channel_2) {
 	if(vpos < VPosMin) {
		vpos = VPosMin;
 		}
 	if(vpos > VPosMax) {
		vpos = VPosMax;
 		}
 	oscCh[1].VPos = vpos;
	return oscCh[1].VPos;
 	}
  
  return -1;

}

S16		Oscilloscope::SetRef(U16 chn, S16 ref)
{
 if(chn == Channel_1) {
 	if(ref < RefMin) {
		ref = RefMin;
 		}
 	if(ref > RefMax) {
		ref = RefMax;
 		}
 	oscCh[0].Reference = ref;
	return oscCh[0].Reference;
 	}

 if(chn == Channel_2) {
 	if(ref < RefMin) {
		ref = RefMin;
 		}
 	if(ref > RefMax) {
		ref = RefMax;
 		}
 	oscCh[1].Reference= ref;
	return oscCh[1].Reference;
 	}
  
 return -1;
}

S16		Oscilloscope::GetVPosOfs(U16 chn)
{
 if(chn == Channel_1) {
 	return oscCh[0].VPosOfs;
 	}

 if(chn == Channel_2) {
 	return oscCh[1].VPosOfs;
 	}
 return -1;
}

S16		Oscilloscope::SetVPosOfs(U16 chn, S16 vposofs)
{
 if(chn == Channel_1) {
 	if(vposofs < VPosOfsMin) {
		vposofs = VPosOfsMin;
 		}
 	if(vposofs > VPosOfsMax) {
		vposofs = VPosOfsMax;
 		}
 	oscCh[0].VPosOfs = vposofs;
	return oscCh[0].VPosOfs;
 	}

 if(chn == Channel_2) {
 	if(vposofs < VPosOfsMin) {
		vposofs = VPosOfsMin;
 		}
 	if(vposofs > VPosOfsMax) {
		vposofs = VPosOfsMax;
 		}
 	oscCh[1].VPosOfs = vposofs;
	return oscCh[1].VPosOfs;
 	}
  
  return -1;

}

S16 	Oscilloscope::GetTimeBase(void)
{
 return oscTimebase;
}

S16	Oscilloscope::SetTimeBase(S16 timebase)
{
 if(timebase < TB_Min) {
 	timebase = TB_Min;
 	}

 if(timebase > TB_Max) {
 	timebase = TB_Max;
 	}
 oscTimebase = timebase;
 return oscTimebase;
}

S32		Oscilloscope::GetHPos(void)
{
 return oscHPos;
}

S32		Oscilloscope::SetHPos(S32 hpos)
{
 if(hpos < 0) {
 	hpos = 0;
 	}
 if(oscRecLen > WWindowSizex) {
	 if(hpos > (oscRecLen - WWindowSizex)) {
	 	hpos = oscRecLen - WWindowSizex;
	 	}
 	}
 else {
 	hpos = 0;
 	}
 oscHPos = hpos;
 return oscHPos;
}

S16	Oscilloscope::GetTrigMode(void)
{
 return oscTrigMode;
}

S16	Oscilloscope::SetTrigMode(S16 trigmode)
{
 if(trigmode < TrigModeMin) {
 	trigmode = TrigModeMin;
 	}

 if(trigmode > TrigModeMax) {
 	trigmode = TrigModeMax;
 	}
 oscTrigMode = trigmode;
 return oscTrigMode;
}

S16	Oscilloscope::GetTrigSlope(void)
{
 return oscTrigSlope;
}

S16	Oscilloscope::SetTrigSlope(S16 trigslope)
{
 if(trigslope < TrigSlopeMin) {
 	trigslope = TrigSlopeMin;
 	}

 if(trigslope > TrigSlopeMax) {
 	trigslope = TrigSlopeMax;
 	}
 oscTrigSlope = trigslope;
 return oscTrigSlope;
}

S16		Oscilloscope::GetTrigSrc(void)
{
 return oscTrigSrc;
}

S16		Oscilloscope::SetTrigSrc(S16 trigsrc)
{
 if(trigsrc < TrigSrcMin) {
 	trigsrc = TrigSrcMin;
 	}

 if(trigsrc > TrigSrcMax) {
 	trigsrc = TrigSrcMax;
 	}
 oscTrigSrc = trigsrc;
 return oscTrigSrc;
}

S16	Oscilloscope::GetTrigPos(void)
{
 return oscTrigPos;
}

S16	Oscilloscope::SetTrigPos(S16 pretrig)
{
 if(pretrig < TrigPosMin) {
	pretrig = TrigPosMin;
 	}
 if(pretrig > TrigPosMax) {
	pretrig = TrigPosMax;
 	}
 oscTrigPos = pretrig;
 return oscTrigPos;
}

S16			Oscilloscope::GetTrigLvl(void)
{
 return oscTrigLvl;
}

S16			Oscilloscope::SetTrigLvl(S16 triglvl)
{
 if(triglvl < TrigLvlMin) {
	triglvl = TrigLvlMin;
 	}
 if(triglvl > TrigLvlMax) {
	triglvl = TrigLvlMax;
 	}
 oscTrigLvl = triglvl;
 return oscTrigLvl;
}

S16	Oscilloscope::GetTrigSen(void)
{
 return oscTrigSen;
}

S16	Oscilloscope::SetTrigSen(S16 trigsen)
{
 if(trigsen < TrigSenMin) {
	trigsen = TrigSenMin;
 	}
 if(trigsen > TrigSenMax) {
	trigsen = TrigSenMax;
 	}
 oscTrigSen = trigsen;
 return oscTrigSen;
}

U32			Oscilloscope::GetRecLen(void)
{
 return oscRecLen;
}
	
U32			Oscilloscope::SetRecLen(U32 reclen)
{
 if(reclen < RecLenMin) {
	reclen = RecLenMin;
 	}
 if(reclen > RecLenMax) {
	reclen = RecLenMax;
 	}
 oscRecLen = reclen;
 return oscRecLen;

}

U16		Oscilloscope::GetOscState(void)
{
 return oscState;
}
	
U16		Oscilloscope::SetOscState(U16 oscstate)
{
// if(oscstate > OS_Stopped) {
//	oscstate = OS_Waiting;
// 	}
 oscState = oscstate;
 return oscState;

}

S16	Oscilloscope::SetCursorT1(S16 cursor)
{
 if(cursor < 0) {
 	cursor = 0;
 	}
 if(cursor >= oscRecLen) {
	cursor = oscRecLen - 1;
 	}
 oscCursorT1 = cursor;
 return oscCursorT1;
}

S16	Oscilloscope::SetCursorT2(S16 cursor)
{
 if(cursor < 0) {
 	cursor = 0;
 	}
 if(cursor >= oscRecLen) {
	cursor = oscRecLen - 1;
 	}
 oscCursorT2 = cursor;
 return oscCursorT2;
}

S16	Oscilloscope::SetCursorV1(U16 chn, S16 cursor)
{
 if(cursor < VPosMin) {
 	cursor = VPosMin;
 	}
 if(cursor > VPosMax) {
	cursor = VPosMax;
 	}
 if(chn == Channel_1) {
 	oscCh1Vcursor1 = cursor;
 	}
 if(chn == Channel_2) {
 	oscCh2Vcursor1 = cursor;
 	}
 return cursor;
}

S16	Oscilloscope::SetCursorV2(U16 chn, S16 cursor)
{
 if(cursor < VPosMin) {
 	cursor = VPosMin;
 	}
 if(cursor > VPosMax) {
	cursor = VPosMax;
 	}
 if(chn == Channel_1) {
 	oscCh1Vcursor2 = cursor;
 	}
 if(chn == Channel_2) {
 	oscCh2Vcursor2 = cursor;
 	}
 return cursor;
}

