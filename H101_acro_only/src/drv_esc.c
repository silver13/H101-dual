
// CHANGING THE H-BRIDGE CODE CAN RESULT IN CONNECTING THE FETs ACROSS THE BATTERY
// AND AS SUCH BRAKING THE BOARD

// forward / reverse function names and macros may be inaccurate



#define ESC_MIN 1200
#define ESC_MAX 1832


#define ESC_THROTTLEOFF 990

// zero = no signal
#define ESC_FAILSAFE 0


// output polarity ( low - motor output with pullup resistor (500 ohms or near) )
// enable for motor output after fets 
#define INVERTED_PWM

// 50 - 500 Hz range
#define ESC_FREQ 500

// enable preload - less noise in esc output but longer latency
#define PRELOAD_ENABLE


//#define ONESHOT_125_ENABLE



// enable for bi-directional control
//#define ENABLE_REVERSE


#define ESC_REVERSE_MAX 1148
#define ESC_REVERSE_MIN 1440

#define ESC_THROTTLEOFF_3D 1488

#define ESC_FORWARD_MIN 1532
#define ESC_FORWARD_MAX 1832

#define ESC_FAILSAFE_3D 0





#ifdef ONESHOT_125_ENABLE
#undef ESC_FREQ
// frequency for oneshot 125 , 1000 seems best as it matches the internal loop (async)
// 50 - 4000 Hz range
#define ESC_FREQ 1000
#endif





///////////////////////////////////////
///////////////////////////////////////
// code start
#define TIMER_PRESCALER 16

#ifndef SYS_CLOCK_FREQ_HZ
#define SYS_CLOCK_FREQ_HZ 48000000
#endif


// max pulse width in microseconds (auto calculated)
#define ESC_uS ((float)1000000.0f/(float)ESC_FREQ)

#define PWMTOP ((SYS_CLOCK_FREQ_HZ/(TIMER_PRESCALER) / ESC_FREQ ) - 1)

//#define PWMTOP_US ( ESC_uS )

#define PWMTOP_US ( (float)1000000.0f/((SYS_CLOCK_FREQ_HZ/(TIMER_PRESCALER))/(PWMTOP + 1))  )


#if ( PWMTOP > 65535 )
#error "pwmtop too high"
#endif


// output polarity ( low - motor output with pullup resistor (500 ohms or near) )
// choice of TIMER_OC_POLARITY_LOW / TIMER_OC_POLARITY_HIGH
#ifdef INVERTED_PWM
// for motor output after fets
#define OUT_POLARITY TIMER_OC_POLARITY_LOW
#else
// for output before fets
#define OUT_POLARITY TIMER_OC_POLARITY_HIGH
#endif

#ifdef PRELOAD_ENABLE
// enable preload
#define ESC_PRELOAD TIMER_OC_PRELOAD_ENABLE
#else
// disable preload
#define ESC_PRELOAD TIMER_OC_PRELOAD_DISABLE
#endif


#include <gd32f1x0.h>

#include "drv_pwm.h"
#include "defines.h"
#include "config.h"
#include "hardware.h"
#include "drv_time.h"

#include <math.h>
#include "util.h"

#ifdef USE_ESC_DRIVER

extern int failsafe;
extern int onground;

int pwmdir = 0;
float debugpwm[4];
unsigned long pwm_failsafe_time = 1;


void pwm_set_forward(uint8_t number, float pwm);
void pwm_set_reverse(uint8_t number, float pwm);
void pwm_set_rev_pwmtop(uint8_t number , int pwmtop )	;
void pwm_set_reverse_3d( uint8_t number , float pwm);


TIMER_OCInitPara TIM_OCInitStructure;




void pwm_init(void)
{
	GPIO_InitPara GPIO_InitStructure;


//pin A0 TM2_ch1
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_2MHZ;
	GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLDOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE1, GPIO_AF_2);
// A0
// Timer2 ch1

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE0, GPIO_AF_2);


// PA2 tm2 ch3

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE2, GPIO_AF_2);
	
		// pin A3

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE3, GPIO_AF_2);

// timer 1 ch3 , ch1 
// pins A10 , A8         
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_10 | GPIO_PIN_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE10, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE8, GPIO_AF_2);


	//pin A9 TM1_ch2
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_9;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PINSOURCE9, GPIO_AF_2);


// timer 3 PB1 af1 ch4
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PINSOURCE1, GPIO_AF_1);
	
	
	TIMER_BaseInitPara TIM_TimeBaseStructure;

	RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2|RCC_APB1PERIPH_TIMER3 , ENABLE);

	RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_TIMER1, ENABLE);
	
	
	
