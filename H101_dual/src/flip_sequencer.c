#include "drv_time.h"
#include "config.h"

// throttle values in various stages
// up = first stage
// hover = during flip
// exit = at end of flip
#define THROTTLE_UP 0.9
#define THROTTLE_HOVER 0.5
#define THROTTLE_EXIT 0.8

#define THROTTLE_HALFHOVER 0.5
#define THROTTLE_HALFEXIT 0.8
#define THROTTLE_UP_ADD 0.3f
#define THROTTLE_HALFEXIT_ADD 0.3f


#define FLIP_RATE 1500
//time to up throttle in beginning stage
// 200ms
#define THROTTLE_UP_TIME 200e3
// end time in level mode
#define LEVEL_MODE_TIME 100e3
#define LEVEL_MODE_ANGLE -0

#define FLIP_TIMEOUT_TOTAL 1500e3
#define FLIP_TIMEOUT_STAGE1 500e3

#define HALFFLIP_RATE 600
#define LEVEL_MODE_HALFANGLE -0

#define HALFFLIP_ALLOW_CONTROL



#define STAGE_FLIP_NONE 0
#define STAGE_FLIP_START 1
#define STAGE_FLIP_THROTTLEUP 2
#define STAGE_FLIP_ROTATING 3
#define STAGE_FLIP_ROTATING_INVERTED 4
#define STAGE_FLIP_LEVELMODE 5
#define STAGE_FLIP_EXIT 6
#define STAGE_HALFFLIP_REVERSEMOTORS 7
#define STAGE_HALFFLIP_LEVELMODE 8


#include <math.h>


int acro_override = 0;
int controls_override = 0;
unsigned long fliptime = 0;
int isflipping = 0;
int flipstage = STAGE_FLIP_NONE;
unsigned int levelmodetime;
int flipindex = 0;
int flipdir = 0;
int halfflip = 0;
int level_override = 0;
int startdir = 1;
float rx_override[4];

extern int onground;
extern float GEstG[3];
extern float rx[];
extern int pwmdir;
extern int motor_dir;




void start_flip()
{

	if ( isflipping == 0 || !onground)
	{
		isflipping = 1;
		fliptime = gettime();
		flipstage = STAGE_FLIP_START;
		
	}
	
flipindex = 0;
flipdir = 0;
halfflip = 0;	
startdir = (pwmdir==FORWARD);
	
	if ( fabsf(rx[0]) < fabsf(rx[1]) ) 
	{
		flipindex = 1;
		if ( rx[1] > 0 ) flipdir = 1; 
	}
	else if ( rx[0] > 0 ) flipdir = 1; 
}


void start_invert()
{

	if ( isflipping == 0 || !onground)
	{
		isflipping = 1;
		fliptime = gettime();
		flipstage = STAGE_FLIP_START;
		
	}
	
flipindex = 0;
flipdir = 0;
halfflip = 1;	
	
startdir = (pwmdir==FORWARD);
	
if ( rx[0] > 0 ) flipdir = 1; 
	
}


