
#ifndef __USER_LIB_H__
#define __USER_LIB_H__

#ifdef __cplusplus
extern "C"
{
#endif
	


#define PLL_CLOCK 72000000

/*---------------------------------------------------------------------------------------------------------*/
//Default
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/*---------------------------------------------------------------------------------------------------------*/
//Official
#include "NUC230_240.h"
#include "cdc_serial.h"
	
/*---------------------------------------------------------------------------------------------------------*/

#include "IRQ.h"
#include "Bridging.h"
#include "FIFO.h"
#include "NRF_P2P.h"
#include "NRF24L01.h"
#include "vMemAlloc.h"


/*---------------------------------------------------------------------------------------------------------*/

extern FIFO_T * g_stVcomRx;
extern uint8_t vcom_txReady;

void PinSetup( void );
void ClkSetup( void );
void GpioSetup( void );
void SpiSetup( void );
void UartSetup( void );
void NVIC_Init( void );
void UsbSetup( void );

#endif //__USER_LIB_H__
