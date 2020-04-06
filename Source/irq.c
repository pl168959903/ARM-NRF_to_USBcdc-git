#include "project.h"

static uint8_t vcom_txReady = 0;

void EINT1_IRQHandler( void ) {
    uint8_t rfData[ 26 ] = { 0 };
    if ( GPIO_GET_INT_FLAG( PB, BIT15 ) ) {
        if ( nrf_rx_package( ( uint8_t* )rfData ) ) {
            vcom_push_data( rfData, 26 );
        }
        GPIO_CLR_INT_FLAG( PB, BIT15 );
    }
}

void EP2_Handler( void ) {
    vcom_txReady = 1;
}

void EP3_Handler( void ) {
    size_t   i;
    size_t   dataSize    = USBD_GET_PAYLOAD_LEN( EP3 );
    uint32_t dataAddress = ( USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR( EP3 ) );
    for ( i = 0; i < dataSize; i++ ) {
        FIFO_ByteIn( g_stVcomRx, ( uint8_t* )( dataAddress + i ) );
    }
	USBD_SET_PAYLOAD_LEN( EP3, EP3_MAX_PKT_SIZE );
}
