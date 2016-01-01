#include <vcl.h>
#pragma hdrstop

#include "JYEscopeMain.h"
#include "Common.h"

#define  STRICT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <conio.h>
#include <windows.h>


#include "Tserial_event.h"
#include "JYEscopeMain.h"
#include "PortSetup.h"


Tserial_event *serial_port;

U8	TxBuf[TxBufSize]; 
U8	RxBuf[RxBufSize];

//U8	*TxHead, *TxTail;
//U8	*RxHead, *RxTail;

U8	Rbuf[100];
U8	FrameBuf[20000];
//U32	FrameIndex;

//bool	SynFound;
//bool	SynChSent;
//bool	Rxing;
//bool	Txing;

char	CommandBuf[100];

//U32	TxCnt, RxCnt;

//char InsertPoint = 0;
/*
WaveCapture	CaptureBuffers[MaxCaptureBufferNumber];
WaveCapture	*Pool;
WaveCapture	*CQ_Head, *CQ_Tail;
*/

COMM MyComm;


void	RxQueueInit(COMM *comm, U8 *rxqueue, U16 rxqueuesize)
{
 comm->RxQueueStart = rxqueue;
 comm->RxQueueSize = rxqueuesize;
 comm->RxHead = rxqueue;
 comm->RxTail = rxqueue;
}

void	TxQueueInit(COMM *comm, U8 *txqueue, U16 txqueuesize)
{
 comm->TxQueueStart = txqueue;
 comm->TxQueueSize = txqueuesize;
 comm->TxHead = txqueue;
 comm->TxTail = txqueue;
}

// ========================================================
// Insert one character to RxQueue. Returns non-zero if success,
//	otherwise returns 0.
// 
U8	RxQueueAdd(COMM *comm,  U8 *byte)
{
 U8	*ptmp;
 
 // Store the at queue end
 *comm->RxTail = *byte;	

 // Advance pointer
 if((comm->RxTail + 1) >= (comm->RxQueueStart + comm->RxQueueSize)) {
 	// Roll over
 	ptmp = comm->RxQueueStart;
 	}
 else {
 	ptmp = (U8 *)(comm->RxTail + 1);
 	}

 if(ptmp == comm->RxHead) {
 	// Queue full. Insertion failed
 	return 0;
 	}
 else {
 	// Successful
 	comm->RxTail = ptmp;
	return 1;
 	}
}

// ========================================================
// Delet one character from RxQueue and save it to location pointed by "byte". 
//	Returns non-zero if success, otherwise returns 0.
// 

U8	RxQueueDelete(COMM *comm, U8 *byte)
{
 if(comm->RxHead == comm->RxTail) {
 	// Queue empty
 	return 0;
 	}
 // Retrieve one byte from queue
 *byte = *comm->RxHead;
 // Advance pointer
 if(comm->RxHead + 1 >= comm->RxQueueStart + comm->RxQueueSize) {
 	// Roll over
 	comm->RxHead = comm->RxQueueStart;
 	}
 else {
 	comm->RxHead++;
 	}
 return 1;
 
}

U8	RxQueueEmpty(COMM *comm)
{
 return (comm->RxHead == comm->RxTail);
}

// ========================================================
// Insert one character to RxQueue. Returns non-zero if success,
//	otherwise returns 0.
// 
U8	TxQueueAdd(COMM *comm,  U8 *byte)
{
 U8	*ptmp;

 // Store the at queue end
 *comm->TxTail = *byte;	

 // Advance pointer
 if((comm->TxTail + 1) >= (comm->TxQueueStart + comm->TxQueueSize)) {
 	// Roll over
 	ptmp = comm->TxQueueStart;
 	}
 else {
 	ptmp = (U8 *)(comm->TxTail + 1);
 	}

 if(ptmp == comm->TxHead) {
 	// Queue full. Insertion failed
 	return 0;
 	}
 else {
 	// Successful
 	comm->TxTail = ptmp;
	return 1;
 	}
}

// ========================================================
// Delet one character from RxQueue and save it to location pointed by "byte". 
//	Returns non-zero if success, otherwise returns 0.
// 

U8	TxQueueDelete(COMM *comm, U8 *byte)
{
 if(comm->TxHead == comm->TxTail) {
 	// Queue empty
 	return 0;
 	}
 // Retrieve one byte from queue
 *byte = *comm->TxHead;
 // Advance pointer
 if(comm->TxHead + 1 >= comm->TxQueueStart + comm->TxQueueSize) {
 	// Roll over
 	comm->TxHead = comm->TxQueueStart;
 	}
 else {
 	comm->TxHead++;
 	}
 return 1;
 
}

U8	TxQueueEmpty(COMM *comm)
{
 return (comm->TxHead == comm->TxTail);
}

void	CommInit(void)
{
 // Uart0 initialization
 RxQueueInit(&MyComm, RxBuf, RxBufSize);
 TxQueueInit(&MyComm, TxBuf, TxBufSize);
 MyComm.RxFrameBufStart= FrameBuf;
 MyComm.RxFrameBufSize = 20000;
 MyComm.RxFrameIndex = 0;
 MyComm.FrameProc = FrameProc;
 MyComm.hook = CommDummy;
 MyComm.Status = 0;


}

void CommDummy(void)
{

}


void	CommRx(COMM *comm)
{
 U8	tmp0;

 // Check and get a byte from RxQueue
 while(RxQueueDelete(comm, &tmp0)) {

 	if(BitTest(comm->Status, CS_LoopbackTest)) {
		// Loopback for testing
//		u0putc(tmp0);
		continue;
		}
 
	// Check for SynCh
	if(tmp0 == SynCh) {
		// Mark it if SycCh found
		BitSet(comm->Status, CS_SynChFound);
		continue;
		}

 	if(BitTest(comm->Status, CS_SynChFound)) {
		// Process the byte following SynCh
		BitClr(comm->Status, CS_SynChFound);	// Clear SynCh_Found flag first
		// The previous byte was SynCh. If Rxing is valid and the byte following SynCh is 0x00
		//	then the SynCh was a data byte. Otherwise check for frame ID. 
		if((BitTest(comm->Status, CS_Rxing)) && (tmp0 == 0x00)) {
			// Frame receiving in process and the current byte is 0x00. Save SynCh as data
			*(comm->RxFrameBufStart + comm->RxFrameIndex) = SynCh;
			comm->RxFrameIndex++;
			// Go to check for frame end
			goto FrameEndCheck;
			}
		else {
			// Rxing not valid or the byte following SynCh is not 0x00.
			// Check for frame ID
			if(IsFrame(tmp0)) {
				// Initialize a frame
				*(comm->RxFrameBufStart + 0) = tmp0;
				*(comm->RxFrameBufStart + 2) = 0xFF;	// Make sure correct frame length check (byte1 & byte 2 are frame length)
				comm->RxFrameIndex = 1;
				BitSet(comm->Status, CS_Rxing);
				}
			else {
				// Clear Rxing anyway. This is to prevent error at Rxing being valid but
				//	SynCh is not followed by frame ID or 0x00.
				BitClr(comm->Status, CS_Rxing);
				}
			}
 		}
	else {
		// Normal byte processing. Save bytes if Rxing is valid
		if(BitTest(comm->Status, CS_Rxing)) {
			*(comm->RxFrameBufStart + comm->RxFrameIndex) = tmp0;
			comm->RxFrameIndex++;
			FrameEndCheck:
//			if(comm->RxFrameIndex == 4) {
//				JYEscope->Memo1->Lines->Append("Frame size: " + IntToStr(*(U16 *)(comm->RxFrameBufStart + 1)));
//				}
			if((comm->RxFrameIndex >= *(U16 *)(comm->RxFrameBufStart + 1)) || (comm->RxFrameIndex > comm->RxFrameBufSize)) {
				// Execute frame processing function
				(*comm->FrameProc)();
//				FrameProc0();
				BitClr(comm->Status, CS_Rxing);
				}
			}
 		}

 	}

}

