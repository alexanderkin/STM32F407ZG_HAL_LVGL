#include "bsp_tim.h"
#include "../../lvgl/src/hal/lv_hal_tick.h"

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

void bsp_Tim6_Init(uint32_t Prescaler)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = Prescaler;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim6);
	
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);
}

void bsp_Tim7_Init(uint16_t count)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 42;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = count;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim7);
	
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig);
	
  HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM6)
  {
    __HAL_RCC_TIM6_CLK_ENABLE();
  }
  else if(tim_baseHandle->Instance==TIM7)
  {
    __HAL_RCC_TIM7_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM6)
  {
    __HAL_RCC_TIM6_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM7)
  {
    __HAL_RCC_TIM7_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TIM7_IRQn);
  }
}

void Delay_us(uint16_t time)
{
  uint16_t differ = 0xffff - time;
	bsp_Tim6_Init(42);
  
  __HAL_TIM_SET_COUNTER(&htim6, differ);
  HAL_TIM_Base_Start(&htim6);
  
  while(differ < 0xffff - 1)
  {
  	differ = __HAL_TIM_GET_COUNTER(&htim6);
  }
  
  HAL_TIM_Base_Stop(&htim6);
}

void Delay_ms(uint16_t time)
{
	uint16_t differ = 0xffff - time;
	bsp_Tim6_Init(42000);

  __HAL_TIM_SET_COUNTER(&htim6, differ);
  HAL_TIM_Base_Start(&htim6);
  
  while(differ < 0xffff - 1)
  {
  	differ = __HAL_TIM_GET_COUNTER(&htim6);
  }
  
  HAL_TIM_Base_Stop(&htim6);
}

void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);
  HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == (&htim7))
  {
    lv_tick_inc(1);
  }
}
