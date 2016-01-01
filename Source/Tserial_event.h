#include "Common.h"

/* ------------------------------------------------------------------------ --
--                                                                          --
--                        PC serial port connection object                  --
--                           for  event-driven programs                     --
--                                                                          --
--                                                                          --
--                                                                          --
--  Copyright @ 2001-2002     Thierry Schneider                             --
--                            thierry@tetraedre.com                         --
--                                                                          --
--                                                                          --
--                                                                          --
-- ------------------------------------------------------------------------ --
--                                                                          --
--  Filename : Tserial_event.cpp                                            --
--  Author   : Thierry Schneider                                            --
--  Created  : April 4th 2000                                               --
--  Modified : June 22nd 2002                                               --
--  Plateform: Windows 95, 98, NT, 2000, XP (Win32)                         --
-- ------------------------------------------------------------------------ --
--                                                                          --
--  This software is given without any warranty. It can be distributed      --
--  free of charge as long as this header remains, unchanged.               --
--                                                                          --
-- ------------------------------------------------------------------------ --
--                                                                          --
-- 01.04.24      Comments added                                             --
-- 01.04.28      Bug 010427 corrected. OnDisconnectedManager was not        --
--                initialized                                               --
-- 01.04.28      connect() function prototype modified to handle 7-bit      --
--                communication                                             --
-- 01.04.29      "ready" field added to remove a bug that occured during    --
--                 reconnect (event manager pointers cleared)               --
--                 I removed the "delete" in Tserial_event_thread_start     --
--                 because it was destroying the object even if we would    --
--                 use it again                                             --
--                                                                          --
-- 02.01.30      Version 2.0 of the serial event object                     --
--                                                                          --
--                                                                          --
-- 02.06.22      - wait for the thread termination before                   --
--                 quiting or restarting                                    --
--               - "owner" field added to be able to call C++ object from   --
--                  the event manager routine                               --
--               - Correction of a bug that occured when receiving data     --
--                 (setting twice the SIG_READ_DONE event)                  --
--                                                                          --
--                                                                          --
--                                                                          --
-- ------------------------------------------------------------------------ --
--                                                                          --
--    Note to Visual C++ users:  Don't forget to compile with the           --
--     "Multithreaded" option in your project settings                      --
--                                                                          --
--         See   Project settings                                           --
--                   |                                                      --
--                   *--- C/C++                                             --
--                          |                                               --
--                          *--- Code generation                            --
--                                       |                                  --
--                                       *---- Use run-time library         --
--                                                     |                    --
--                                                     *---- Multithreaded  --
--                                                                          --
--                                                                          --
--                                                                          --
-- ------------------------------------------------------------------------ */


#ifndef TSERIAL_EVENT_H
#define TSERIAL_EVENT_H

#include <stdio.h>
#include <windows.h>


#define SERIAL_PARITY_NONE 	0
#define SERIAL_PARITY_ODD  		1
#define SERIAL_PARITY_EVEN 		2

#define 	SERIAL_CONNECTED         	0
#define 	SERIAL_DISCONNECTED    1
#define 	SERIAL_DATA_SENT         	2
#define 	SERIAL_DATA_ARRIVAL    3
#define 	SERIAL_RING              	4
#define 	SERIAL_CD_ON            	5
#define 	SERIAL_CD_OFF            	6
#define	SERIAL_DATA_TOSEND	7

#define SIG_POWER_DOWN     		0
#define SIG_READER         			1
#define SIG_READ_DONE      		2    // data received has been read
#define SIG_WRITER         			3
#define SIG_DATA_TO_TX     		4    // data waiting to be sent
#define SIG_MODEM_EVENTS   		5
#define SIG_MODEM_CHECKED  	6

#define	TxBufSize				6000
#define	RxBufSize				6000

typedef unsigned long uint32;
typedef void (*type_myCallBack) (uint32 object, uint32 event);
//typedef void (*type_myCallBack) (Tserial_event object, uint32 event);

#ifndef __BORLANDC__
#define bool  BOOL 
#define true  TRUE
#define false FALSE
#endif


#define SERIAL_SIGNAL_NBR 		7         	// number of events in the thread
#define SERIAL_MAX_RX     		256       	// Input buffer max size
#define SERIAL_MAX_TX     		256      	// output buffer max size
//#define SERIAL_MAX_RX     		512       	// Input buffer max size
//#define SERIAL_MAX_TX     		512       	// output buffer max size

// ===================================================
// Frame level comm constants
//
#define	FrameSizeMax		16384		// in bytes

// Frame IDs
#define	FID_Status			0xF0	// For query sent to slave (M48) for status
#define	FID_Keycode		0xF1	// Keycode frame from Uart0
#define	FID_Query			0xE0	// 
#define	FID_Connect			0xE1	//
#define	FID_Confirm			0xE2	//
#define	FID_SetSerial		0xE3	// Send serial port paramters
#define	FID_DSOparam		0xE4	// Frame to send/set DSO parameters
#define	FID_DSOparamOL	0xE5	// Offline DSO parameters
#define	FID_Disconnect		0xE9	//
#define	FID_Loopback		0xEF	//
#define	FID_Diag			0xFF	// For diagnostic
#define	FID_DataFM			0xD0	// FlashMode data
#define	FID_DataRM			0xD1	// RollingMode data
#define	FID_DataOL			0xD2	// Offline waveform data
#define	FID_DSO			0xC0	// DSO command set
#define	FID_WriteRequest	0x11	//
#define	FID_WriteReturn		0x13	// Write return
#define	FID_WriteWaveform	0x14	// Write waveform data to FG085
#define	FID_ReadWaveform 	0x16	// Read saved waveform

