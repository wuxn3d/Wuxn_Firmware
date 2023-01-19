#include "includes.h"

static uint8_t fac_us=0;

void Delay_init(void)
{
  SysTick->CTRL&=0xfffffffb;  
  fac_us=mcuClocks.rccClocks.HCLK_Frequency/(8*1000000); 
}

void Delay_us(uint32_t us)					
{
  uint32_t temp;
  SysTick->LOAD=us*fac_us;  		
  SysTick->VAL=0x00;						
  SysTick->CTRL=0x01;						
  do
  {
    temp=SysTick->CTRL;
  }while(temp&0x01&&!(temp&(1<<16)));	 	
  SysTick->CTRL=0x00;						
  SysTick->VAL=0x00;						
}

void Delay_ms(uint16_t ms)
{
  for (uint16_t i = 0; i < ms; i++) {
    Delay_us(1000);
  }
}
