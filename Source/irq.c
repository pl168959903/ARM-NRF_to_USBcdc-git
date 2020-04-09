#include "project.h"

void EINT1_IRQHandler( void ) {
    if ( GPIO_GET_INT_FLAG( PB, BIT15 ) ) {
        NRF_Nop(g_stNrf0);
        if(g_stNrf0->statusReg & NRF_REG_STATUS_RX_DR_MSK){
					NRF_FlushRx(g_stNrf0);
					NRF_WriteRegByte(g_stNrf0, NRF_REG_STATUS, g_stNrf0->statusReg);
					vcomPrintf("RX_DR\n");
        }
        if(g_stNrf0->statusReg & NRF_REG_STATUS_TX_DS_MSK){
						vcomPrintf("TX_DS\n");
        }
        if(g_stNrf0->statusReg & NRF_REG_STATUS_MAX_RT_MSK){
						vcomPrintf("MAX_RT\n");
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
