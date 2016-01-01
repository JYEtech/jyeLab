//---------------------------------------------------------------------------

#ifndef ScopeH
#define ScopeH
//---------------------------------------------------------------------------

#include "Common.h"

//extern 	Oscilloscope *jyScope;

#define	ModeThreshold	TB_50ms

#define	NumOfChannel			2
// Number of parameters to write to waveform file
#define	NumOfParam			14

#define	Channel_1				1
#define	Channel_2				2

// ============= Limit constants ==================
// VPos limits
#define	VPosMax0				256
#define	VPosMin0				-256
#define	VPosOfsMax0			256
#define	VPosOfsMin0				-256

#define	RefMax0					255 * 3
#define	RefMin0					0

// VSen limits
#define	VSenMax0				VS_10mV
#define	VSenMin0				VS_5V

// Cpl limits
#define	CplMax0					CP_GND
#define	CplMin0					CP_DC

// Timebase limits
#define	TimeBaseMax0			TB_02us
#define	TimeBaseMin0			TB_10m

// Time mode limits
#define	TrigModeMax0			TM_Single
#define	TrigModeMin0			TM_Auto

// Time slope limits
#define	TrigSlopeMax0			TE_Rising
#define	TrigSlopeMin0			TE_Falling

// Time source limits
#define	TrigSrcMax0				TS_Ch2
#define	TrigSrcMin0				TS_Ch1

// VPos limits
#define	TrigLvlMax0				(768/2)		//256
#define	TrigLvlMin0				(-768/2)		//256
	
#define	TrigPosMax0				100
#define	TrigPosMin0				0

#define	TrigSenMax0			40
#define	TrigSenMin0				2

//#define	RecLenMax				16384
#define	RecLenMax0				24000
#define	RecLenMin0				720

// DSO model codes
enum	DSO_Model_Code {
	DSO_Model_None	= 0x00,
	DSO_Model_082 		= 0x02,
	DSO_Model_094		= 0x03,
	DSO_Model_068		= 0x04,

	};

// -- Trig Mode type
enum 	TrigModes {
	TM_Auto,
	TM_Normal,
	TM_Single,
	TM_Last
	};

// -- Time base type
enum 	TimeBases {
	TB_Custom	= 0,
	TB_50m,
	TB_20m,
	TB_10m,	//		60s
	TB_5m,		//		30s
	TB_2m,		//		12s
	TB_1m,		//		6s
	TB_50s,		//		5s
	TB_20s,		// 		2s
	TB_10s,		// 1Hz	1s
	TB_5s,		// 2Hz	500ms
	TB_2s,		// 5Hz	200ms
	TB_1s,		// 10Hz	100ms
	TB_05s,		// 20Hz	50ms
	TB_02s,		// 50Hz	20ms
	TB_01s,		// 100Hz	10ms
	TB_50ms,	// 200Hz	5ms
	TB_20ms,	// 500Hz	2ms
	TB_10ms,	// 1K	1ms
	TB_5ms,		// 2K	500us
	TB_2ms,		// 5K	200us
	TB_1ms,		// 10K	100us
	TB_05ms,	// 20K	50us
	TB_02ms,	// 50K	20us (400 cycles)
	TB_01ms,	// 100K	10us (200 cycles)
	TB_50us,	// 200K	100 cycles
	TB_20us,	// 500K	40 cycles
	TB_10us,	// 1M	20 cycles
	TB_5us,		// 2M	10 cycles
	TB_2us,		// 5M	
	TB_1us,		// 10M
	TB_05us,	// 20M
	TB_02us,	// 50M
	TB_01us,
	TB_50ns,
	TB_20ns,
	TB_10ns,
	TB_5ns,
	TB_max
	};

// -- Trig Edge type
enum 	TrigSlopes  {
	TE_Falling,
	TE_Rising,
	};

// -- V sensitivity type
enum 	VerticalSensitivity {
	VS_Custom	= 0,
	VS_100V,
	VS_50V,
	VS_20V,
	VS_10V,
	VS_5V,
	VS_2V,
	VS_1V,
	VS_05V,
	VS_02V,
	VS_01V,
	VS_50mV,
	VS_20mV,
	VS_10mV,
	VS_5mV,
	VS_2mV,
	VS_1mV,
	VS_Last
	};

// -- Coupling type
enum 	Coupling {
	CP_DC = 0,
	CP_AC,
	CP_GND
	};

enum 	TirgSrc  {
	TS_Ch1 = 0,
	TS_Ch2,
	TS_External,
	TS_TrigOut		// take this place to hold trig output setting
	};

/*
typedef 	enum 	 {
	TP_0,
	TP_1_8,
	TP_2_8,
	TP_3_8,
	TP_4_8,
	TP_5_8,
	TP_6_8,
	TP_7_8,
	TP_8_8
	}t_TrigPos;
*/

// -- Channel configuration type
enum 	ChnConfig	{
	CF_Reserved = 0,
	CF_Ch1				= 0x01,
	CF_Ch2				= 0x02,
	CF_Ch1Ch2			= 0x03
	};

