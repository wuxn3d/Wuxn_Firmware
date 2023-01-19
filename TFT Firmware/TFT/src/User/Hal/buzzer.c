#include "buzzer.h"
#include "GPIO_Init.h"
#include "includes.h"

#ifdef BUZZER_PIN

void TIM3_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC->APB1ENR |= RCC_APB1Periph_TIM3;
  TIM3->CR1 &= ~TIM_CR1_CEN;
  TIM3->DIER |= TIM_DIER_UIE;
  TIM3->SR &= ~TIM_SR_UIF;
  TIM3->ARR = mcuClocks.PCLK1_Timer_Frequency / 1000000 - 1;  
}

void Buzzer_Config(void)
{
  GPIO_InitSet(BUZZER_PIN, MGPIO_MODE_OUT_PP, 0);
  TIM3_Config();
}

void Buzzer_DeConfig(void)
{
  GPIO_InitSet(BUZZER_PIN, MGPIO_MODE_IPN, 0);
}

static BUZZER buzzer;
volatile uint32_t buzzerEndTime = 0;
volatile uint32_t toggles = 0;

void tone(const uint16_t frequency, const uint16_t duration);

void loopBuzzer(void)
{
  if (!buzzerEndTime)
  {
    if (buzzer.count == 0) return;
    buzzerEndTime = OS_GetTimeMs() + buzzer.duration[buzzer.rIndex];
    if (buzzer.frequency[buzzer.rIndex] > 0)
    {
      tone(buzzer.frequency[buzzer.rIndex], buzzer.duration[buzzer.rIndex]);
    }
    buzzer.rIndex = (buzzer.rIndex + 1) % BUZZER_CACHE_SIZE;
    buzzer.count--;
  }
  else if (OS_GetTimeMs() > buzzerEndTime && toggles == 0)
  {
    buzzerEndTime = 0;
    TIM3->CR1 &= ~TIM_CR1_CEN;  
    GPIO_SetLevel(BUZZER_PIN, BUZZER_STOP_LEVEL);
  }
}

void Buzzer_TurnOn(const uint16_t frequency, const uint16_t duration)
{
  while (buzzer.count == BUZZER_CACHE_SIZE)
  {
    loopBuzzer();
  }
  buzzer.duration[buzzer.wIndex] = duration;
  buzzer.frequency[buzzer.wIndex] = frequency;
  buzzer.wIndex = (buzzer.wIndex + 1) % BUZZER_CACHE_SIZE;
  buzzer.count++;
}

void Buzzer_play(SOUND sound)
{
  
    
  switch (sound)
  {
    case sound_ok:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(523, 50);
      Buzzer_TurnOn(0, 40);
      Buzzer_TurnOn(689, 50);
      break;

    case sound_success:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(523, 50);
      Buzzer_TurnOn(0, 40);
      Buzzer_TurnOn(523, 50);
      Buzzer_TurnOn(0, 80);
      Buzzer_TurnOn(784, 50);
      break;

    case sound_cancel:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(689, 50);
      Buzzer_TurnOn(0, 40);
      Buzzer_TurnOn(523, 50);
      break;

    case sound_notify:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(659, 50);
      Buzzer_TurnOn(0, 40);
      Buzzer_TurnOn(698, 50);
      break;

    case sound_error:
      Buzzer_TurnOn(147, 50);
      Buzzer_TurnOn(0, 25);
      Buzzer_TurnOn(147, 50);
      Buzzer_TurnOn(0, 25);
      Buzzer_TurnOn(147, 50);
      break;

    case sound_deny:
      if (infoSettings.alertSound != 1)
        break;
      Buzzer_TurnOn(131, 200);
      break;

    case sound_toast:
      if (infoSettings.toastSound != 1)
        break;
      Buzzer_TurnOn(147, 50);
      Buzzer_TurnOn(0, 15);
      Buzzer_TurnOn(147, 50);
      Buzzer_TurnOn(0, 25);
      Buzzer_TurnOn(131, 50);
      break;

    case sound_heated:
      if (infoSettings.heaterSound != 1)
        break;
      Buzzer_TurnOn(262, 50);
      Buzzer_TurnOn(0, 20);
      Buzzer_TurnOn(262, 50);
      Buzzer_TurnOn(0, 40);
      Buzzer_TurnOn(262, 50);
      break;

    case sound_cooled:
      if (infoSettings.heaterSound != 1)
        break;
      Buzzer_TurnOn(392, 50);
      Buzzer_TurnOn(0, 20);
      Buzzer_TurnOn(392, 50);
      Buzzer_TurnOn(0, 40);
      Buzzer_TurnOn(262, 50);
      break;

    #ifdef WUXN_CODE
    case sound_connected:
      Buzzer_TurnOn(262,150);
      Buzzer_TurnOn(  0, 75);
      Buzzer_TurnOn(262,150);
      Buzzer_TurnOn(  0, 75);
      Buzzer_TurnOn(392,150);
      Buzzer_TurnOn(  0, 75);
      Buzzer_TurnOn(392,150);
      break;
    #endif
  
    case sound_interval:
      Buzzer_TurnOn(  0,50);  
      break;

    case sound_keypress:
    default:
      if (infoSettings.touchSound != 1)
        break;
      Buzzer_TurnOn(BUZZER_FREQUENCY_HZ, BUZZER_FREQUENCY_DURATION_MS);
      break;
  }
}

void tone(const uint16_t frequency, const uint16_t duration)
{
  if (frequency == 0 || duration == 0) return;

  NVIC_DisableIRQ(TIM3_IRQn);
  toggles = 2 * (frequency * duration / 1000);  

  TIM3->CR1 &= ~TIM_CR1_CEN;
  TIM3->CNT =0;
  TIM3->PSC = (1000000 / (2 * frequency)) - 1;
  TIM3->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void)
{
  if ((TIM3->SR & TIM_SR_UIF) != 0)  
  {
    if (toggles != 0)
    {
      toggles--;
      GPIO_ToggleLevel(BUZZER_PIN);
    }
    else
    {
      TIM3->CR1 &= ~TIM_CR1_CEN;  
    }
    TIM3->SR &= ~TIM_SR_UIF;  
  }
}

#endif
