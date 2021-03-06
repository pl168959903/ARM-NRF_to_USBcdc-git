
#include "project.h"

// VCOM
FIFO_T* g_stVcomRx;

// NRF
FIFO_T*           g_stNrfRx;
NRF_T*            g_stNrf0;
NRFP2P_CHANNEL_T* g_stNrfP1;
const uint8_t     p1Addr[ 5 ]     = { 0x00, 0x79, 0x02, 0x08, 0x11 };
const uint8_t     CameraAddr[ 5 ] = { 0x01, 0x79, 0x02, 0x08, 0x11 };

void PinSetup( void ) {
    // UART0
    SYS->GPB_MFP &= ~( SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk );
    SYS->GPB_MFP |= ( SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD );

    // SPI1
    SYS->GPC_MFP |= ( SYS_GPC_MFP_PC8_SPI1_SS0 | SYS_GPC_MFP_PC9_SPI1_CLK | SYS_GPC_MFP_PC10_SPI1_MISO0 | SYS_GPC_MFP_PC11_SPI1_MOSI0 );
    SYS->ALT_MFP |= ( SYS_ALT_MFP_PC8_SPI1_SS0 | SYS_ALT_MFP_PC9_SPI1_CLK | SYS_ALT_MFP_PC10_SPI1_MISO0 | SYS_ALT_MFP_PC11_SPI1_MOSI0 );

    // NRF INT
    SYS->GPB_MFP |= SYS_GPB_MFP_PB15_INT1;
}
void ClkSetup( void ) {
    // System clock
    CLK_EnableXtalRC( CLK_PWRCON_XTL12M_EN_Msk );
    while ( !CLK_WaitClockReady( CLK_CLKSTATUS_XTL12M_STB_Msk ) ) {};
    CLK_SetCoreClock( PLL_CLOCK );

    // Uart clock
    CLK_SetModuleClock( UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART( 1 ) );

    // SPI clock
    CLK_SetModuleClock( SPI1_MODULE, CLK_CLKSEL1_SPI1_S_PLL, MODULE_NoMsk );

    // USB clock
    CLK_SetModuleClock( USBD_MODULE, 0, CLK_CLKDIV_USB( 3 ) );
}
void GpioSetup( void ) {
    GPIO_SetMode( PB, BIT2, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PB, BIT3, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PD, BIT6, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PD, BIT7, GPIO_PMD_OUTPUT );
    GPIO_SetMode( PB, BIT15, GPIO_PMD_QUASI );
    GPIO_SetMode( PA, BIT15, GPIO_PMD_OUTPUT );  // NRF_CE

    GPIO_EnableInt( PB, 15, GPIO_INT_FALLING );
}
void SpiSetup( void ) {
    CLK_EnableModuleClock( SPI1_MODULE );
    SPI_Open( SPI1, SPI_MASTER, SPI_MODE_0, 8, 10000000 );
}
void UartSetup( void ) {
    CLK_EnableModuleClock( UART0_MODULE );
    UART_Open( UART0, 115200 );
}
void NVIC_Init( void ) {
    NVIC_EnableIRQ( EINT1_IRQn );
    NVIC_EnableIRQ( USBD_IRQn );
    NVIC_SetPriority( USBD_IRQn, 0 );
    NVIC_SetPriority( EINT1_IRQn, 1 );
}
void UsbSetup( void ) {
    CLK_EnableModuleClock( USBD_MODULE );
    VCOM_Init();
    USBD_Open( &gsInfo, VCOM_ClassRequest, NULL );
}
void NrfSetup( void ) {
    g_stNrfRx = FIFO_New( 1024, NULL );
    g_stNrf0  = NRF_New( &g_stSpi1, &SetCE, &ResetCE, &NrfDelay );
    nrfP2P_InitNrf( g_stNrf0, 0x50 );
    g_stNrfP1 = nrfP2P_NewChannel( g_stNrf0, 1, ( uint8_t* )p1Addr, true, true );
    nrfP2P_EnableTxDypw( g_stNrf0 );
    nrfP2P_EnableTxAutoAck( g_stNrf0, ( uint8_t* )CameraAddr );
    NRF_RxMode( g_stNrf0 );
}
void DelayUs( uint32_t us ) {
    uint32_t delayMaxTime = 16777216 / ( CLK_GetCPUFreq() / 1000000 );
    do {
        if ( us > delayMaxTime ) {
            CLK_SysTickDelay( delayMaxTime );
            us -= delayMaxTime;
        }
        else {
            CLK_SysTickDelay( us );
            us = 0;
        }
    } while ( us > 0 );
}
