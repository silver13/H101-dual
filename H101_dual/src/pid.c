/*
The MIT License (MIT)

Copyright (c) 2015 silverx

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


//#define RECTANGULAR_RULE_INTEGRAL
#define MIDPOINT_RULE_INTEGRAL
//#define SIMPSON_RULE_INTEGRAL


//#define NORMAL_DTERM
#define NEW_DTERM
//#define MAX_FLAT_LPF_DIFF_DTERM
//#define DTERM_LPF_1ST_HZ 100
//#define  DTERM_LPF_2ND_HZ 120


#include "pid.h"
#include "util.h"
#include "config.h"
#include <stdlib.h>
#include <math.h>
#include "defines.h"
#include <stdbool.h>


// Kp                            ROLL, PITCH, YAW
float pidkp_flash[PIDNUMBER] = { 0.12, 0.12, 0.4 };

// Ki                            ROLL, PITCH, YAW
float pidki_flash[PIDNUMBER] = { 0.65, 0.65, 5.0 };

// Kd                            ROLL, PITCH, YAW
float pidkd_flash[PIDNUMBER] = { 0.61, 0.61, 0.0 };


// output limit
const float outlimit[PIDNUMBER] = { 0.8, 0.8, 0.4 };

// limit of integral term (abs)
const float integrallimit[PIDNUMBER] = { 0.8, 0.8, 0.4 };



// multiplier for pids at 3V - for PID_VOLTAGE_COMPENSATION - default 1.33f H101
#define PID_VC_FACTOR 1.33f


// PID rates can be switched to the second set found below by using the following channel:
// #define DUAL_PID_RATES CH_VID

// Tuning PIDs with gestures only changes the first set though.

// working pids loaded from flash / above automatically
float pidkp[6] = { 0, 0, 0,  0.12, 0.12, 0.4 };
float pidki[6] = { 0, 0, 0,  0.65, 0.65, 5.0 };
float pidkd[6] = { 0, 0, 0,  0.61, 0.61, 0.0 };
//                           ^-- second PID set starts here


#ifdef DUAL_PID_RATES
extern char aux[AUXNUMBER];
#endif

int number_of_increments[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
int current_pid_axis = 0;
int current_pid_term = 0;
float * current_pid_term_pointer = pidkp;


#ifdef NORMAL_DTERM
static float lastrate[PIDNUMBER];
#endif
float pidoutput[PIDNUMBER];

float error[PIDNUMBER];
float setpoint[PIDNUMBER];
extern float looptime;
extern float gyro[3];
extern int onground;
extern float looptime;
extern float vbattfilt;

static float lasterror[PIDNUMBER];
float ierror[PIDNUMBER] = { 0, 0, 0 };
float timefactor;
float v_compensation = 1.00;

#ifdef NORMAL_DTERM
static float lastrate[PIDNUMBER];
#endif

#ifdef NEW_DTERM
static float lastratexx[PIDNUMBER][2];
#endif


#ifdef MAX_FLAT_LPF_DIFF_DTERM
static float lastratexx[PIDNUMBER][4];
#endif

#ifdef SIMPSON_RULE_INTEGRAL
static float lasterror2[PIDNUMBER];
#endif


// calculate change from ideal loop time
// 0.0032f is there for legacy purposes, should be 0.001f = looptime
// this is called in advance as an optimization because it has division
void pid_precalc()
{
	timefactor = 0.0032f / looptime;
#ifdef PID_VOLTAGE_COMPENSATION
	v_compensation = mapf ( vbattfilt , 3.00 , 4.00 , PID_VC_FACTOR , 1.00);
	if( v_compensation > PID_VC_FACTOR) v_compensation = PID_VC_FACTOR;
	if( v_compensation < 1.00f) v_compensation = 1.00;
#endif
}

#ifdef DTERM_LPF_2ND_HZ
//the compiler calculates these
static float two_one_minus_alpha = 2*FILTERCALC_NEW( 0.001 , (1.0f/DTERM_LPF_2ND_HZ) );
static float one_minus_alpha_sqr = (FILTERCALC_NEW( 0.001 , (1.0f/DTERM_LPF_2ND_HZ) ) )*(FILTERCALC_NEW( 0.001 , (1.0f/DTERM_LPF_2ND_HZ) ));
static float alpha_sqr = (1 - FILTERCALC_NEW( 0.001 , (1.0f/DTERM_LPF_2ND_HZ) ))*(1 - FILTERCALC_NEW( 0.001 , (1.0f/DTERM_LPF_2ND_HZ) ));

static float last_out[3], last_out2[3];

float lpf2( float in, int num)
 {

  float ans = in * alpha_sqr + two_one_minus_alpha * last_out[num]
      - one_minus_alpha_sqr * last_out2[num];

  last_out2[num] = last_out[num];
  last_out[num] = ans;

  return ans;
 }
#endif

// Cycle through P / I / D - The initial value is P
// The return value is the currently selected TERM (after setting the next one)
// 1: P
// 2: I
// 3: D
// The return value is used to blink the leds in main.c
int next_pid_term()
{
	switch (current_pid_term)
	{
		case 0:
			current_pid_term_pointer = pidki;
			current_pid_term = 1;
			break;
		case 1:
			current_pid_term_pointer = pidkd;
			current_pid_term = 2;
			break;
		case 2:
			current_pid_term_pointer = pidkp;
			current_pid_term = 0;
			break;
	}

	return current_pid_term + 1;
}

// Cycle through the axis - Initial is Roll
// Return value is the selected axis, after setting the next one.
// 1: Roll
// 2: Pitch
// 3: Yaw
// The return value is used to blink the leds in main.c
int next_pid_axis()
{
	const int size = 3;
	if (current_pid_axis == size - 1) {
		current_pid_axis = 0;
	}
	else {
		#ifdef COMBINE_PITCH_ROLL_PID_TUNING
		if (current_pid_axis <2 ) {
			// Skip axis == 1 which is roll, and go directly to 2 (Yaw)
			current_pid_axis = 2;
		}
		#else
		current_pid_axis++;
		#endif
	}

	return current_pid_axis + 1;
}

#define PID_GESTURES_MULTI 1.1f

int change_pid_value(int increase)
{
	float multiplier = 1.0f/(float)PID_GESTURES_MULTI;
	if (increase) {
		multiplier = (float)PID_GESTURES_MULTI;
		number_of_increments[current_pid_term][current_pid_axis]++;
	}
	else {
		number_of_increments[current_pid_term][current_pid_axis]--;
	}

	current_pid_term_pointer[current_pid_axis] = current_pid_term_pointer[current_pid_axis] * multiplier;

    #ifdef COMBINE_PITCH_ROLL_PID_TUNING
	if (current_pid_axis == 0) {
		current_pid_term_pointer[current_pid_axis+1] = current_pid_term_pointer[current_pid_axis+1] * multiplier;
	}
	#endif

	return abs(number_of_increments[current_pid_term][current_pid_axis]);
}

// Increase currently selected term, for the currently selected axis, (by functions above) by 10%
// The return value, is absolute number of times the specific term/axis was increased or decreased.  For example, if P for Roll was increased by 10% twice,
// And then reduced by 10% 3 times, the return value would be 1  -  The user has to rememeber he has eventually reduced the by 10% and not increased by 10%
// I guess this can be improved by using the red leds for increments and blue leds for decrements or something, or just rely on SilverVISE
int increase_pid()
{
	return change_pid_value(1);
}

// Same as increase_pid but... you guessed it... decrease!
int decrease_pid()
{
	return change_pid_value(0);
}

// https://www.rcgroups.com/forums/showpost.php?p=39354943&postcount=13468
void rotateErrors()
{
	// rotation around x axis:
	float temp = gyro[0] * looptime;
	ierror[1] -= ierror[2] * temp;
	ierror[2] += ierror[1] * temp;

	// rotation around y axis:
	temp = gyro[1] * looptime;
	ierror[2] -= ierror[0] * temp;
	ierror[0] += ierror[2] * temp;

	// rotation around z axis:
	temp = gyro[2] * looptime;
	ierror[0] -= ierror[1] * temp;
	ierror[1] += ierror[0] * temp;
}

float pid(int x)
{
	int pidindex = x;
#ifdef DUAL_PID_RATES
	if ( aux[DUAL_PID_RATES]) {
		pidindex += 3;
	}
#endif

	if (onground)
	  {
		  ierror[x] *= 0.98f; // 50 ms time-constant
	  }
#ifdef TRANSIENT_WINDUP_PROTECTION
	static float avgSetpoint[3];
	static int count[3];
	extern float splpf( float in,int num );
	if ( x < 2 && (count[x]++ % 2) == 0 ) {
		avgSetpoint[x] = splpf( setpoint[x], x );
	}
#endif
	int iwindup = 0;
	if ((pidoutput[x] == outlimit[x]) && (error[x] > 0))
	  {
		  iwindup = 1;
	  }
	if ((pidoutput[x] == -outlimit[x]) && (error[x] < 0))
	  {
		  iwindup = 1;
	  }
#ifdef TRANSIENT_WINDUP_PROTECTION
	if ( x < 2 && fabsf( setpoint[x] - avgSetpoint[x] ) > 0.1f ) {
		iwindup = 1;
	}
#endif

	if (!iwindup)
	  {
#ifdef MIDPOINT_RULE_INTEGRAL
		  // trapezoidal rule instead of rectangular
		  ierror[x] = ierror[x] + (error[x] + lasterror[x]) * 0.5f * pidki[pidindex] * looptime;
		  lasterror[x] = error[x];
#endif

#ifdef RECTANGULAR_RULE_INTEGRAL
		  ierror[x] = ierror[x] + error[x] * pidki[pidindex] * looptime;
		  lasterror[x] = error[x];
#endif

#ifdef SIMPSON_RULE_INTEGRAL
		  // assuming similar time intervals
		  ierror[x] = ierror[x] + 0.166666f * (lasterror2[x] + 4 * lasterror[x] + error[x]) * pidki[pidindex] * looptime;
		  lasterror2[x] = lasterror[x];
		  lasterror[x] = error[x];
#endif

	  }

	limitf(&ierror[x], integrallimit[x]);

	// P term
	pidoutput[x] = error[x] * pidkp[pidindex];

// https://www.rcgroups.com/forums/showpost.php?p=39606684&postcount=13846
// https://www.rcgroups.com/forums/showpost.php?p=39667667&postcount=13956
#ifdef FEED_FORWARD_STRENGTH
	if ( x < 2 ) {
		static float lastSetpoint[2];
		static float bucket[2];
		static float buckettake[2];
		if ( setpoint[x] != lastSetpoint[x] ) {
			bucket[x] += setpoint[x] - lastSetpoint[x];
			buckettake[x] = bucket[x] * 0.1f; // Spread it evenly over 10 ms (two PACKET_PERIODs)
		}
		lastSetpoint[x] = setpoint[x];

		if ( fabsf( bucket[x] ) > 0.0f ) {
			float take = buckettake[x];
			if ( bucket[x] < 0.0f != take < 0.0f || fabsf( take ) > fabsf( bucket[x] ) ) {
				take = bucket[x];
			}
			bucket[x] -= take;

			float ff = take * timefactor * FEED_FORWARD_STRENGTH * pidkd[x];

			// 4 point moving average filter to smooth out the 5 ms steps:
			#define POT 2 // power of two
			static float ma_value = 0;
			static float ma_array[ 1 << POT ] = { 0 };
			static int ma_index = 0;
			ma_value -= ma_array[ ma_index ];
			ma_value += ff;
			ma_array[ ma_index ] = ff;
			++ma_index;
			if ( ma_index >= ( 1 << POT ) ) {
				ma_index = 0;
			}
			ff = ma_value / ( 1 << POT ); // dividing by a power of two is handled efficiently by the compiler (__ARM_scalbnf)

			if ( ff < 0.0f == pidoutput[x] < 0.0f ) {
				if ( fabsf( ff ) > fabsf( pidoutput[x] ) ) {
					pidoutput[x] = ff; // Take the larger of P or FF as long as P and FF have the same sign.
				}
			} else {
				pidoutput[x] += ff; // Always add FF if the signs are opposite.
			}
		}
	}
#endif

	// I term
	pidoutput[x] += ierror[x];

	// D term

    #ifdef NORMAL_DTERM
    pidoutput[x] = pidoutput[x] - (gyro[x] - lastrate[x]) * pidkd[pidindex] * timefactor;
    lastrate[x] = gyro[x];
    #endif

    #ifdef NEW_DTERM
    pidoutput[x] = pidoutput[x] - ((0.5f) * gyro[x] - (0.5f) * lastratexx[x][1]) * pidkd[pidindex] * timefactor;

    lastratexx[x][1] = lastratexx[x][0];
    lastratexx[x][0] = gyro[x];
    #endif

    #ifdef MAX_FLAT_LPF_DIFF_DTERM
    pidoutput[x] = pidoutput[x] - ( + 0.125f *gyro[x] + 0.250f * lastratexx[x][0]
                - 0.250f * lastratexx[x][2] - ( 0.125f) * lastratexx[x][3]) * pidkd[x] * timefactor 						;

    lastratexx[x][3] = lastratexx[x][2];
    lastratexx[x][2] = lastratexx[x][1];
    lastratexx[x][1] = lastratexx[x][0];
    lastratexx[x][0] = gyro[x];
    #endif


	#ifdef DTERM_LPF_1ST_HZ
	float dterm;
	static float lastrate[3];
	static float dlpf[3] = {0};

	dterm = - (gyro[x] - lastrate[x]) * pidkd[x] * timefactor;
	lastrate[x] = gyro[x];

	lpf( &dlpf[x], dterm, FILTERCALC_NEW( 0.001 , 1.0f/DTERM_LPF_1ST_HZ ) );

	pidoutput[x] += dlpf[x];
	#endif

	#ifdef DTERM_LPF_2ND_HZ
	float dterm;
	static float lastrate[3];
	float lpf2( float in, int num);
	if ( pidkd[x] > 0)
	{
	    dterm = - (gyro[x] - lastrate[x]) * pidkd[x] * timefactor;
	    lastrate[x] = gyro[x];
	    dterm = lpf2(  dterm, x );
	    pidoutput[x] += dterm;
	}
	#endif

#ifdef PID_VOLTAGE_COMPENSATION
	pidoutput[x] *= v_compensation;
#endif

	limitf(&pidoutput[x], outlimit[x]);

	return pidoutput[x];
}