void Tserial_event_thread_start(void *arg);

typedef unsigned (WINAPI *PBEGINTHREADEX_THREADFUNC) (LPVOID lpThreadParameter);
typedef unsigned *PBEGINTHREADEX_THREADID;

/* ---------------------------------------------------------------------- */
/* ---------------------  Tserial_event_thread_start  ------------------- */
/* ---------------------------------------------------------------------- */
/**
    This function is not part of the Tserial_event object. It is simply used
    to start the thread from an external point of the object.
*/
void Tserial_event_thread_start(void *arg)
{
    class Tserial_event *serial_unit;

    serial_unit = (Tserial_event *) arg;
    
    if (serial_unit!=0)
        serial_unit->run();
}

/* -------------------------------------------------------------------- */
/* -------------------------    Tserial_event ------------------------- */
/* -------------------------------------------------------------------- */
Tserial_event::Tserial_event()
{
 int tmp;

 ready            	= false;
 baudrate           	= 115200;
// baudrate			= fmPortSetup->cbBaudrate->Items->Strings[fmPortSetup->cbBaudrate->ItemIndex].ToInt(); 
 databit			= 8;
 stopbit			= ONESTOPBIT;	// 0 for one stop bit
 paritymode       	= SERIAL_PARITY_NONE;
 port[0]          	= 0;
 threadid         	= 0;
 serial_handle    	= INVALID_HANDLE_VALUE;
 thread_handle   	= 0;
 owner            	= 0;
 tx_in_progress  	= 0;
 rx_in_progress 	= 0;
 max_rx_size     	= 1;
 tx_size          	= 0;
 received_size    	= 0;
 check_modem  	= false;

 manager          	= 0;

    /* -------------------------------------------------------------- */
    // creating Events for the different sources
 tmp = 0;
 while(tmp < SERIAL_SIGNAL_NBR) {
 	if ((tmp == SIG_READER) || (tmp == SIG_WRITER) || (tmp == SIG_MODEM_EVENTS)) {
		serial_events[tmp] = CreateEvent(NULL, TRUE, FALSE, NULL);  // Manual Reset
 		}
     	else {
		serial_events[tmp] = CreateEvent(NULL, FALSE, FALSE, NULL); // Auto reset
     		}
	tmp++;
    }
 
}

/* -------------------------------------------------------------------- */
/* --------------------------    ~Tserial_event ----------------------- */
/* -------------------------------------------------------------------- */
Tserial_event::~Tserial_event()
{
 int i;

// JYEscope->Memo1->Lines->Append(AnsiString("\nTserial destructor reached"));

 if (thread_handle!=0) {
 	WaitForSingleObject(thread_handle, 2000);
 	}
 thread_handle = 0;

//---------------------------------------------
// deleting the events

 for (i=0; i<SERIAL_SIGNAL_NBR; i++) {
 	if (serial_events[i] != INVALID_HANDLE_VALUE) {
		CloseHandle(serial_events[i]);
 		}
	serial_events[i] = INVALID_HANDLE_VALUE;
 	}

 if (serial_handle != INVALID_HANDLE_VALUE) {
 	CloseHandle(serial_handle);
 	}
 serial_handle = INVALID_HANDLE_VALUE;
 
}


/* -------------------------------------------------------------------- */
/* --------------------------    disconnect   ------------------------- */
/* -------------------------------------------------------------------- */
void Tserial_event::disconnect(void)
{
// while(tx_in_progress) {
//	}
	
 ready = false;
 SetEvent(serial_events[SIG_POWER_DOWN]);

 if (thread_handle!=0) {
	WaitForSingleObject(thread_handle, 2000);
 	}
 thread_handle = 0;
}

/* -------------------------------------------------------------------- */
/* --------------------------    connect      ------------------------- */
/* -------------------------------------------------------------------- */
/**
     Serial port, file and overlapped structures initialization
*/
int  Tserial_event::connect (char *port_arg, bool modem_events)
{
    int  error;
    DCB  dcb;
    int  i;
    bool hard_handshake;
    COMMTIMEOUTS cto = { 0, 0, 0, 0, 0 };
//    COMMTIMEOUTS cto = { MAXDWORD, 0, 0, 0, 0 };

    /* --------------------------------------------- */
 if (serial_handle != INVALID_HANDLE_VALUE) {
	CloseHandle(serial_handle);
 	}
 serial_handle = INVALID_HANDLE_VALUE;

 if (port_arg !=0) {
    	strncpy(port, port_arg, 10);
//       rate        = rate_arg;
//       parityMode  = parity_arg;
       check_modem = modem_events;

       error      = 0;
       ZeroMemory(&ovReader, sizeof(ovReader));  	// clearing the overlapped
       ZeroMemory(&ovWriter, sizeof(ovWriter));
       ZeroMemory(&ovWaitEvent, sizeof(ovWaitEvent));
       memset(&dcb, 0, sizeof(dcb));

       /* -------------------------------------------------------------------- */
       // set DCB to configure the serial port
       dcb.DCBlength       = sizeof(dcb);                   
        
       /* ---------- Serial Port Config ------- */
// 	dcb.BaudRate = fmPortSetup->cbBaudrate->Items->Strings[fmPortSetup->cbBaudrate->ItemIndex].ToInt(); 
       dcb.BaudRate        = baudrate;

       switch(paritymode) {
            	case SERIAL_PARITY_NONE:
              	dcb.Parity      = NOPARITY;
                     dcb.fParity     = 0;
                     break;
            	case SERIAL_PARITY_EVEN:
               	dcb.Parity      = EVENPARITY;
                     dcb.fParity     = 1;
                     break;
            	case SERIAL_PARITY_ODD:
              	dcb.Parity      = ODDPARITY;
                     dcb.fParity     = 1;
                     break;
        	}


//       dcb.StopBits        = ONESTOPBIT;
//       dcb.ByteSize        = (BYTE) ByteSize;

       dcb.StopBits        = stopbit;
       dcb.ByteSize        = databit;


       // -------------- modified 2005-05-16 ---------------
       dcb.fInX            = FALSE;
       dcb.fOutX           = FALSE;

       hard_handshake = false;
        
       if (hard_handshake) {
            	dcb.fOutxDsrFlow    = TRUE;
            	dcb.fOutxCtsFlow    = TRUE;
            	dcb.fRtsControl     = RTS_CONTROL_HANDSHAKE;
            	dcb.fDtrControl     = DTR_CONTROL_HANDSHAKE;
        	}
       else {
            	dcb.fOutxDsrFlow    = FALSE;
            	dcb.fOutxCtsFlow    = FALSE;
            	/* */
            	dcb.fRtsControl     = RTS_CONTROL_ENABLE;
            	dcb.fDtrControl     = DTR_CONTROL_ENABLE;
            	/* * /
            	dcb.fRtsControl     = RTS_CONTROL_DISABLE;
            	dcb.fDtrControl     = DTR_CONTROL_DISABLE;
            	/* */
        	}
    
        /* OLD VERSION  (modifies 2005-05-16)

        dcb.fOutxCtsFlow    = 0;
        dcb.fOutxDsrFlow    = 0;
        dcb.fDtrControl     = DTR_CONTROL_DISABLE;
        dcb.fDsrSensitivity = 0;
        dcb.fRtsControl     = RTS_CONTROL_DISABLE;
        dcb.fOutX           = 0;
        dcb.fInX            = 0;
        
        */
        
        /* ----------------- misc parameters ----- */
        dcb.fErrorChar      = 0;
        dcb.fBinary         = 1;
        dcb.fNull           = 0;
        dcb.fAbortOnError   = 0;
//        dcb.wReserved       = 0;
        dcb.XonLim          = 2;
        dcb.XoffLim         = 4;
        dcb.XonChar         = 0x13;
        dcb.XoffChar        = 0x19;
        dcb.EvtChar         = 0;
        
        /* -------------------------------------------------------------------- */
        // opening serial port
        serial_handle    = CreateFile(port, 
        						GENERIC_READ | GENERIC_WRITE, 
        						0, 
        						NULL, 
        						OPEN_EXISTING,
        						FILE_FLAG_OVERLAPPED,
        						NULL
        						);
                   

        ovReader.hEvent    = serial_events[SIG_READER];
        ovWriter.hEvent    = serial_events[SIG_WRITER];
        ovWaitEvent.hEvent = serial_events[SIG_MODEM_EVENTS];

// 	InitTxBuf();
// 	InitRxBuf();
 	CommInit();

	if (serial_handle != INVALID_HANDLE_VALUE) {
		tx_in_progress = 0;
		rx_in_progress = 0;
            	if (check_modem) {
                	if(!SetCommMask(serial_handle, EV_RING | EV_RLSD)) {
                    		error = 1;
                		}
            		}
            	else {
                	if(!SetCommMask(serial_handle, 0)) { 	// Disable all events
                    		error = 1;
                		}
            		}
                
            	// set timeouts
            	if(!SetCommTimeouts(serial_handle, &cto)) {
                	error = 2;
            		}

            	// set DCB
            	if(!SetCommState(serial_handle, &dcb)) {
                	error = 4;
            		}
		}
        else {
            	error = 8;
        	}
    	}
 else {
 	error = 16;
 	}


 /* --------------------------------------------- */
 for (i=0; i<SERIAL_SIGNAL_NBR; i++) {
      	if (serial_events[i] == INVALID_HANDLE_VALUE) {
        	error = 32;
      		}
    	}

 /* --------------------------------------------- */
 if (error!=0) {
     	CloseHandle(serial_handle);
       serial_handle = INVALID_HANDLE_VALUE;
// 	JYEscope->Memo1->Lines->Append(JYEscope->cbPortList->Items->Strings[JYEscope->cbPortList->ItemIndex] + " open FAILED !");
    	}
 else {
       // start thread
       thread_handle = (HANDLE) _beginthreadex(NULL, 0, (PBEGINTHREADEX_THREADFUNC) Tserial_event_thread_start, this, 0, &threadid);
    	}

 /* --------------------------------------------- */
 return (error);
}


