#include <gd32f1x0.h>

#include "drv_pwm.h"
#include "defines.h"
#include "config.h"
#include "hardware.h"

#ifdef USE_PWM_DRIVER

TIMER_OCInitPara TIM_OCInitStructure;


// CHANGING THE H-BRIDGE CODE CAN RESULT IN CONNECTING THE FETs ACROSS THE BATTERY
// AND AS SUCH BRAKING THE BOARD


// CENTER ALIGNED PWM METHOD

// set in config.h 

//#define PWM_490HZ
//#define PWM_8KHZ_OLD
//#define PWM_8KHZ
//#define PWM_16KHZ
//#define PWM_24KHZ

// 490Khz
#ifdef PWM_490HZ
#define PWMTOP 16383
#define TIMER_PRESCALER 3
#endif


// 8Khz - ch div 3
#ifdef PWM_8KHZ_OLD
#define PWMTOP 1023
#define TIMER_PRESCALER 3
#endif

// 8Khz
#ifdef PWM_8KHZ
#define PWMTOP 3072
#define TIMER_PRESCALER 1
#endif

// 16Khz
#ifdef PWM_16KHZ
#define PWMTOP 1535
#define TIMER_PRESCALER 1
#endif

// 24Khz
#ifdef PWM_24KHZ
#define PWMTOP 1023
#define TIMER_PRESCALER 1
#endif


// 32Khz
#ifdef PWM_32KHZ
#define PWMTOP 767
#define TIMER_PRESCALER 1
#endif

void pwm_init(void)
{
	GPIO_InitPara GPIO_InitStructure;


//pin A0 TM2_ch1
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_50MHZ;
	GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLDOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE1, GPIO_AF_2);


	TIMER_BaseInitPara TIM_TimeBaseStructure;

	RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2 | 
															RCC_APB1PERIPH_TIMER14 |
																RCC_APB1PERIPH_TIMER3, ENABLE);
	
	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1, ENABLE);

// timer 2
	TIM_TimeBaseStructure.TIMER_Prescaler = TIMER_PRESCALER - 1;	//
	TIM_TimeBaseStructure.TIMER_CounterMode = TIMER_COUNTER_CENTER_ALIGNED2;
	TIM_TimeBaseStructure.TIMER_Period = PWMTOP;
	TIM_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
// init timer 2
	TIMER_BaseInit(TIMER2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIMER_OCMode = TIMER_OC_MODE_PWM1;
	TIM_OCInitStructure.TIMER_OCPolarity = TIMER_OC_POLARITY_HIGH;
	TIM_OCInitStructure.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE;
	TIM_OCInitStructure.TIMER_OCIdleState = TIMER_OC_IDLE_STATE_RESET;

	TIM_OCInitStructure.TIMER_Pulse = 0;

	TIMER_OC2_Init(TIMER2, &TIM_OCInitStructure);

	TIMER_CtrlPWMOutputs(TIMER2, ENABLE);

	TIMER_CARLPreloadConfig(TIMER2, ENABLE);


	TIMER_OC2_Preload(TIMER2, TIMER_OC_PRELOAD_DISABLE);


	// pin A3

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE3, GPIO_AF_2);

	TIMER_OC4_Init(TIMER2, &TIM_OCInitStructure);


	TIMER_OC4_Preload(TIMER2, TIMER_OC_PRELOAD_DISABLE);

//   TIMER_Enable( TIMER2, ENABLE );

// timer 1 ch3 , ch1 
// pins A10 , A8         
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_10 | GPIO_PIN_8;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE10, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE8, GPIO_AF_2);


// timer 1

// init timer 1
	TIMER_BaseInit(TIMER1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIMER_Pulse = 0;

	TIMER_OC3_Init(TIMER1, &TIM_OCInitStructure);

	TIMER_OC1_Init(TIMER1, &TIM_OCInitStructure);

	TIMER_CtrlPWMOutputs(TIMER1, ENABLE);

	TIMER_CARLPreloadConfig(TIMER1, ENABLE);


	TIMER_OC3_Preload(TIMER1, TIMER_OC_PRELOAD_DISABLE);

	TIMER_OC1_Preload(TIMER1, TIMER_OC_PRELOAD_DISABLE);


// bridge dir 2
///////////////

	//pin A9 TM1_ch2
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_9;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE9, GPIO_AF_2);

