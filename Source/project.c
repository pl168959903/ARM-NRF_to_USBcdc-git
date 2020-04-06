
#include <project.h>

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
}
void UsbSetup( void ) {
    CLK_EnableModuleClock( USBD_MODULE );
}