/* -------------------------------------------------------------------- */
/* ---------------------           setManager     --------------------- */
/* -------------------------------------------------------------------- */
// Assign a processing manager
void         Tserial_event::setManager(type_myCallBack manager_arg)
{
  manager = manager_arg;
}
/* -------------------------------------------------------------------- */
/* ---------------------           setRxSize      --------------------- */
/* -------------------------------------------------------------------- */
void         Tserial_event::setRxSize(int size)
{
 max_rx_size = size;
 if (max_rx_size>SERIAL_MAX_RX) {
 	max_rx_size = SERIAL_MAX_RX;
 	}
}
/* -------------------------------------------------------------------- */
/* ---------------------           getDataInBuffer     --------------------- */
/* -------------------------------------------------------------------- */
char *      Tserial_event::getDataInBuffer(void)
{
    return(rxBuffer);
}
/* -------------------------------------------------------------------- */
/* ---------------------           getDataInSize     --------------------- */
/* -------------------------------------------------------------------- */
int      Tserial_event::getDataInSize(void)
{
    return(received_size);
}
/* -------------------------------------------------------------------- */
/* ---------------------           dataHasBeenRead     --------------------- */
/* -------------------------------------------------------------------- */
void   Tserial_event::dataHasBeenRead(void)
{
    SetEvent(serial_events[SIG_READ_DONE]);
}
/* -------------------------------------------------------------------- */
/* -----------------------   getNbrOfBytes  --------------------------- */
/* -------------------------------------------------------------------- */
int Tserial_event::getNbrOfBytes    (void)
{
    struct _COMSTAT status;
    int             n;
    unsigned long   etat;

    n = 0;

    if (serial_handle!=INVALID_HANDLE_VALUE)
    {
        ClearCommError(serial_handle, &etat, &status);
        n = status.cbInQue;
    }
    return(n);
}
/* -------------------------------------------------------------------- */
/* --------------------------    sendData     ------------------------- */
/* -------------------------------------------------------------------- */
void Tserial_event::sendData (char *buffer, int size)
{
 if ((!tx_in_progress) && (size<SERIAL_MAX_TX) && (buffer!=0)) {
        tx_in_progress = 1;
        memcpy(txBuffer, buffer, size);
        tx_size = size;
        SetEvent(serial_events[SIG_DATA_TO_TX]);
        // indicating data to be sent
    	}
}
/* -------------------------------------------------------------------- */
/* --------------------------    OnEvent      ------------------------- */
/* -------------------------------------------------------------------- */
void Tserial_event::OnEvent (unsigned long events)
{
    unsigned long ModemStat;

    GetCommModemStatus(serial_handle, &ModemStat);

 if ((events & EV_RING)!=0) {
 	if ((ModemStat &  MS_RING_ON)!= 0) {
		if (manager!=0) {
			manager((uint32) this, SERIAL_RING);
			}
        	}
    	}

 if ((events & EV_RLSD)!=0) {
	if ((ModemStat &  MS_RLSD_ON)!= 0) {
		if (manager!=0) {
			manager((uint32) this, SERIAL_CD_ON);
     			}
      		}
	else {
		if (manager!=0) {
			manager((uint32) this, SERIAL_CD_OFF);
			}
		}
 	}
}
/* -------------------------------------------------------------------- */
/* --------------------------       run       ------------------------- */
/* -------------------------------------------------------------------- */

/**
 this function is the main loop of the Tserial object. There is a
 do while() loop executed until either an error or a PowerDown is 
 received.
 this is not a busy wait since we use the WaitForMultipleObject function
*/