// timer 1

// init timer 1
	TIMER_BaseInit(TIMER1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIMER_Pulse = 0;

	TIMER_OC2_Init(TIMER2, &TIM_OCInitStructure);

	TIMER_CtrlPWMOutputs(TIMER1, ENABLE);

	TIMER_CARLPreloadConfig(TIMER1, ENABLE);


	TIMER_OC2_Preload(TIMER1, TIMER_OC_PRELOAD_DISABLE);



// timer 3 PB1 af1 ch4

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE1, GPIO_AF_1);


// timer 3

// init timer 3
	TIMER_BaseInit(TIMER3, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIMER_Pulse = 0;

	TIMER_OC4_Init(TIMER3, &TIM_OCInitStructure);

	TIMER_CtrlPWMOutputs(TIMER3, ENABLE);

	TIMER_CARLPreloadConfig(TIMER3, ENABLE);


	TIMER_OC4_Preload(TIMER3, TIMER_OC_PRELOAD_DISABLE);

// A0
// Timer2 ch1

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_0;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE0, GPIO_AF_2);


// timer 2

// init timer 2
	TIMER_BaseInit(TIMER2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIMER_Pulse = 0;

	TIMER_OC1_Init(TIMER2, &TIM_OCInitStructure);

	TIMER_CtrlPWMOutputs(TIMER2, ENABLE);

	TIMER_CARLPreloadConfig(TIMER2, ENABLE);


	TIMER_OC1_Preload(TIMER2, TIMER_OC_PRELOAD_DISABLE);

// PA2 tm2 ch3

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_2;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE2, GPIO_AF_2);


	TIMER_OC3_Init(TIMER2, &TIM_OCInitStructure);

	TIMER_OC3_Preload(TIMER2, TIMER_OC_PRELOAD_DISABLE);



	TIMER_Enable(TIMER3, ENABLE);

	TIMER_Enable(TIMER1, ENABLE);

	TIMER_Enable(TIMER2, ENABLE);


}

int pwmdir = 0;

void pwm_set_reverse(uint8_t number, float pwm);

void pwm_set_forward(uint8_t number, float pwm);



extern int failsafe;
extern float rx[];
unsigned long motorbeeptime = 0;

#include "drv_time.h"

#ifndef MOTOR_BEEPS_TIMEOUT
// default value if not defined elsewhere
#define MOTOR_BEEPS_TIMEOUT 30e6
#endif

#define MOTOR_BEEPS_PWM_ON 0.2
#define MOTOR_BEEPS_PWM_OFF 0.0

#include  "drv_pwm.h"
#include <stdlib.h>
void motorbeep( void)
{
	if (failsafe)
	{
		unsigned long time = gettime();
		if (!motorbeeptime)
				motorbeeptime = time;
		else
			if ( time - motorbeeptime > MOTOR_BEEPS_TIMEOUT)
			{
				if ((time%2000000 < 125000))
				{
					
					for ( int i = 0 ; i <= 3 ; i++)
						{
						pwm_set( i , MOTOR_BEEPS_PWM_ON);
							delay(50);
						pwm_set( i , MOTOR_BEEPS_PWM_OFF);
							delay(50);
						pwm_set( i , MOTOR_BEEPS_PWM_ON);
							delay(50);
						pwm_set( i , MOTOR_BEEPS_PWM_OFF);
							delay(50);
						pwm_set( i , MOTOR_BEEPS_PWM_ON);
							delay(50);
						pwm_set( i , MOTOR_BEEPS_PWM_OFF);
											
						}
				
				}
				else
				{
				for ( int i = 0 ; i <= 3 ; i++)
					{
					pwm_set( i , MOTOR_BEEPS_PWM_OFF);
					}
					
				}
				
			}
	}
	else
		motorbeeptime = 0;
}


