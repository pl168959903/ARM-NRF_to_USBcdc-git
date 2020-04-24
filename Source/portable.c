
#include "Project.h"

//-----------------------------------------------------------------------------
//NRF
uint8_t SPI1_WriteByte( uint8_t data ) {
    SPI_WRITE_TX0(SPI1, data);
	SPI_TRIGGER(SPI1);
	while(SPI_IS_BUSY(SPI1)){};
    return ( uint8_t )( SPI_READ_RX0(SPI1) );
}
uint8_t SPI1_ReadByte( void ) {
    SPI_TRIGGER(SPI1);
    while(SPI_IS_BUSY(SPI1)){};
    return ( uint8_t )( SPI_READ_RX0(SPI1) );
}
void SPI1_SS( void ) {
    SPI_SET_SS0_LOW(SPI1);
}
void SPI1_DSS( void ) {
    SPI_SET_SS0_HIGH(SPI1);
}
void SPI1_Dummy( void ) {
    SPI_TRIGGER( USER_CFG_NRF24L01_0_SPI );
    while ( SPI_IS_BUSY( USER_CFG_NRF24L01_0_SPI ) ){};
}
SPI_Func_T g_stSpi1 = { SPI1_WriteByte, SPI1_ReadByte, SPI1_SS, SPI1_DSS, SPI1_Dummy };

void NrfDelay(uint32_t time){
    DelayUs(time);
}

void SetCE(void){
    PA15 = 1;
    DelayUs(10);
}

void ResetCE(void){
    PA15 = 0;
}