void Tserial_event::run(void)
{
 bool          done;
 long          status;
 unsigned long read_nbr, result_nbr;
 char          success;
 char *txptr, *rxptr;

 ready                   = true;
 done                    = false;
 tx_in_progress          = 0;
 rx_in_progress          = 0;
 WaitCommEventInProgress = 0;
 

 if (manager!=0) {
	manager((uint32) this, SERIAL_CONNECTED);
 	}

 GetLastError();               // just to clear any pending error
 SetEvent(serial_events[SIG_READ_DONE]);	// This will start receiving

 /* ----------------------------------------------------------- */
 while(!done) {
    	/* ------------------------------------------------------------------ */
       /*                                                                    */
       /*                                                                    */
       /*                                                                    */
       /*                          Waiting  for signals                      */
       /*                                                                    */
       /*                                                                    */
       /*                                                                    */
       /* ------------------------------------------------------------------ */
       // Main wait function. Waiting for something to happen.
       // This may be either the completion of a Read or a Write or
       // the reception of modem events, Power Down, new Tx
       //
       status = WaitForMultipleObjects(SERIAL_SIGNAL_NBR, serial_events, FALSE, INFINITE);

       // processing answer to filter other failures
       status = status - WAIT_OBJECT_0;
       if ((status<0) || (status>=SERIAL_SIGNAL_NBR)) {
			done = true;   // error
        	}
       else {
            	/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
            	/* ++++++++++++++++++++ EVENT DISPATCHER ++++++++++++++++++ */
            	/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
     		switch(status) {
          		case SIG_POWER_DOWN:
              		// receiving a POWER down signal. Stopping the thread
              		//	This event is set when Disconnect command is executed.
                    		done = true;
                    		break;
					
                /* ######################################################## */
                /* #                                                      # */
                /* #                                                      # */
                /* #                       TX                           # */
                /* #                                                      # */
                /* #                                                      # */
                /* ######################################################## */
               	 case SIG_DATA_TO_TX:
                   	 	// Signal asserted that there is a new valid data in txBuffer
             		      	// in the "txBuffer" variable
            	        	// sending data to the port
            	        	while(!TxQueueEmpty(&MyComm)) {
					// Load Tx buffer from TxQueue
					this->tx_size = 0;
					txptr = txBuffer;
					while(TxQueueDelete(&MyComm, txptr++)) {	// Copy data from Tx queue to 'buffer'
						tx_size++;
						if(tx_size >= SERIAL_MAX_TX) {
							break;
							}
						}

					// Send data in txBuffer
					success = (char) WriteFile(serial_handle, txBuffer, tx_size, &result_nbr, &ovWriter);
               	     		if (!success) {
						// Sending failed. Check for cause.
                     	       	if(GetLastError() != ERROR_IO_PENDING ) {
                                			// Real failure. Quit.
                                			done = true;
                            			}
						else {
							// Sending is just not finished yet. Wait till it finishes which is inidcated by SIG_WRITER being signaled.
							break;
							}
                        			}
					// Sending is successful. So loopback to check TxQueue again.
            	        		}
							
                    		break;

			 // Note that ovWriter was set to point at SIG_WRITER in connect(). Once WriteFile() is done SIG_WRITER will be set.
              	  case SIG_WRITER:
                	    	// WriteFile has terminated
                	    	// checking the result of the operation
                	    	if (GetOverlappedResult(serial_handle, &ovWriter, &result_nbr, FALSE)) {
                	        	// Sending operation completed successfully
                 	       	ResetEvent(serial_events[SIG_WRITER]);
					// Go to check if there is any data to be sent.
					SetEvent(serial_events[SIG_DATA_TO_TX]);
					}
                    		else {
                        		// GetOverlapped didn't succeed !
                       	 	// What's the reason ?
                        		if(GetLastError() != ERROR_IO_PENDING ) {
						// failure. This will cause the port being closed just like a disconnect command			
						done = true;  
                        			}
					// Otherwise contiune waiting
                    			}
                   	 	break;


                /* ######################################################## */
                /* #                                                      # */
                /* #                                                      # */
                /* #                       RX                             # */
                /* #                                                      # */
                /* #                                                      # */
                /* ######################################################## */
            		case SIG_READ_DONE:
                    		// previous reading is finished
                    		// I start a new one here
                    		if (!rx_in_progress) {
					rx_in_progress = 1;
					do {
						if(ReadFile(serial_handle, &rxBuffer, max_rx_size, &received_size, &ovReader)) {
							// Read success. Process data
							RxQueueAdd(&MyComm, rxBuffer);	// Note: max_rx_size has been set to 1 so only do save once.
/*					
							rxptr = rxBuffer;
							read_nbr = received_size;
							while(read_nbr) {
								if(!RxQueueAdd(&MyComm, rxptr)) {
		//							JYEscope->Memo1->Lines->Append(AnsiString("RxBuf overflow !!"));
									continue;
									};
								rxptr++;
								read_nbr--;
								}
*/							
							CommRx(&MyComm);
							// Start a new read
//							SetEvent(serial_events[SIG_READ_DONE]);
							}
						else {
							// ReadFile not completed. It will return  ERROR_IO_PENDING if no other error.
							//	In this case the process will be suspended until ovReader event (i.e. SIG_READER) is set, which
							//	transfer the control to the following case statement.
		                            	if(GetLastError() != ERROR_IO_PENDING ) {
		                            		// real failure => quiting
		                            		done = true;
		                            		}
							break;			
							}
						} while(received_size);
                    			}
                    		break;
					
                	case SIG_READER:
                    		// reading the result of the terminated read
	                    	//BOOL GetOverlappedResult(
 	                   	//    HANDLE hFile,							// handle of file, pipe, or communications device
   	                 	//    LPOVERLAPPED lpOverlapped,			// address of overlapped structure
 	                   	//    LPDWORD lpNumberOfBytesTransferred,	// address of actual bytes count
       	             	//    BOOL bWait 							// wait flag
         	           	//   );
         	           	//
                    		if (GetOverlappedResult(serial_handle, &ovReader, &received_size, FALSE)) {
                        		// no error => OK
                      	  	// Read operation completed successfully
                      	  	ResetEvent(serial_events[SIG_READER]);
					rx_in_progress = 0; 	// read has ended
					RxQueueAdd(&MyComm, rxBuffer);
/*
					rxptr = rxBuffer;
					while(received_size) {
						if(!RxQueueAdd(&MyComm, rxptr)) {
//							JYEscope->Memo1->Lines->Append(AnsiString("RxBuf overflow !!"));
							continue;
							};
						rxptr++;
						received_size--;
						}
*/						
					CommRx(&MyComm);
					// Start a new read
					SetEvent(serial_events[SIG_READ_DONE]);
                    			}
     	               	else {
       	                 	// GetOverlapped didn't succeed !
              	          	// What's the reason ?
                     	   	if(GetLastError() != ERROR_IO_PENDING ) {
//     						JYEscope->Memo1->Lines->Append(AnsiString("Serial port receiving error"));
						done = true;  // failure. This will cause the port being closed just like a disconnect command
                        			}
					// Otherwise continue waiting
                    			}
                    		break;
						
            		}
        	}
    	};

 // --------------------- Disconnecting ----------------
 ready = false;
 if (serial_handle != INVALID_HANDLE_VALUE) {
 	CloseHandle(serial_handle);
 	}
 serial_handle = INVALID_HANDLE_VALUE;

 if (manager!=0) {
 	// Notice manager the port closed
 	manager((uint32) this, SERIAL_DISCONNECTED);
 	}
 
}


