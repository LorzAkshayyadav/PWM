#include <stdint.h>
#include "stm32f4xx_hal.h"
UART_HandleTypeDef huart3;
TIM_HandleTypeDef htim4;
uint8_t x=0;
int _write(int file, char *ptr, int len) {
	//HAL_UART_Transmit(&huart3, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    for (int i = 0; i < len; i++) {
        ITM_SendChar(ptr[i]);
    }
    return len;
}
void uart_init(){
	//give clock access to pd8 and pd9
	__HAL_RCC_GPIOD_CLK_ENABLE();
	//initialize gpio struct
	GPIO_InitTypeDef GPIO_INIT = {0};
	//set port to af mode
	GPIO_INIT.Pin=GPIO_PIN_8|GPIO_PIN_9;
	GPIO_INIT.Mode = GPIO_MODE_AF_PP;
	//set af type
	GPIO_INIT.Alternate = GPIO_AF7_USART3;
	//set freq
	GPIO_INIT.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	//enable gpio
	HAL_GPIO_Init(GPIOD,&GPIO_INIT);
	//give clock access to usart3
	__HAL_RCC_USART3_CLK_ENABLE();
	//define baudrate
	huart3.Instance=USART3;
	huart3.Init.BaudRate=9600;
	//define parrity bits
	huart3.Init.Parity=UART_PARITY_NONE;
	//define wordlength
	huart3.Init.Mode=UART_MODE_TX_RX ;
	//define stop bit
	huart3.Init.StopBits=UART_STOPBITS_1;
	// enable interupt
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.OverSampling =UART_OVERSAMPLING_16   ;
	HAL_NVIC_EnableIRQ(USART3_IRQn);
	//enable usart
	HAL_UART_Init(&huart3);
	//call recieve interrupt fn
	HAL_UART_Receive_IT(&huart3, &x, 1);
}
void pwm_init() {
    GPIO_InitTypeDef GPIO_INIT = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    // Enable clocks
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    // Configure PD12 (TIM4_CH1)
    GPIO_INIT.Pin = GPIO_PIN_12;
    GPIO_INIT.Mode = GPIO_MODE_AF_PP;
    GPIO_INIT.Pull = GPIO_NOPULL;
    GPIO_INIT.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_INIT.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_INIT);

    // Configure TIM4 for PWM
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 16 - 1;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 1000 - 1;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim4);

    // Configure PWM Channel 1
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1);

    // Start PWM on Channel 1
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}

int main(void) {
    HAL_Init();
    pwm_init();
    uart_init();
    while (1) {
        // You can update duty cycle here using:
       //  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, new_pulse_value);

    }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance==USART3)
	{
		int d = x-'0';
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, d*100);
		HAL_UART_Receive_IT(&huart3, &x, 1);
	}
}
void USART3_IRQHandler(){
	HAL_UART_IRQHandler(&huart3);
}

// Required for HAL timing
void SysTick_Handler() {
    HAL_IncTick();
}
