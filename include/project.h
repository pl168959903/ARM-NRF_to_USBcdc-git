
#ifndef __USER_LIB_H__
#define __USER_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#define USER_CFG_NRF24L01_0_SPI SPI1

#define PLL_CLOCK 72000000

/*---------------------------------------------------------------------------------------------------------*/
// Default
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------------------------------------*/
// Official
#include "NUC230_240.h"
#include "cdc_serial.h"

/*---------------------------------------------------------------------------------------------------------*/

#include "Bridging.h"
#include "FIFO.h"
#include "IRQ.h"
#include "NRF24L01.h"
#include "NRF_P2P.h"
#include "vMemAlloc.h"

/*---------------------------------------------------------------------------------------------------------*/

extern SPI_Func_T        g_stSpi1;
extern FIFO_T*           g_stVcomRx;
extern FIFO_T*           g_stNrfRx;
extern NRF_T*            g_stNrf0;
extern NRFP2P_CHANNEL_T* g_stNrfP1;
extern volatile uint8_t  vcom_txReady;

extern const uint8_t     p1Addr[ 5 ];
extern const uint8_t     CameraAddr[ 5 ];

void SetCE( void );
void ResetCE( void );
void NrfDelay(uint32_t time);

void PinSetup( void );
void ClkSetup( void );
void GpioSetup( void );
void SpiSetup( void );
void UartSetup( void );
void NVIC_Init( void );
void UsbSetup( void );
void NrfSetup(void);
void DelayUs( uint32_t us );

#endif  //__USER_LIB_H__
