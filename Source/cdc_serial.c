#include "cdc_serial.h"

static STR_VCOM_LINE_CODING gLineCoding = { 0, 0, 0, 8 };
static uint16_t             vcom_ctrlSignal;
volatile uint8_t            vcom_txReady = 1;

/*--------------------------------------------------------------------------*/

void USBD_IRQHandler( void ) {
    uint32_t u32IntSts = USBD_GET_INT_FLAG();
    uint32_t u32State  = USBD_GET_BUS_STATE();

    //------------------------------------------------------------------
    if ( u32IntSts & USBD_INTSTS_FLDET ) {
        // Floating detect
        USBD_CLR_INT_FLAG( USBD_INTSTS_FLDET );

        if ( USBD_IS_ATTACHED() ) {
            /* USB Plug In */
            USBD_ENABLE_USB();
        }
        else {
            /* USB Un-plug */
            USBD_DISABLE_USB();
        }
    }

    //------------------------------------------------------------------
    if ( u32IntSts & USBD_INTSTS_BUS ) {
        /* Clear event flag */
        USBD_CLR_INT_FLAG( USBD_INTSTS_BUS );

        if ( u32State & USBD_STATE_USBRST ) {
            /* Bus reset */
            USBD_ENABLE_USB();
            USBD_SwReset();
        }
        if ( u32State & USBD_STATE_SUSPEND ) {
            /* Enable USB but disable PHY */
            USBD_DISABLE_PHY();
        }
        if ( u32State & USBD_STATE_RESUME ) {
            /* Enable USB and enable PHY */
            USBD_ENABLE_USB();
        }
    }

    //------------------------------------------------------------------
    if ( u32IntSts & USBD_INTSTS_USB ) {
        // USB event
        if ( u32IntSts & USBD_INTSTS_SETUP ) {
            // Setup packet
            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_SETUP );

            /* Clear the data IN/OUT ready flag of control end-points */
            USBD_STOP_TRANSACTION( EP0 );
            USBD_STOP_TRANSACTION( EP1 );

            USBD_ProcessSetupPacket();
        }
        // EP events
        if ( u32IntSts & USBD_INTSTS_EP0 ) {

            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_EP0 );

            // control IN
            USBD_CtrlIn();
        }
        if ( u32IntSts & USBD_INTSTS_EP1 ) {

            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_EP1 );

            // control OUT
            USBD_CtrlOut();
        }
        if ( u32IntSts & USBD_INTSTS_EP2 ) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_EP2 );
            // Bulk IN
            EP2_Handler();
        }
        if ( u32IntSts & USBD_INTSTS_EP3 ) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_EP3 );
            // Bulk Out
            EP3_Handler();
        }
        if ( u32IntSts & USBD_INTSTS_EP4 ) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_EP4 );
        }
        if ( u32IntSts & USBD_INTSTS_EP5 ) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_EP5 );
        }
        if ( u32IntSts & USBD_INTSTS_EP6 ) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_EP6 );
        }
        if ( u32IntSts & USBD_INTSTS_EP7 ) {
            /* Clear event flag */
            USBD_CLR_INT_FLAG( USBD_INTSTS_EP7 );
        }
    }
    /* clear unknown event */
    USBD_CLR_INT_FLAG( u32IntSts );
}
void VCOM_Init( void ) {
    /* Init setup packet buffer */
    /* Buffer for setup packet -> [0 ~ 0x7] */
    USBD->STBUFSEG = SETUP_BUF_BASE;

    /*****************************************************/
    /* EP0 ==> control IN endpoint, address 0 */
    USBD_CONFIG_EP( EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0 );
    /* Buffer range for EP0 */
    USBD_SET_EP_BUF_ADDR( EP0, EP0_BUF_BASE );

    /* EP1 ==> control OUT endpoint, address 0 */
    USBD_CONFIG_EP( EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0 );
    /* Buffer range for EP1 */
    USBD_SET_EP_BUF_ADDR( EP1, EP1_BUF_BASE );

    /*****************************************************/
    /* EP2 ==> Bulk IN endpoint, address 1 */
    USBD_CONFIG_EP( EP2, USBD_CFG_EPMODE_IN | BULK_IN_EP_NUM );
    /* Buffer offset for EP2 */
    USBD_SET_EP_BUF_ADDR( EP2, EP2_BUF_BASE );

    /* EP3 ==> Bulk Out endpoint, address 2 */
    USBD_CONFIG_EP( EP3, USBD_CFG_EPMODE_OUT | BULK_OUT_EP_NUM );
    /* Buffer offset for EP3 */
    USBD_SET_EP_BUF_ADDR( EP3, EP3_BUF_BASE );
    /* trigger receive OUT data */
    USBD_SET_PAYLOAD_LEN( EP3, EP3_MAX_PKT_SIZE );

    /* EP4 ==> Interrupt IN endpoint, address 3 */
    USBD_CONFIG_EP( EP4, USBD_CFG_EPMODE_IN | INT_IN_EP_NUM );
    /* Buffer offset for EP4 ->  */
    USBD_SET_EP_BUF_ADDR( EP4, EP4_BUF_BASE );
}
void VCOM_ClassRequest( void ) {
    uint8_t buf[ 8 ];

    USBD_GetSetupPacket( buf );

    if ( buf[ 0 ] & 0x80 ) /* request data transfer direction */
    {
        // Device to host
        switch ( buf[ 1 ] ) {
        case GET_LINE_CODE: {
            /* VCOM-1 */
            if ( buf[ 4 ] == 0 ) {
                USBD_MemCopy( ( uint8_t* )( USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR( EP0 ) ), ( uint8_t* )&gLineCoding, 7 );
            }
            /* Data stage */
            USBD_SET_DATA1( EP0 );
            USBD_SET_PAYLOAD_LEN( EP0, 7 );
            /* Status stage */
            USBD_PrepareCtrlOut( 0, 0 );
            break;
        }
        default: {
            /* Setup error, stall the device */
            USBD_SetStall( EP0 );
            USBD_SetStall( EP1 );
            break;
        }
        }
    }
    else {
        // Host to device
        switch ( buf[ 1 ] ) {
        case SET_CONTROL_LINE_STATE: {
            /* VCOM-1 */
            if ( buf[ 4 ] == 0 ) {
                vcom_ctrlSignal = buf[ 2 ];
            }

            /* Status stage */
            USBD_SET_DATA1( EP0 );
            USBD_SET_PAYLOAD_LEN( EP0, 0 );
            break;
        }
        case SET_LINE_CODE: {
            if ( buf[ 4 ] == 0 ) /* VCOM-1 */
                USBD_PrepareCtrlOut( ( uint8_t* )&gLineCoding, 7 );

            /* Status stage */
            USBD_SET_DATA1( EP0 );
            USBD_SET_PAYLOAD_LEN( EP0, 0 );

            break;
        }
        default: {
            // Stall
            /* Setup error, stall the device */
            USBD_SetStall( EP0 );
            USBD_SetStall( EP1 );
            break;
        }
        }
    }
}
void VCOM_Tx( uint8_t* buf, size_t size ) {
    size_t sendSize;
    while ( size > 0 ) {
        while ( vcom_txReady == 0 ) {};
        sendSize = ( size > EP2_MAX_PKT_SIZE ) ? EP2_MAX_PKT_SIZE : size;
        USBD_MemCopy( ( uint8_t* )( USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR( EP2 ) ), buf, sendSize );
        USBD_SET_PAYLOAD_LEN( EP2, sendSize );
        vcom_txReady = 0;
        buf          = ( uint8_t* )( ( size_t )buf + sendSize );
        size -= sendSize;
    }
    if ( USBD_GET_PAYLOAD_LEN( EP2 ) == EP2_MAX_PKT_SIZE ) {
        while ( vcom_txReady == 0 ) {};
        USBD_SET_PAYLOAD_LEN( EP2, 0 );
    }
}

void VCOM_TxFifo( FIFO_T* fifo ) {
    while ( FIFO_IsEmpty( fifo ) == false ) {
			size_t  sendSize = 0; 
    uint8_t temp[ EP2_MAX_PKT_SIZE ] = {0};
        while ( sendSize < EP2_MAX_PKT_SIZE && FIFO_IsEmpty( fifo ) == false ) {
            FIFO_ByteOut( fifo, temp+sendSize );
            sendSize++;
        }
        VCOM_Tx(temp, sendSize);
    }
}

int vcomPrintf( const char* fmt, ... ) {
    int     i;
    char    buf[ 256 ];
    va_list aptr;
    va_start( aptr, fmt );
    i = vsprintf( buf, fmt, aptr );
    va_end( aptr );
    VCOM_Tx( ( uint8_t* )buf, i );
    return i;
}