// timer 2
	TIM_TimeBaseStructure.TIMER_Prescaler = TIMER_PRESCALER - 1;	//
	TIM_TimeBaseStructure.TIMER_CounterMode = TIMER_COUNTER_UP;
	TIM_TimeBaseStructure.TIMER_Period = PWMTOP;
	TIM_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
// init timer 2
	TIMER_BaseInit(TIMER2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIMER_OCMode = TIMER_OC_MODE_PWM1;
	TIM_OCInitStructure.TIMER_OCPolarity = OUT_POLARITY;
	TIM_OCInitStructure.TIMER_OutputState = TIMER_OUTPUT_STATE_ENABLE;
	TIM_OCInitStructure.TIMER_OCIdleState = TIMER_OC_IDLE_STATE_RESET;

	TIM_OCInitStructure.TIMER_Pulse = 0;



	
	
	TIMER_OC1_Init(TIMER2, &TIM_OCInitStructure);

	TIMER_OC1_Preload(TIMER2, ESC_PRELOAD);


	TIMER_OC2_Init(TIMER2, &TIM_OCInitStructure);

	TIMER_OC2_Preload(TIMER2, ESC_PRELOAD);
	
	
	TIMER_OC3_Init(TIMER2, &TIM_OCInitStructure);

	TIMER_OC3_Preload(TIMER2, ESC_PRELOAD);


	TIMER_OC4_Init(TIMER2, &TIM_OCInitStructure);

	TIMER_OC4_Preload(TIMER2, ESC_PRELOAD);
	


	TIMER_CARLPreloadConfig(TIMER2, ENABLE);
	



// timer 1

// init timer 1
	TIMER_BaseInit(TIMER1, &TIM_TimeBaseStructure);


	TIMER_OC3_Init(TIMER1, &TIM_OCInitStructure);
	
	TIMER_OC3_Preload(TIMER1, ESC_PRELOAD);


	TIMER_OC1_Init(TIMER1, &TIM_OCInitStructure);
	
	TIMER_OC1_Preload(TIMER1, ESC_PRELOAD);
	
	
	TIMER_OC2_Init(TIMER1, &TIM_OCInitStructure);

	TIMER_OC2_Preload(TIMER1, ESC_PRELOAD);
	


	TIMER_CARLPreloadConfig(TIMER1, ENABLE);




// timer 3

// init timer 3
	TIMER_BaseInit(TIMER3, &TIM_TimeBaseStructure);

	TIMER_OC4_Init(TIMER3, &TIM_OCInitStructure);
	
	TIMER_OC4_Preload(TIMER3, ESC_PRELOAD);



	TIMER_CARLPreloadConfig(TIMER3, ENABLE);




	TIMER_Enable(TIMER1, ENABLE);

	TIMER_Enable(TIMER2, ENABLE);
	
	TIMER_Enable(TIMER3, ENABLE);
	
	
	TIMER_CtrlPWMOutputs(TIMER1, ENABLE);
	
	TIMER_CtrlPWMOutputs(TIMER3, ENABLE);
				
	TIMER_CtrlPWMOutputs(TIMER2, ENABLE);



// set failsafetime so signal is off at start
pwm_failsafe_time = gettime() - 100000;

// top fets off
	GPIO_WriteBit(GPIOF, GPIO_PIN_1, Bit_RESET);	// bridge dir 1    
	GPIO_WriteBit(GPIOA, GPIO_PIN_4, Bit_RESET);	// bridge dir 2

#ifdef INVERTED_PWM
	for (int i = 0; i <= 3; i++)
	{
	// because the signal is inverted we set to max to turn the fets off
	 pwm_set_forward(i, 1.0f);
	}

delay(3000 + PWMTOP_US*2);

// set to positive for pull-up - resistor soldering
		  GPIO_WriteBit(GPIOF, GPIO_PIN_1, Bit_SET);	// bridge dir 1      
		  GPIO_WriteBit(GPIOA, GPIO_PIN_4, Bit_RESET);	// bridge dir 2
#endif
}



void motorbeep( void)
{

}


void pwm_set(uint8_t number, float pwm)
{

#ifdef ENABLE_REVERSE
	// reverse mode
	pwm_set_reverse_3d(number, pwm);
#else 	
	// forward only mode
	if (pwmdir == DIR2)
	  {
		  pwm_set_reverse(number, pwm);
	  }
else
	{
		 pwm_set_reverse(number, 0.0f);
	}
#endif
}



