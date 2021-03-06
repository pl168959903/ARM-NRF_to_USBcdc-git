#include "project.h"

// uint8_t temp[ 32 ] = { 0xAA, 0xBB, 0xCC };

void EINT1_IRQHandler( void ) {
    if ( GPIO_GET_INT_FLAG( PB, BIT15 ) ) {
        NRF_Nop( g_stNrf0 );
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_RX_DR_MSK ) {
            uint8_t  i;
            uint8_t data[32];
            uint32_t len = NRF_ReadRxPayloadWide( g_stNrf0 );
            NRF_RxPayload(g_stNrf0, data, len);
            for ( i = 0; i < len; i++ ) {
                FIFO_ByteIn(g_stNrfRx, (data + i));
            }
            NRF_WriteRegByte( g_stNrf0, NRF_REG_STATUS, g_stNrf0->statusReg );
        }
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_TX_DS_MSK ) {}
        if ( g_stNrf0->statusReg & NRF_REG_STATUS_MAX_RT_MSK ) {}
        
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
