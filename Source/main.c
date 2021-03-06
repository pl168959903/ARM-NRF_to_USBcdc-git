
#include "project.h"

void printReg( void );

void CmdDecoder( void );
void CmdSend( uint8_t length );

int main( void ) {
    g_stVcomRx = FIFO_New( 1024, NULL );
    SYS_UnlockReg();
    ClkSetup();
    PinSetup();
    UartSetup();
    SpiSetup();
    GpioSetup();
    UsbSetup();
    NVIC_Init();
    SYS_LockReg();
    USBD_Start();
    do{
        FIFO_Rst( g_stVcomRx );
        DelayUs(100000);
    }while(FIFO_CmdCheck( g_stVcomRx, ( uint8_t* )"CMD:START", 0, 9, 9, 0, false ) == false);
    vcomPrintf( "SYS:OK\n" );
    FIFO_Rst( g_stVcomRx );

    
    NrfSetup();
    vcomPrintf( "SYS:NRF_READY\n" );
    //NRF_RegPrintf( g_stNrf0 );

    //------------------------------------------------
    // NRF
    while ( 1 ) {
        if ( !FIFO_IsEmpty( g_stNrfRx ) ) {
            VCOM_TxFifo( g_stNrfRx );
        }
        CmdDecoder();
    }
}

void CmdDecoder( void ) {
    if ( FIFO_IsEmpty( g_stVcomRx ) == false ) {
        uint32_t c_time = 0;
        while ( c_time++ < 500 ) {
            if ( FIFO_CmdCheck( g_stVcomRx, ( uint8_t* )"CMD:", 0, 4, 4, 0, false ) ) {
                if ( FIFO_CmdCheck( g_stVcomRx, ( uint8_t* )"GET:", 4, 8, 4, 0, false ) ) {
                    if ( FIFO_CmdCheck( g_stVcomRx, ( uint8_t* )"IMG", 8, 11, 3, 0, false ) ) {
                        if ( FIFO_WaitData( g_stVcomRx, 11, 0 ) ) {
                            CmdSend( 11 );
                        }
                    }
                    if ( FIFO_CmdCheck( g_stVcomRx, ( uint8_t* )"GPS", 8, 11, 3, 0, false ) ) {
                        if ( FIFO_WaitData( g_stVcomRx, 11, 0 ) ) {
                            CmdSend( 11 );
                        }
                    }
                }
                if ( FIFO_CmdCheck( g_stVcomRx, ( uint8_t* )"SET:", 4, 8, 4, 0, false ) ) {
                    if ( FIFO_CmdCheck( g_stVcomRx, ( uint8_t* )"CYC:", 8, 12, 4, 0, false ) ) {
                         if ( FIFO_WaitData( g_stVcomRx, 16, 0 ) ) {
                            CmdSend( 16 );
                        }
                    }
                }
            }
            DelayUs( 1000 );
        }
        FIFO_Rst( g_stVcomRx );
    }
}

void CmdSend( uint8_t length ) {
    uint8_t f;
    FIFO_WaitData( g_stVcomRx, length, 0 );
    NRF_TxMode( g_stNrf0 );
    f = nrfP2P_SendPacket( g_stNrf0, ( uint8_t* )CameraAddr, g_stVcomRx->buf, length );
    NRF_RxMode( g_stNrf0 );
    FIFO_Rst( g_stVcomRx );
    vcomPrintf( "SYS:" );
    if ( f == true )
        vcomPrintf( "OK\n" );
    else
        vcomPrintf( "TOUT\n" );
}