void pwm_set_rev_pwmtop(uint8_t number , int pwmtop )	
{	
	
int pwm = pwmtop;
	
	if ( pwm < 0 ) pwm = 0;
  if ( pwm > PWMTOP ) pwm = PWMTOP;
	

	TIM_OCInitStructure.TIMER_Pulse = (uint32_t) pwm;
	switch (number)
	  {
	  case 0:		// fl
		  TIMER_OC2_Init(TIMER2, &TIM_OCInitStructure);
		  break;

	  case 1:		// bl
		  TIMER_OC4_Init(TIMER2, &TIM_OCInitStructure);
		  break;

	  case 2:		// fr
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


void pwm_set_reverse( uint8_t number , float pwm)
{

	if ( pwm < 0 ) pwm = 0;
	
	debugpwm[number] = mapf ( pwm , 0 , 1 ,  (float) ESC_MIN ,  (float) ESC_MAX ) ;
	
	pwm = mapf ( pwm , 0 , 1 , ( (float) PWMTOP/PWMTOP_US)*ESC_MIN , ( (float) PWMTOP/PWMTOP_US)*ESC_MAX ); 

if ( onground ) pwm = ((float)PWMTOP/PWMTOP_US) * ESC_THROTTLEOFF;
	
	if ( failsafe ) 
	{
		if ( !pwm_failsafe_time )
		{
			pwm_failsafe_time = gettime();
		}
		else
		{
			// 100mS after failsafe we turn off the signal (for safety while flashing)
			if ( gettime() - pwm_failsafe_time > 100000 )
			{
				pwm = ((float)PWMTOP/PWMTOP_US) * ESC_FAILSAFE;
			}
		}
		
	}
	else
	{
		pwm_failsafe_time = 0;
	}

	if ( pwm > ((float)PWMTOP/PWMTOP_US)*ESC_MAX ) pwm = ((float)PWMTOP/PWMTOP_US)*ESC_MAX ;

#ifdef ONESHOT_125_ENABLE
	pwm = pwm/8;
#endif
	
	pwm = lroundf(pwm);
	
	if ( pwm < 0 ) pwm = 0;
  if ( pwm > PWMTOP ) pwm = PWMTOP;
	
 pwm_set_rev_pwmtop( number ,  pwm );	

}

/////////////////////
// 3D mode driver

void pwm_set_reverse_3d( uint8_t number , float pwm)
{

	if ( pwm < 0 ) pwm = 0;

	
if ( pwmdir == DIR2 )
{
	
	// forward mode
	
	pwm = mapf ( pwm , 0 , 1 , ( (float) PWMTOP/PWMTOP_US)*ESC_FORWARD_MIN , ( (float) PWMTOP/PWMTOP_US)*ESC_FORWARD_MAX ); 

}
else
{
	// reverse mode
	
	pwm = mapf ( pwm , 0 , 1 , ( (float) PWMTOP/PWMTOP_US)*ESC_REVERSE_MIN , ( (float) PWMTOP/PWMTOP_US)*ESC_REVERSE_MAX ); 

}


	if ( onground ) pwm = ((float)PWMTOP/PWMTOP_US) * ESC_THROTTLEOFF_3D ;
	
	if ( failsafe ) 
	{
		if ( !pwm_failsafe_time )
		{
			pwm_failsafe_time = gettime();
		}
		else
		{
			// 100mS after failsafe we turn off the signal (for safety while flashing)
			if ( gettime() - pwm_failsafe_time > 100000 )
			{
				pwm = ((float)PWMTOP/PWMTOP_US) * ESC_FAILSAFE_3D;
			}
		}
		
	}
	else
	{
		pwm_failsafe_time = 0;
	}

	
	if ( pwm > ((float)PWMTOP/PWMTOP_US)*ESC_FORWARD_MAX ) pwm = ((float)PWMTOP/PWMTOP_US)*ESC_FORWARD_MAX ;

	
#ifdef ONESHOT_125_ENABLE
	pwm = pwm/8;
#endif
	
	pwm = lroundf(pwm);
	
	if ( pwm < 0 ) pwm = 0;
  if ( pwm > PWMTOP ) pwm = PWMTOP;
	
 pwm_set_rev_pwmtop( number ,  pwm );	

}



// may be reverse
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
	  case 0:		// FR ok
		  TIMER_OC2_Init(TIMER1, &TIM_OCInitStructure);
		  break;

	  case 1:		// BR ok
		  TIMER_OC4_Init(TIMER3, &TIM_OCInitStructure);
		  break;

	  case 2:		// FL ok
		  TIMER_OC1_Init(TIMER2, &TIM_OCInitStructure);
		  break;

	  case 3:		// BL ok        
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
	pwmdir = dir;


}

#endif