/* ======================================================== */
/* ===============  OnCharArrival     ===================== */
/* ======================================================== */
void SerialEventManager(uint32 object, uint32 event)
{
 U8 *buffer;
 char  tmp;
 int   size;
 Tserial_event *com;
 char   txt[512];

 com = (Tserial_event *) object;
 if (com!=0) {
      	switch(event) {
            	case  SERIAL_CONNECTED  :
//                  	JYEscope->Memo1->Lines->Append(AnsiString("Connected !"));
                     break;
					 
            	case  SERIAL_DISCONNECTED  :
     			JYEscope->Memo1->Lines->Append(AnsiString("Serial port closed"));
			ConnectFailed();
 			break;
			
            	case  SERIAL_DATA_SENT  :
			// Continue sending if TxBuf is not empty.
//			if((!com->tx_in_progress) && (!TxBufEmpty())){
			if((!com->tx_in_progress) && (!TxQueueEmpty(&MyComm))){
				com->tx_size = 0;
				buffer = com->txBuffer;
//				while(DeleteFromTxBuf(buffer++)) {
				while(TxQueueDelete(&MyComm, buffer++)) {
					com->tx_size++;
					if(com->tx_size >= SERIAL_MAX_TX) {
						break;
						}
					}
        			// indicating data to be sent
        			com->tx_in_progress = 1;
        			SetEvent(com->serial_events[SIG_DATA_TO_TX]);		
				}
			break;
			
            	case  SERIAL_DATA_ARRIVAL  :
			size   = com->getDataInSize();
			buffer = com->getDataInBuffer();

 //     	 				SetEvent(ConnectWaitEvent);
			while(size) {
//				if(!AddToRxBuf(buffer++)) {
				if(!RxQueueAdd(&MyComm, buffer++)) {
//					JYEscope->Memo1->Lines->Append(AnsiString("RxBuf overflow !!"));
					};
				size--;
				}
			// Call RxProc to process the received data
//			RxProc();
			CommRx(&MyComm);
			
			// To start new receiving by setting SIG_READ_DONE
			com->dataHasBeenRead();
			break;
			
		case SERIAL_DATA_TOSEND:
//			if((!com->tx_in_progress) && (!TxBufEmpty())){
			if((!com->tx_in_progress) && (!TxQueueEmpty(&MyComm))){
				com->tx_size = 0;
				buffer = com->txBuffer;
//				while(DeleteFromTxBuf(buffer++)) {	// Copy data from Tx queue to 'buffer'
				while(TxQueueDelete(&MyComm, buffer++)) {	// Copy data from Tx queue to 'buffer'
					com->tx_size++;
					if(com->tx_size >= SERIAL_MAX_TX) {
						break;
						}
					}
        			// indicating data to be sent
        			com->tx_in_progress = 1;
        			SetEvent(com->serial_events[SIG_DATA_TO_TX]);		
				}

/*			
    			if ((!com->tx_in_progress) && (size<SERIAL_MAX_TX) && (buffer!=0)) {
        			com->tx_in_progress = 1;
        	 	 	memcpy(txBuffer, buffer, size);
        			com->tx_size = size;
        			// indicating data to be sent
        			SetEvent(serial_events[SIG_DATA_TO_TX]);
    				}
*/
			break;
        	}
    	}
}

char 	*PortAvailable(char *portname)
{
 bool	bSuccess = false;
 HANDLE hPort;
 DWORD	dwError;

 hPort = CreateFile(portname, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
 if (hPort == INVALID_HANDLE_VALUE) {
 	dwError = GetLastError();
 	//Check to see if the error was because some other app had the port open or a general failure
 	if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION || dwError == ERROR_SEM_TIMEOUT) {
     		bSuccess = TRUE;
 	 	}
  	}
 else {
 	//The port was opened successfully
 	bSuccess = TRUE;

 	//Don't forget to close the port, since we are going to do nothing with it anyway
 	CloseHandle(hPort);
 	}

 if(bSuccess) {
 	return portname;
 	}
 else {
 	return 0;
 	}

}

/*
// ========================================================
// Setup Uart 0 transmitting buffer
//
void	InitTxBuf(void)
{
 TxHead = TxBuf;
 TxTail = TxBuf;
}

// ========================================================
// Setup Uart 0 receiption buffer
//
void	InitRxBuf(void)
{
 RxHead = RxTail = RxBuf;
}

// ========================================================
//	Check TxBuf
U8	TxBufEmpty(void)
{
 return (TxTail == TxHead);
}

// ========================================================
//	Check RxBuf
U8	RxBufEmpty(void)
{
 return (RxTail == RxHead);
}

// ========================================================
// Add one character to TxBuf. Returns non-zero if success,
//	otherwise returns 0.
// 
U8	AddToTxBuf(U8 *Item)
{
 U8	*ptr;

 *TxTail = *Item;
 if(TxTail+1 == TxBuf + TxBufSize) {
 	ptr = TxBuf;
 	}
   else {
   	ptr = TxTail+1;
   	}
 if(ptr == TxHead) {
 	return 0;
 	}
   else {
   	TxTail = ptr; 
	return 1;
	}
}

// ========================================================
// Delet one character from TxBuf and save it to "Item". 
//	Returns non-zero if success, otherwise returns 0.
// 
U8	DeleteFromTxBuf(U8 *Item)
{
 if(TxHead == TxTail) {
 	return 0;
	}
 *Item = *TxHead;
 if((TxHead + 1) == (TxBuf + TxBufSize)) {
 	TxHead = TxBuf;
 	}
   else {
   	TxHead++;
   	}
 return 1;
}

// ========================================================
// Add one character to RxBuf. Returns non-zero if success,
//	otherwise returns 0.
// 
U8	AddToRxBuf(U8 *Item)
{
 U8	*ptr;

 *RxTail = *Item;
 if(RxTail + 1 == RxBuf + RxBufSize) {
 	ptr = RxBuf;
 	}
   else {
   	ptr = RxTail + 1;
   	}
 if(ptr == RxHead) {
 	return 0;
 	}
   else {
   	RxTail = ptr; 
	return 1;
	}
}

// ========================================================
// Delet one character from RxBuf and save it to "Item". 
//	Returns non-zero if success, otherwise returns 0.
// 
U8	DeleteFromRxBuf(U8 *Item)
{
 if(RxHead == RxTail) {
 	return 0;
 	}
 *Item = *RxHead;
 if(RxHead + 1 == RxBuf + RxBufSize) {
 	RxHead = RxBuf;
 	}
 else {
 	RxHead++;
   	}
 return 1;
}


void	CommInit(void)
{
 SynFound = false;
 SynChSent = false;
 Rxing = false;
 Txing = false;
}

void	RxProc(void)
{
 U8	tmp0;
 U16	tmp1;

 // Check and get a byte from Uart0RxBuf
 while(DeleteFromRxBuf(&tmp0)) {
//       JYEscope->Memo2->Lines->Append(IntToStr(tmp0));

	// Check for SynCh
	if(tmp0 == SynCh) {
		// Mark it if SycCh found
		SynFound = 1;
		continue;
		}

 	if(SynFound) {
		// Process the byte following SynCh
		SynFound = 0;	// Clear SynCh_Found flag first
		// The previous byte was SynCh. If Rxing is valid and the byte following SynCh is 0x00
		//	then the SynCh was a data byte. Otherwise check for frame ID. 
		if((Rxing == true) && (tmp0 == 0x00)) {
			// Frame receiving in process and the current byte is 0x00. Save SynCh as data
			FrameBuf[FrameIndex++] = SynCh;
			// Go to check for frame end
			goto FrameEndCheck;
			}
		else {
			// Rxing not valid or the byte following SynCh is not 0x00.
			// Check for frame ID
			if(IsFrame(tmp0)) {
				// Initialize a freme
				FrameBuf[0] = tmp0;
				FrameBuf[2] = 0xFF;	// Make sure correct frame length check (byte1 & byte 2 are frame length)
				RxCnt = 0xFFFF;
				FrameIndex = 1;
				Rxing = true;
				}
			else {
				// Clear Rxing anyway. This is to prevent error at Rxing being valid but
				//	SynCh is not followed by frame ID or 0x00.
				Rxing = false;
				}
			}
 		}
	else {
		// Normal byte processing. Save bytes if Rxing is valid
		if(Rxing) {
			FrameBuf[FrameIndex++] = tmp0;
			FrameEndCheck:
			if(FrameIndex == 3) {
//				RxCnt = ((U16)FrameBuf[2] << 8) + FrameBuf[1];
				RxCnt = ((U32)FrameBuf[2] * 256 + (U32)FrameBuf[1]);
				}
//			if((FrameIndex >= (((U16)FrameBuf[2] << 8) + FrameBuf[1])) ||(FrameIndex > FrameSizeMax)) {
			if((FrameIndex >= RxCnt) ||(FrameIndex > FrameSizeMax)) {
				FrameProc();
				Rxing = false;
				}
			}
 		}
 	}

}
*/