/*
#define	DsoCmd_EnterVdso		0xE0
#define	DsoCmd_GetParam		0xE3
#define	DsoCmd_SetParam		0xE4
#define	DsoCmd_ExitVdso		0xE9
#define	DsoCmd_Test			0xEB
#define	DsoCmd_SetTimebase	0xA1	
#define	DsoCmd_SetTrigger		0xA2
#define	DsoCmd_SetChannel		0xA3
#define	DsoCmd_Hold			0xED
#define	DsoCmd_Run			0xEE
*/

enum CommStatus {
	CS_Mode				= 0x80,	// 0: ASC mode, 1: Binary mode
	CS_SynChFound			= 0x40,
	CS_Rxing				= 0x20,
	CS_CmdBufFull			= 0x10,
	CS_LoopbackTest		= 0x08,	// 1: low level loop back for testing
	};

#define	SynCh				0xFE			// SYNC characters
#define	EOL					0x0a			// End of line

typedef	struct {
	U8	Baudrate[8];
	U8	Databit[2];
	U8	Stopbit[2];
	U8	Parity[2];
	U8	Status;
	U8	*RxHead;
	U8	*RxTail;
	U8	*TxHead;
	U8	*TxTail;
	U8	*RxQueueStart;
	U8	*TxQueueStart;
	U8	*RxFrameBufStart;
	U8	*TxFramebufStart;
	U16	RxFrameIndex;
	U16	TxFrameIndex;
	U16	RxQueueSize;
	U16	TxQueueSize;
	U16	RxFrameBufSize;
	U16	TxFrameBufSize;
	void	(*FrameProc)(void);	// Function for fram processing
	void	(*hook)(void);		// Function for special handlings
	}COMM;

/* -------------------------------------------------------------------- */
/* -----------------------------  Tserial  ---------------------------- */
/* -------------------------------------------------------------------- */
class Tserial_event
{
    // -------------------------------------------------------- //
//protected:
public:
    bool          		ready;
    bool          		check_modem;
    char          		port[10];                         	// port name "com1",...
    long          		baudrate;                          // baudrate
    int			databit;
    int			stopbit;
    int           		paritymode;

    HANDLE        	serial_events[SERIAL_SIGNAL_NBR];  	// events to wait on
    unsigned int  	threadid;                         	// ...
    HANDLE        	serial_handle;                  	// ...
    HANDLE        	thread_handle;                	// ...
    OVERLAPPED    	ovReader;                         	// Overlapped structure for ReadFile
    OVERLAPPED    	ovWriter;                          	// Overlapped structure for WriteFile
    OVERLAPPED    	ovWaitEvent;                   	// Overlapped structure for WaitCommEvent
    char          		tx_in_progress;               	// BOOL indicating if a WriteFile is in progress
    char          		rx_in_progress;              	// BOOL indicating if a ReadFile is in progress
    char          		WaitCommEventInProgress;
    U8          		rxBuffer[SERIAL_MAX_RX];
    unsigned long    	max_rx_size;
    unsigned long    	received_size;
    U8          		txBuffer[SERIAL_MAX_TX];
    int           		tx_size;
    DWORD         	dwCommEvent;                       // to store the result of the wait

    type_myCallBack manager;

    // ............................................................
    void          OnCharArrival    (char c);
    void          OnEvent          (unsigned long events);


    // ++++++++++++++++++++++++++++++++++++++++++++++
    // .................. EXTERNAL VIEW .............
    // ++++++++++++++++++++++++++++++++++++++++++++++
public:
                  	Tserial_event();
                 	~Tserial_event();
    	void  	*owner;                // do what you want with this
    	void        run          (void);
	int  		Tserial_event::connect (char *port_arg, bool modem_events);	

    	void        setManager       (type_myCallBack manager);
    	void        setRxSize        (int size);
    	void        sendData         (char *buffer, int size);
    	int           getNbrOfBytes    (void);
    	int           getDataInSize    (void);
    	char *     getDataInBuffer  (void);
    	void        dataHasBeenRead  (void);
    	void        disconnect       (void);
	};

/* -------------------------------------------------------------------- */

void SerialEventManager(uint32 object, uint32 event);
char		*PortAvailable(char *portname);
void	InitTxBuf(void);
void	InitRxBuf(void);
U8	TxBufEmpty(void);
U8	RxBufEmpty(void);
U8	AddToTxBuf(U8 *Item);
U8	DeleteFromTxBuf(U8 *Item);
U8	AddToRxBuf(U8 *Item);
U8	DeleteFromRxBuf(U8 *Item);
void	CommInit(void);
void	RxProc(void);
U32	TxProc(U8 *buf, U32 size);
U8	PortValid(HANDLE porthandle);
U8	IsFrame(U8 FrameID);
void	FrameProc(void);
void	Command(U8 cmdID);
void AppendToMemo(TMemo* Memo, LPCSTR text);

void	RxQueueInit(COMM *comm, U8 *rxqueue, U16 rxqueuesize);
void	TxQueueInit(COMM *comm, U8 *txqueue, U16 txqueuesize);
U8	RxQueueAdd(COMM *comm,  U8 *byte);
U8	RxQueueDelete(COMM *comm, U8 *byte);
U8	RxQueueEmpty(COMM *comm);
U8	TxQueueAdd(COMM *comm,  U8 *byte);
U8	TxQueueDelete(COMM *comm, U8 *byte);
U8	TxQueueEmpty(COMM *comm);
void	CommInit(void);
void CommDummy(void);
void	CommRx(COMM *comm);

extern 	Tserial_event *serial_port;
extern	U8	FrameBuf[];
extern	char	CommandBuf[];
extern	COMM MyComm;
extern	U8	TxBuf[]; 

#endif TSERIAL_EVENT_H


