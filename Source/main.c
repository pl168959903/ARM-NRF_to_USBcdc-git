
#include "project.h"

const uint8_t p1Addr[ 5 ]     = { 0x00, 0x79, 0x02, 0x08, 0x11 };
const uint8_t CameraAddr[ 5 ] = { 0x01, 0x79, 0x02, 0x08, 0x11 };
uint8_t       temp[ 32 ]      = { 0xAA, 0xBB, 0xCC };

FIFO_T*           g_stVcomRx;
FIFO_T*           g_stNrfRx;
NRF_T*            g_stNrf0;
NRFP2P_CHANNEL_T* g_stNrfP1;

void printReg( void );

int main( void ) {
    g_stVcomRx = FIFO_New( 1024, NULL );
    g_stNrfRx  = FIFO_New( 1024, NULL );
    g_stNrf0   = NRF_New( &g_stSpi1, &SetCE, &ResetCE );
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

    nrfP2p_InitNrf( g_stNrf0, 0x50 );
		printReg();
    g_stNrfP1 = nrfP2p_NewChannel( g_stNrf0, 4, ( uint8_t* )p1Addr, ( uint8_t* )CameraAddr, 32, 32, true );
		NRF_RxMode( g_stNrf0 );
		printReg();
    //------------------------------------------------
    // NRF
    while ( 1 ) {
			DelayUs(5000000);
			printReg();
    };
}

void printReg( void ) {
    uint8_t i;
    uint8_t reg[5] = {0};
    printf( "--------------------------------------\n" );
    for ( i = 0; i <= 0x09; i++ ) {
        reg[0] = NRF_ReadRegByte( g_stNrf0, i );
        vcomPrintf( "Reg : 0x%02X = 0x%02X\n", i, reg[0] );
    }
		for ( i = 0x0A; i <= 0x0B; i++ ) {
        NRF_ReadRegArray( g_stNrf0, i, reg, 5 );
        vcomPrintf( "Reg : 0x%02X = 0x%02X%02X%02X%02X%02X\n", i, reg[4], reg[3], reg[2], reg[1], reg[0] );
    }
		for ( i = 0x0C; i <= 0x0F; i++ ) {
        reg[0] = NRF_ReadRegByte( g_stNrf0, i );
        vcomPrintf( "Reg : 0x%02X = 0x%02X\n", i, reg[0] );
    }
		for ( i = 0x10; i <= 0x10; i++ ) {
        NRF_ReadRegArray( g_stNrf0, i, reg, 5 );
        vcomPrintf( "Reg : 0x%02X = 0x%02X%02X%02X%02X%02X\n", i, reg[4], reg[3], reg[2], reg[1], reg[0] );
    }
		for ( i = 0x11; i <= 0x17; i++ ) {
        reg[0] = NRF_ReadRegByte( g_stNrf0, i );
        vcomPrintf( "Reg : 0x%02X = 0x%02X\n", i, reg[0] );
    }
}