//=======================================================
// Return the number of bytes actually sent
//
U32	TxProc(U8 *buf, U32 size)
{
 U32 tmp;
 U8	tmp0;

 if(!PortValid(serial_port->serial_handle)) {
	// Port not vaild
 	JYEscope->Memo1->Lines->Append(fmPortSetup->cbPortList->Items->Strings[fmPortSetup->cbPortList->ItemIndex] + " not valid");
	return 0;
 	}
 
 // Send dummy char
 tmp0 = 0x00;
// while(!AddToTxBuf(&tmp0)) {
 while(!TxQueueAdd(&MyComm, &tmp0)) {
// 	SerialEventManager((uint32)serial_port, SERIAL_DATA_TOSEND);
	SetEvent(serial_port->serial_events[SIG_DATA_TO_TX]);	
 	}

 // Send SynCh
 tmp0 = SynCh;
// while(!AddToTxBuf(&tmp0)) {
 while(!TxQueueAdd(&MyComm, &tmp0)) {
// 	SerialEventManager((uint32)serial_port, SERIAL_DATA_TOSEND);
	SetEvent(serial_port->serial_events[SIG_DATA_TO_TX]);	
 	}

 tmp0 = 0x00;
 tmp = size;
 while(size) {
// 	while(!AddToTxBuf(buf)) {
 	while(!TxQueueAdd(&MyComm, buf)) {
// 		SerialEventManager((uint32)serial_port, SERIAL_DATA_TOSEND);
		SetEvent(serial_port->serial_events[SIG_DATA_TO_TX]);	
 		}
	if(*buf == SynCh) {
		// Send 0x00 
// 		while(!AddToTxBuf(&tmp0)) {
 		while(!TxQueueAdd(&MyComm, &tmp0)) {
//			SerialEventManager((uint32)serial_port, SERIAL_DATA_TOSEND);
			SetEvent(serial_port->serial_events[SIG_DATA_TO_TX]);	
 			}
		}
	buf++;
 	size--;
 	}

 // Inform serial event manager
// SerialEventManager((uint32)serial_port, SERIAL_DATA_TOSEND);
 SetEvent(serial_port->serial_events[SIG_DATA_TO_TX]);	
// JYEscope->Memo1->Lines->Append(IntToStr(tmp - size) + " bytes sent");
 
 return (tmp - size);
 
 
}

//=======================================================
// Return non-zero if port handle is valid
//
U8	PortValid(HANDLE porthandle)
{
 return (porthandle != INVALID_HANDLE_VALUE);
}


U8		IsFrame(U8 FrameID)
{
 U8 tmp0 = 0;
 
 switch(FrameID) {
	case FID_DSOparam:
	case FID_Query:
	case FID_Connect:
	case FID_Confirm:
	case FID_Disconnect:
	case FID_Loopback:
	case FID_DataFM:
	case FID_DataRM:
	case FID_SetSerial:
	case FID_WriteRequest:
	case FID_WriteReturn:
	case FID_WriteWaveform:
	case FID_DSOparamOL:
	case FID_DataOL:
	case FID_ReadWaveform:
	case FID_DSO:
	case FID_Diag:
		tmp0 = 1;
		break;

	default:
		break;
	}

 return(tmp0);
}

