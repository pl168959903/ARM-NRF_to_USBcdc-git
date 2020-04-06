
#include "project.h"

FIFO_T * g_stVcomRx;

int main(void)
{ 
	g_stVcomRx = FIFO_New(128, NULL);
	SYS_UnlockReg();
	clk_init();
	pin_mode_init();
	UART_init();
	SPI_init();
	IRQ_init();
	VCOM_Init();
	SYS_LockReg();
	
	printf("SystemCoreClock : %d hz\n", SystemCoreClock);
	printf("CyclesPerUs : %d us\n", CyclesPerUs);
	printf("PllClock : %d hz\n", PllClock);
	
	
	USBD_Open(&gsInfo, VCOM_ClassRequest, NULL);
	USBD_Start();
	
	//------------------------------------------------
	uint8_t i;
	for(i=0;i<18;i++)
	{
		uint8_t tt;
		nrf_reg_read_byte(i,&tt);
		printf("reg %d : %X\n",i,tt);
	}
	//------------------------------------------------
	nrf_rx_mode();
	CLK_SysTickDelay(1000000);
	LED_set(0x0F);
	while(1)
	{
		if(vcom_txFifoSize)
		{	
			vcom_tx_trigger();
		}		
	}
}