enum	oscFlags {
	OF_TrigLvlLineOn					= 0x0001,
	OF_TrigLvlLineAlwaysOn				= 0x0002,
	OF_TrigPointLineOn					= 0x0004,
	OF_TrigPointLineAlwaysOn			= 0x0008,
	OF_CursorT_On						= 0x0010,
	OF_CursorT_Tied_On					= 0x0020,
	OF_CursorV1_On						= 0x0040,	// Ch1 delta V cursor
	OF_CursorV2_On						= 0x0080,	// Ch2 delta V cursor	
	};

enum 	 OscState {
	OS_Stopped,
	OS_Running,
	OS_Offline,
	OS_Recall
	};

enum TransformMethod {
	Trans_Copy,
	Trans_Vertical,
	Trans_Horizon,
	};

typedef	struct {
	S16			Sen;
	S16			SenOrig;
	S16			Cpl;
	S16			VPos;
	S16			VPosOfs;
	S16			Average;
	S16			Reference;
	sSIZE		*BufA;		// Primary buffer 
	sSIZE		*BufB;		// Secondary buffer
	sSIZE		*BufDupA;	// 
	sSIZE		*BufDupB;	//

}	CHANNEL;
class Oscilloscope  {
public:
	CHANNEL		oscCh[NumOfChannel];
	// -- About capture data
//	U32			oscCh1SenuV;	// Sensitivity in uV
//	U32			oscCh2SenuV:

	// -- About capture configuration
	U16			oscNumOfChn;
	U32			oscRecLen;
	U16			oscChnCfg;
	U32			SampleRate;
	U16			Resolution;
	S16			oscTimebase;
	U16			oscTBOrig;
	U16			oscHPos;
	S16			oscTrigMode;
	S16			oscTrigSlope;
	S16			oscTrigLvl;
	S16			oscTrigSrc;
	S16			oscTrigPos;
	S16			oscTrigSen;
	
	U32			oscSampleStart;
	
	
	U16			oscSysMode;
	U16			oscBufSelector;	// Select active buffer. 0: BufA, 1: BufB. The selected buffer is used for capturing live data

	U16			oscState;		// 0 - stopped, 1 - running
	U16			oscFlags;

	S16			oscCursorT1;
	S16			oscCursorT2;
	S16			oscCh1Vcursor1;
	S16			oscCh1Vcursor2;
	S16			oscCh2Vcursor1;
	S16			oscCh2Vcursor2;

	AnsiString	Caption;
	U16			RecallIndex;		// 
	void			*MyParent;

	U16			Model;
	bool			Ch1SenEnabled;
	bool			Ch1CplEnabled;
	bool			Ch2SenEnabled;
	bool			Ch2CplEnabled;
	bool			TrigSrcEnabled;
	bool			TrigSenEnabled;

	S16			VSenMax;
	S16			VSenMin;
	S16			VPosOfsMax;
	S16			VPosOfsMin;
	S16			CplMax;
	S16			CplMin;
	S16			VPosMax;
	S16			VPosMin;
	S16			TB_Max;
	S16			TB_Min;

	S16			TrigModeMax	;
	S16			TrigModeMin;

// Time slope limits
	S16			TrigSlopeMax;
	S16			TrigSlopeMin;

// Time source limits
	S16			TrigSrcMax;
	S16			TrigSrcMin;

// VPos limits
	S16			TrigLvlMax;
	S16			TrigLvlMin;

	S16			TrigPosMax;
	S16			TrigPosMin;

	S16			TrigSenMax;
	S16			TrigSenMin;

	S16			RefMax;
	S16			RefMin;

	S32			RecLenMax;
	S32			RecLenMin;
	
// Metadata
	AnsiString	ModelName;
	AnsiString	Manufacturer;
	AnsiString	Time;
	AnsiString	Date;
	
	
// Member functions
Oscilloscope(U16 channels, U32 recordlength);
~Oscilloscope(void);
void 	Restore(void);
S16 		GetVSen(U16 chn);
S16 		SetVSen(U16 chn, S16 vsen);
S16		GetCpl(U16 chn);
S16		SetCpl(U16 chn, S16 cpl);
S16		GetVPos(U16 chn);
S16		SetVPos(U16 chn, S16 vpos);
S16		SetRef(U16 chn, S16 ref);
S16		GetVPosOfs(U16 chn);
S16		SetVPosOfs(U16 chn, S16 vposofs);
S16 		GetTimeBase(void);
S16		SetTimeBase(S16 timebase);
S32		GetHPos(void);
S32		SetHPos(S32 hpos);
S16		GetTrigMode(void);
S16		SetTrigMode(S16 trigmode);
S16		GetTrigSlope(void);
S16		SetTrigSlope(S16 trigslope);
S16		GetTrigSrc(void);
S16		SetTrigSrc(S16 trigsrc);
S16		GetTrigPos(void);
S16		SetTrigPos(S16 pretrig);
S16		GetTrigLvl(void);
S16		SetTrigLvl(S16 triglvl);	
S16		GetTrigSen(void);
S16		SetTrigSen(S16 trigsen);
U32		GetRecLen(void);
U32		SetRecLen(U32 reclen);
U16		GetOscState(void);
U16		SetOscState(U16 oscstate);
S16		SetCursorT1(S16 cursor);
S16		SetCursorT2(S16 cursor);
S16		SetCursorV1(U16 chn, S16 cursor);
S16		SetCursorV2(U16 chn, S16 cursor);

protected:

private:
};

//extern	Oscilloscope *jyScope;

//void __fastcall ShowWave(void);


#endif