void flip_sequencer()
{
	if ( !isflipping) return;
	
	if (onground) flipstage = STAGE_FLIP_EXIT;
	
	if ( isflipping && gettime() - fliptime > FLIP_TIMEOUT_TOTAL )
			{
				// abort after 1 second
				flipstage = STAGE_FLIP_EXIT;
			}
if ( !halfflip)
{	
//////////////////////////////////////
// 360 Eversion !?
//////////////////////////////////////
	switch (flipstage )
	{
		case STAGE_FLIP_NONE:
			
		break;
		
		case STAGE_FLIP_START:		
			acro_override = 1;
			controls_override = 1;
			level_override = 0;
			rx_override[0] = 0;
			rx_override[1] = 0;
			rx_override[2] = 0;
		  rx_override[3] = THROTTLE_UP;
		
			flipstage = STAGE_FLIP_THROTTLEUP;	
		break;

		
		case STAGE_FLIP_THROTTLEUP:		 
			if ( gettime() - fliptime > THROTTLE_UP_TIME )
			{
				if ( flipdir) 
						rx_override[flipindex] = (float) FLIP_RATE / (float) MAX_RATE;
				else 
					  rx_override[flipindex] = (float)- FLIP_RATE / (float) MAX_RATE;
				rx_override[3] = THROTTLE_UP;
				flipstage = STAGE_FLIP_ROTATING;
			}			
		break;
	
			
		case STAGE_FLIP_ROTATING:
			if ( gettime() - fliptime > FLIP_TIMEOUT_STAGE1 + THROTTLE_UP_TIME )
			{
				// abort
				flipstage = STAGE_FLIP_EXIT;
			}
			
			if ( (startdir && GEstG[2] < 0) || (!startdir && GEstG[2] > 0 ) ) 
			{
				//we are inverted
				rx_override[3] = THROTTLE_HOVER;
				flipstage = STAGE_FLIP_ROTATING_INVERTED;			
			}
			
			
		break;
		
			
		case STAGE_FLIP_ROTATING_INVERTED:
			
			if ( (startdir&&GEstG[2] > 200) || ((!startdir )&&GEstG[2] < -200) ) 
			{
				//we are no longer inverted
				levelmodetime = gettime();
				
				rx_override[3] = THROTTLE_EXIT;				
				acro_override = 0;				
				flipstage = STAGE_FLIP_LEVELMODE;
				level_override = 1;
			}
		break;

		
		case STAGE_FLIP_LEVELMODE:		
			// allow control in other axis at this point
		  rx_override[0] = rx[0];
		  rx_override[1] = rx[1];
		  rx_override[2] = rx[2];
			if ( flipdir )		
				rx_override[flipindex] = (float) LEVEL_MODE_ANGLE / (float) MAX_ANGLE_HI;
		  else
				rx_override[flipindex] = (float) - LEVEL_MODE_ANGLE / (float) MAX_ANGLE_HI;
		  //aierror[flipindex] = 0.0f; // reset integral term
			if( gettime() - levelmodetime > LEVEL_MODE_TIME )
					flipstage = STAGE_FLIP_EXIT;
		break;
			
			
		case STAGE_FLIP_EXIT:
			isflipping = 0;
			flipstage = STAGE_FLIP_NONE;
			acro_override = 0;
			controls_override = 0;
			level_override = 0;
		break;
	
	
		default:
		  flipstage = STAGE_FLIP_EXIT;	
		break;
	
	}
}else
{
	////////////////////////////////////////////////
	// half flip
	////////////////////////////////////////////////
	switch (flipstage )
	{
		case STAGE_FLIP_NONE:
			
		break;
		
		case STAGE_FLIP_START:
		// 		
			acro_override = 1;
			controls_override = 1;
			level_override = 0;
		#ifndef HALFFLIP_ALLOW_CONTROL
			rx_override[0] = 0;
			rx_override[1] = 0;
			rx_override[2] = 0;
		#endif
		  rx_override[3] = THROTTLE_UP;
		
			flipstage = STAGE_FLIP_THROTTLEUP;
			flipindex = 0;
		// if already inverted we skip a few steps
			if ( (startdir && GEstG[2] < 0) || (!startdir && GEstG[2] > 0 ) ) 
			{
				rx_override[3] = THROTTLE_HALFHOVER;		
				flipstage = STAGE_HALFFLIP_REVERSEMOTORS;	
			}	
		break;
		
		case STAGE_FLIP_THROTTLEUP:
			#ifdef HALFFLIP_ALLOW_CONTROL
		  rx_override[0] = rx[0];
		  rx_override[1] = rx[1];
		  rx_override[2] = rx[2]; 
		  #endif
			rx_override[3] = rx[3] + THROTTLE_UP_ADD;	
	
			if ( gettime() - fliptime > THROTTLE_UP_TIME )
			{
			#ifndef HALFFLIP_ALLOW_CONTROL
			rx_override[0] = 0;
			rx_override[1] = 0;
			rx_override[2] = 0;
			#endif
				if ( flipdir) 
						rx_override[flipindex] = (float) HALFFLIP_RATE / (float) MAX_RATE;
				else 
					  rx_override[flipindex] = (float)- HALFFLIP_RATE / (float) MAX_RATE;
				rx_override[3] = THROTTLE_HALFHOVER;
				flipstage = STAGE_FLIP_ROTATING;
			}
			
		break;
		
		case STAGE_FLIP_ROTATING:
			if ( gettime() - fliptime > FLIP_TIMEOUT_STAGE1 + THROTTLE_UP_TIME )
			{
				// abort
				flipstage = STAGE_FLIP_EXIT;
			}
			if ( (startdir && GEstG[2] < 0) || (!startdir && GEstG[2] > 0 ) ) 
			{
				//
				rx_override[3] = THROTTLE_HALFHOVER;
				
				flipstage = STAGE_HALFFLIP_REVERSEMOTORS;	
			
			}
		break;
			

		case STAGE_HALFFLIP_REVERSEMOTORS:									
		  motor_dir = !motor_dir;				
		  acro_override = 0;
			level_override = 1;
			flipstage = STAGE_HALFFLIP_LEVELMODE;	
		break;
			
			
		case STAGE_HALFFLIP_LEVELMODE:	
			// allow control in other axis at this point
		  rx_override[0] = rx[0];
		  rx_override[1] = rx[1];
		  rx_override[2] = rx[2];
			
	//		rx_override[flipindex] = apid(flipindex) ;
			
			rx_override[3] = THROTTLE_HALFEXIT_ADD + rx[3];  
		
			if ( flipdir )		
			rx_override[flipindex] += (float) LEVEL_MODE_HALFANGLE / (float) MAX_ANGLE_HI;
		  else
			rx_override[flipindex] += (float) - LEVEL_MODE_HALFANGLE / (float) MAX_ANGLE_HI;

			if( gettime() - levelmodetime > LEVEL_MODE_TIME )
					flipstage = STAGE_FLIP_EXIT;
		break;
			
			
		case STAGE_FLIP_EXIT:
			isflipping = 0;
			flipstage = STAGE_FLIP_NONE;
			acro_override = 0;
			controls_override = 0;
			level_override = 0;
		break;
	
	
		default:
		  flipstage = STAGE_FLIP_EXIT;	
		break;
	
	}
	
}

	
}