void pwm_set(uint8_t number, float pwm)
{

	if (pwmdir == REVERSE)
	  {
		  pwm_set_reverse(number, pwm);
	  }
	if (pwmdir == FORWARD)
	  {
		  pwm_set_forward(number, pwm);
	  }


}


void pwm_set_forward(uint8_t number, float pwm)
{
	pwm = pwm * PWMTOP;

	if (pwm < 0)
		pwm = 0;
	if (pwm > PWMTOP)
		pwm = PWMTOP;

	TIM_OCInitStructure.TIMER_Pulse = (uint32_t) pwm;
	switch (number)
	  {
	  case 0:		// fr
		  TIMER_OC2_Init(TIMER2, &TIM_OCInitStructure);
		  break;

	  case 1:		// br
		  TIMER_OC4_Init(TIMER2, &TIM_OCInitStructure);
		  break;

	  case 2:		// fl
		  TIMER_OC3_Init(TIMER1, &TIM_OCInitStructure);
		  break;

	  case 3:		// bl
		  TIMER_OC1_Init(TIMER1, &TIM_OCInitStructure);
		  break;

	  default:
		  // handle error;
		  //
		  break;

	  }

}


void pwm_set_reverse(uint8_t number, float pwm)
{
	pwm = pwm * PWMTOP;

	if (pwm < 0)
		pwm = 0;
	if (pwm > PWMTOP)
		pwm = PWMTOP;

	TIM_OCInitStructure.TIMER_Pulse = (uint32_t) pwm;
	switch (number)
	  {
	  case 2:		// FR ok
		  TIMER_OC2_Init(TIMER1, &TIM_OCInitStructure);
		  break;

	  case 3:		// BR ok
		  TIMER_OC4_Init(TIMER3, &TIM_OCInitStructure);
		  break;

	  case 0:		// FL ok
		  TIMER_OC1_Init(TIMER2, &TIM_OCInitStructure);
		  break;

	  case 1:		// BL ok        
		  TIMER_OC3_Init(TIMER2, &TIM_OCInitStructure);
		  break;

	  default:
		  // handle error;
		  //
		  break;

	  }

}


void pwm_dir(int dir)
{
	if (dir == FORWARD)
	  {
		  pwmdir = FORWARD;
		  for (int i = 0; i <= 3; i++)
			  pwm_set_reverse(i, 0.0f);
		  GPIO_WriteBit(GPIOA, GPIO_PIN_4, Bit_RESET);	// bridge dir 2
		  GPIO_WriteBit(GPIOF, GPIO_PIN_1, Bit_SET);	// bridge dir 1

	  }

	if (dir == REVERSE)
	  {
		  pwmdir = REVERSE;
		  for (int i = 0; i <= 3; i++)
			  pwm_set_forward(i, 0.0f);
		  GPIO_WriteBit(GPIOF, GPIO_PIN_1, Bit_RESET);	// bridge dir 1
		  GPIO_WriteBit(GPIOA, GPIO_PIN_4, Bit_SET);	// bridge dir 2
	  }
	if (dir == FREE)
	  {
		  for (int i = 0; i <= 3; i++)
		    {
			    pwm_set_reverse(i, 0.0f);
			    pwm_set_forward(i, 0.0f);
		    }
		  GPIO_WriteBit(GPIOF, GPIO_PIN_1, Bit_RESET);	// bridge dir 1
		  GPIO_WriteBit(GPIOA, GPIO_PIN_4, Bit_RESET);	// bridge dir 2
	  }
	if (dir == BRAKE)
	  {
		  /*
		     for ( int i = 0 ; i <= 3; i++)
		     {
		     pwm_set_forward( i , 0.0f );
		     pwm_set_reverse( i , 0.0f );
		     }
		     delay(100);
		     GPIO_WriteBit(GPIOF, GPIO_PIN_1, Bit_SET); // bridge dir 1
		     GPIO_WriteBit(GPIOA, GPIO_PIN_4, Bit_SET); // bridge dir 2
		   */
	  }

}

#endif