void	FrameProc(void)
{
 U16 tmp0, tmp1, tmp2, tmp3;
 sSIZE	*ptmp0, *ptmp1, *ptmp2;
 
 char	*strtmp1 = "HH ";	// 3 character string for Hex diaplay mode
 char	HexDigits[17] = {"0123456789ABCDEF"};
 
 // Frame analysis
 switch((U8)FrameBuf[0]) {
	case FID_Confirm:
      	 	SetEvent(ConnectWaitEvent);
     		JYEscope->Memo1->Lines->Append(AnsiString("Device detected"));
		switch(FrameBuf[3]) {
			case 'O': 
				FrontEndType = 'O';
				OnScreen->Model = FrameBuf[4];
	     			JYEscope->Memo1->Lines->Append(AnsiString("Device type: Oscilloscope"));
				switch (OnScreen->Model) {
					case 0x02:
	     					JYEscope->Memo1->Lines->Append(AnsiString("Device model: DSO 082"));
						JYEscope->UnitName->Caption = "DSO082";
						break;
					case 0x03:
	     					JYEscope->Memo1->Lines->Append(AnsiString("Device model: DSO 094"));
						JYEscope->UnitName->Caption = "DSO094";
						// Send parameter
						Command(FID_DSOparam);
						break;
					case 0x04:
	     					JYEscope->Memo1->Lines->Append(AnsiString("Device model: DSO 068"));
						JYEscope->UnitName->Caption = "DSO068";
//						DSO068_Setup();
						break;
					default:
	     					JYEscope->Memo1->Lines->Append(AnsiString("Device model: Unknown"));
						JYEscope->UnitName->Caption = "DSO";
						break;
					}
				
				break;

			case 'G':
				// Signal generator frontend
				FrontEndType = 'G';
	     			JYEscope->Memo1->Lines->Append(AnsiString("Device type: Signal generator"));
				switch (FrameBuf[4]) {
					case 0x01:
	     					JYEscope->Memo1->Lines->Append(AnsiString("Device model: FG 085"));
						JYEscope->UnitName->Caption = "FG085";
						break;
					default:
	     					JYEscope->Memo1->Lines->Append(AnsiString("Device model: Unknown"));
						JYEscope->UnitName->Caption = "F. Gen";
						break;
					}
				break;

			default:
				JYEscope->UnitName->Caption = "N/A";
				break;
			}
		break;

	case FID_DSO:
		// DSO commands (for models other than DSO 094)
		switch(OnScreen->Model) {
			case 0x04:	// DSO 068
				DSO_CmdProc068();
				break;

			default:
				break;
			}
		break;
		
	case FID_WriteReturn:
     		JYEscope->Memo1->Lines->Append(AnsiString("Data write successful"));
		break;
				
	case FID_DSOparam:
//     		JYEscope->Memo1->Lines->Append(AnsiString("Parameter frame received"));
		// Update coupling
		Capturer->SetCpl(Channel_1, FrameBuf[3 + 4]);
		Capturer->SetCpl(Channel_2, FrameBuf[3 + 8]);
		CplRefresh();

//     		JYEscope->Memo1->Lines->Append(IntToStr((U16)FrameBuf[3 + 14] * 256 + (U16)FrameBuf[3 + 13]));
/*			
		tmp0 = (U16)(&jyScope->oscSysMode) - (U16)(&jyScope->oscTimebase) + 1; 
		ptmp0 = FrameBuf + 3;
		ptmp1 = (U8 *)&jyScope->oscTimebase;
		tmp1 = 0;
		while(tmp1 < tmp0) {
			*ptmp1++ = *ptmp0++;
			tmp1++;
			}
		VSenRefresh();
		TBRefresh();
*/		
		break;

 // DSO094 parameter structure:
/*
typedef	struct 	{
	U8	TimeBase;	// 00
	S16	HPos;		// 01
	U8	Ch1Vsen;	// 03
	U8	Ch1Cpl;		// 04
	S16	Ch1Vpos;	// 05
	U8	Ch2Vsen;	// 07
	U8	Ch2Cpl;		// 08
	S16	Ch2Vpos;	// 09
	U8	TrigMode;	// 11
	U8	TrigEdge;		// 12
	S16	TrigLvl;		// 13
	U8	TrigSrc;		// 15
	S8	TrigPos;		// 16
	U8	TrigSen;		// 17
	S16	RecLen;		// 18
	U8	Baudrate;	// 20
	U8	ChnCfg;		// 21
	S8	Ref1;		// 22
	S8	Ref2;		// 23
	U8	SysMode;	// 24	
	U8	Status;		// 25
	} DSO;
*/
	case FID_DSOparamOL:
		// Offline DSO parameter frame received
		// Save parameters to object 'Capturer' if it is created
		Capturer->SetTimeBase(FrameBuf[3 + 0] + 2);		; // 2 is the TB seting offset between jyeLab and DSO094
		Capturer->SetHPos(3 * (*(S16 *)(&FrameBuf[3 + 1])));
		Capturer->SetVSen(Channel_1, FrameBuf[3 + 3]);
		Capturer->SetCpl(Channel_1, FrameBuf[3 + 4]);
//		Capturer->SetVPos(Channel_1, *(S16 *)(&FrameBuf[3 + 5]));
		Capturer->SetVSen(Channel_2, FrameBuf[3 + 7]);
		Capturer->SetCpl(Channel_2, FrameBuf[3 + 8]);
//		Capturer->SetVPos(Channel_2, *(S16 *)(&FrameBuf[3 + 10]));
		Capturer->SetTrigMode(FrameBuf[3 + 11] - 1);
		Capturer->SetTrigSlope(FrameBuf[3 + 12] - 1);
		Capturer->SetTrigLvl(3 * (*(S16 *)(&FrameBuf[3 + 13])));
		Capturer->SetTrigSrc(FrameBuf[3 + 15] - 1);
		Capturer->SetTrigPos(FrameBuf[3 + 16]);
		Capturer->SetRecLen(3 * (*(U16 *)(&FrameBuf[3 + 18])));	
		Capturer->SetRef(Channel_1, 3 * FrameBuf[3 + 22]);
		Capturer->SetRef(Channel_2, 3 * FrameBuf[3 + 23]);
		// Keep original settings of Vsen and TB
		Capturer->oscTBOrig = Capturer->oscTimebase;
		Capturer->oscCh[0].SenOrig = Capturer->oscCh[0].Sen;
		Capturer->oscCh[1].SenOrig = Capturer->oscCh[1].Sen;
		// Set VPos to proper level
		Capturer->oscCh[0].VPos = (V0 - Capturer->oscCh[0].Reference) + 60;
		Capturer->oscCh[1].VPos = (V0 - Capturer->oscCh[1].Reference) - 60;
		break;

 	case FID_DataOL:
		// Data length
		tmp0 = *(U16 *)&FrameBuf[1];
		
		// Always set SampleStart to 0 for FlashMode data
		Capturer->oscSampleStart = 0;		

		// Select Buffer A
		ptmp1 = Capturer->oscCh[0].BufA;
		ptmp2 = Capturer->oscCh[1].BufA;
		Capturer->oscBufSelector = 0;
		
		// Load channel 2 data
		tmp3 = (U16)FrameBuf[5];
//		tmp1 = tmp0/2 - 1;
		tmp1 = tmp0/2 + 1;
		tmp2 = 2;
		while(tmp1) {
			tmp0 = tmp3;						// Current sample
			tmp3 = (U16)FrameBuf[tmp2 + 5];	// Next sample
			// 1-for-3 Linear interpopulation
			// Vertically multiply by 3 since the pixel density of waveform window is 3 times higher than screen on front end device
			*ptmp2++ = 3 * tmp0;				// 
			*ptmp2++ = 2 * tmp0 + tmp3;
			*ptmp2++ = tmp0 + 2 * tmp3;
			tmp2 += 2;
			tmp1--;
			}
		
		// Load channel 1 data
		// Sample length
		tmp0 = ((U16)FrameBuf[2] * 256 + (U16)FrameBuf[1]) - 6;	

		tmp3 = (U16)FrameBuf[6];
		tmp1 = tmp0/2 + 1;
		tmp2 = 2;
		while(tmp1) {
			tmp0 = tmp3;						// Current sample
			tmp3 = (U16)FrameBuf[tmp2 + 6];	// Next sample
			// 1-for-3 Linear interpopulation
			// Vertically multiply by 3 since the pixel density of waveform window is 3 times higher than screen on front end device
			*ptmp1++ = 3 * tmp0;				// 
			*ptmp1++ = 2 * tmp0 + tmp3;
			*ptmp1++ = tmp0 + 2 * tmp3;
			tmp2 += 2;
			tmp1--;
			}
// 		JYEscope->Memo1->Lines->Append("Waveform received");

		SetEvent(WaveformUploadEvent);
		break;

 	case FID_DataFM:
//     		JYEscope->Memo1->Lines->Append(AnsiString("Data block received"));
			
		 if(Capturer->oscState != OS_Running) {
		 	break;
		 	}

		if(Capturer->oscTrigMode == TM_Single) {
			// Enter Stopped state
			Capturer->oscState = OS_Stopped;
			OscStateRefresh();
			BackupWaveform(Capturer);
			}
		
		// Sample length
		tmp0 = ((U16)FrameBuf[2] * 256 + (U16)FrameBuf[1]) - 6;	
		
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
		
		// Load channel 2 data
		tmp3 = (U16)FrameBuf[5];
//		tmp1 = tmp0/2 - 1;
		tmp1 = tmp0/2 + 1;
		tmp2 = 2;
		while(tmp1) {
			tmp0 = tmp3;						// Current sample
			tmp3 = (U16)FrameBuf[tmp2 + 5];	// Next sample
			// 1-for-3 Linear interpopulation
			// Vertically multiply by 3 since the pixel density of waveform window is 3 times higher than screen on front end device
			*ptmp2++ = 3 * tmp0;				// 
			*ptmp2++ = 2 * tmp0 + tmp3;
			*ptmp2++ = tmp0 + 2 * tmp3;
			tmp2 += 2;
			tmp1--;
			}
		
		// Load channel 1 data
		// Sample length
		tmp0 = ((U16)FrameBuf[2] * 256 + (U16)FrameBuf[1]) - 6;	

		tmp3 = (U16)FrameBuf[6];
		tmp1 = tmp0/2 + 1;
		tmp2 = 2;
		while(tmp1) {
			tmp0 = tmp3;						// Current sample
			tmp3 = (U16)FrameBuf[tmp2 + 6];	// Next sample
			// 1-for-3 Linear interpopulation
			// Vertically multiply by 3 since the pixel density of waveform window is 3 times higher than screen on front end device
			*ptmp1++ = 3 * tmp0;				// 
			*ptmp1++ = 2 * tmp0 + tmp3;
			*ptmp1++ = tmp0 + 2 * tmp3;
			tmp2 += 2;
			tmp1--;
			}

		SetEvent(WaveformUpdateEvent);
		break;

	case FID_DataRM:
//     		JYEscope->Memo1->Lines->Append(AnsiString("DataRM frame received"));

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
		tmp3 = FrameBuf[3];		// Ch 2

		// Insert the first sample
		*ptmp1++ = 2 * tmp0 + tmp2;
		*ptmp2++ = 2 * tmp1 + tmp3;
		// Insert the second sample
		*ptmp1++ = tmp0 + 2 * tmp2;
		*ptmp2++ = tmp1 + 2 * tmp3;
		// Insert the third sample, i.e. the newly captured sample
		*ptmp1  = 3 * tmp2;
		*ptmp2  = 3 * tmp3;

/*
		// Interpopulation
		// The previous samples
		tmp0 = (*(ptmp1 + Capturer->oscSampleStart))/3;	// Ch 1
		tmp1 = (*(ptmp2 + Capturer->oscSampleStart))/3;	// Ch 2
		// New samples
		tmp2 = FrameBuf[4];		// Ch 1
		tmp3 = FrameBuf[3];		// Ch 2
		
		// Insert the first sample
		Capturer->oscSampleStart++;
		if(Capturer->oscSampleStart == Capturer->oscRecLen) {
			Capturer->oscSampleStart = 0;
			}
		*(ptmp1 + Capturer->oscSampleStart) = 2 * tmp0 + tmp2;
		*(ptmp2 + Capturer->oscSampleStart) = 2 * tmp1 + tmp3;

		// Insert the second sample
		Capturer->oscSampleStart++;
		if(Capturer->oscSampleStart == Capturer->oscRecLen) {
			Capturer->oscSampleStart = 0;
			}
		*(ptmp1 + Capturer->oscSampleStart) = tmp0 + 2 * tmp2;
		*(ptmp2 + Capturer->oscSampleStart) = tmp1 + 2 * tmp3;

		// Store the new (third) sample
		Capturer->oscSampleStart++;
		if(Capturer->oscSampleStart == Capturer->oscRecLen) {
			Capturer->oscSampleStart = 0;
			}
		*(ptmp1 + Capturer->oscSampleStart) = 3 * tmp2;
		*(ptmp2 + Capturer->oscSampleStart) = 3 * tmp3;
*/		
		SetEvent(WaveformUpdateEvent);
		break;

	case FID_Disconnect:
     		JYEscope->Memo1->Lines->Append(AnsiString("Disconnect request received"));
		serial_port->disconnect();
		ConnectFailed();
/*		
    		ConnectState = 0;
    		JYEscope->UnitState->Caption = "Disconnected";
    		JYEscope->btnConnect->Caption = "Connect";
    		JYEscope->DeviceConnectionInd->Picture = DeviceDisconnected->Picture;
		JYEscope->UnitName->Caption = "N/A";
		JYEscope->btnRunStop->Enabled = false;

		Capturer->SetOscState(OS_Offline);
		
 		// Restore configuration to uninitialized state
 		OnScreen->Restore();
		
		OscStateRefresh();
		BackupWaveform(Capturer);

//		RepaintScreen();
*/
		break;

	case FID_Diag:
     		JYEscope->Memo1->Lines->Append(AnsiString("Diag fram received -- "));
		*(strtmp1 + 0) = HexDigits[FrameBuf[3] >> 4];
		*(strtmp1 + 1) = HexDigits[FrameBuf[3] & 0x0F];
		AppendToMemo(JYEscope->Memo1, strtmp1);
		AppendToMemo(JYEscope->Memo1, "-- ");
		tmp0 = 0;
		tmp1 = *(U16 *)&FrameBuf[1] - 6;
		while(tmp0 < tmp1) {
			*(strtmp1 + 0) = HexDigits[FrameBuf[tmp0 + 4] >> 4];
			*(strtmp1 + 1) = HexDigits[FrameBuf[tmp0 + 4] & 0x0F];
			AppendToMemo(JYEscope->Memo1, strtmp1);
			tmp0++;
			}
		
	default:
		break;
	}
}

// ===================================================
//
void	Command(U8 cmdID)
{
 S16 tmp0;
 U16 tmp1;
 
 // Do not send command if serial port is not available
 if(!PortValid(serial_port->serial_handle)) {
	// Port not vaild
	return;
 	}

 switch(cmdID) {
	case FID_Connect:
		CommandBuf[0] = FID_Connect;
		CommandBuf[1] = 0x04;
		CommandBuf[2] = 0x00;
		CommandBuf[3] = 0xC0;	// Connect DSO
//		JYEscope->Memo1->Lines->Append(AnsiString("Connecting ..."));
		break;

	case FID_Disconnect:
		CommandBuf[0] = FID_Disconnect;
		CommandBuf[1] = 0x04;
		CommandBuf[2] = 0x00;
		CommandBuf[3] = 0xFF;
//		JYEscope->Memo1->Lines->Append(AnsiString("Disconnect request sent"));
		break;

	case FID_Confirm:
		CommandBuf[0] = FID_Confirm;
		CommandBuf[1] = 0x04;
		CommandBuf[2] = 0x00;
		CommandBuf[3] = 0xFF;
		break;

	case FID_DSOparam:
		CommandBuf[0] = FID_DSOparam;
		CommandBuf[1] = 0x20;
		CommandBuf[2] = 0x00;
		CommandBuf[3] = OnScreen->oscTimebase - 2;
		CommandBuf[4] = OnScreen->oscHPos;
		CommandBuf[5] = OnScreen->oscHPos >> 8;
		CommandBuf[6] = OnScreen->oscCh[0].Sen;
		CommandBuf[7] = OnScreen->oscCh[0].Cpl;
		CommandBuf[8] = OnScreen->oscCh[0].VPos;
		CommandBuf[9] = OnScreen->oscCh[0].VPos >> 8;
		CommandBuf[10] = OnScreen->oscCh[1].Sen;
		CommandBuf[11] = OnScreen->oscCh[1].Cpl;
		CommandBuf[12] = OnScreen->oscCh[1].VPos;
		CommandBuf[13] = OnScreen->oscCh[1].VPos >> 8;
		CommandBuf[14] = OnScreen->oscTrigMode + 1;
		CommandBuf[15] = OnScreen->oscTrigSlope + 1;
		tmp0 = OnScreen->oscTrigLvl;
		if(OnScreen->oscTrigSrc == TS_Ch1) {
//			tmp0 -= OnScreen->oscCh[0].VPosOfs;
			tmp0 -= (V0 - OnScreen->oscCh[0].Reference);
			}
		if(OnScreen->oscTrigSrc == TS_Ch2) {
//			tmp0 -= OnScreen->oscCh[1].VPosOfs;
			}
			tmp0 -= (V0 - OnScreen->oscCh[1].Reference);
		tmp0 /= 3;
		*(S16 *)&CommandBuf[16] = tmp0;
//		CommandBuf[16] = tmp0;
//		CommandBuf[17] = tmp0 >> 8;
		CommandBuf[18] = OnScreen->oscTrigSrc + 1;
		CommandBuf[19] = OnScreen->oscTrigPos;
		CommandBuf[20] = OnScreen->oscTrigSen;
//		CommandBuf[20] = 4;

		tmp1 = OnScreen->oscRecLen/3 + 1;
		CommandBuf[21] = tmp1;
		CommandBuf[22] = tmp1 >> 8;
//		CommandBuf[21] = OnScreen->oscRecLen;
//		CommandBuf[22] = OnScreen->oscRecLen >> 8;

//		CommandBuf[23] = OnScreen->oscChnCfg;
		CommandBuf[23] = 3;		// Ch1 & Ch2
//		CommandBuf[24] = OnScreen->oscSysMode;
		CommandBuf[24] = 10;		// Virtual DSO mode
//		JYEscope->Memo1->Lines->Append(AnsiString("Parameter frame sent"));
		break;

	case FID_SetSerial:
		CommandBuf[0] = FID_SetSerial;
		CommandBuf[1] = 0x0D;
		CommandBuf[2] = 0x00;
		CommandBuf[3] = 0x00;
		
	default:
		break;
 	}

 TxProc(CommandBuf, (((U16)CommandBuf[2] << 8) |CommandBuf[1]));
}

void AppendToMemo(TMemo* Memo, LPCSTR text)
{
    int nPos, nNum;

    if (Memo==NULL || text==NULL) return;

    nNum = Memo->Perform(EM_GETLINECOUNT, 0, 0);
    nPos = Memo->Perform(EM_LINEINDEX, nNum, 0);
    if (nPos < 0)
        nPos = Memo->Perform(EM_LINEINDEX, nNum-1, 0);
    nNum = Memo->Perform(EM_LINELENGTH, nPos, 0);
    Memo->Perform(EM_SETSEL, nPos+nNum, nPos+nNum);
    Memo->Perform(EM_REPLACESEL, 0, (long)text);
}


