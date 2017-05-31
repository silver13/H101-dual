
#include "defines.h"

/*

 .----------------.  .----------------.  .----------------.  .----------------.
| .--------------. || .--------------. || .--------------. || .--------------. |
| |  ____  ____  | || |     __       | || |     ____     | || |     __       | |
| | |_   ||   _| | || |    /  |      | || |   .'    '.   | || |    /  |      | |
| |   | |__| |   | || |    `| |      | || |  |  .--.  |  | || |    `| |      | |
| |   |  __  |   | || |     | |      | || |  | |    | |  | || |     | |      | |
| |  _| |  | |_  | || |    _| |_     | || |  |  `--'  |  | || |    _| |_     | |
| | |____||____| | || |   |_____|    | || |   '.____.'   | || |   |_____|    | |
| |              | || |              | || |              | || |              | |
| '--------------' || '--------------' || '--------------' || '--------------' |
 '----------------'  '----------------'  '----------------'  '----------------'

***DO NOT FLASH H8 FIRMWARE TO THE H101 BY ACCIDENT***

*/

//  ACRO ONLY //  ACRO ONLY //  ACRO ONLY
//  ACRO ONLY //  ACRO ONLY //  ACRO ONLY
//  ACRO ONLY //  ACRO ONLY //  ACRO ONLY

// rate pids in pid.c ( for acro mode)


// rate in deg/sec
// for low rates ( acro mode)
#define MAX_RATE 180.0f
#define MAX_RATEYAW 180.0f

// multiplier for high rates
// devo/module uses high rates only
#define HIRATEMULTI 2.0f
#define HIRATEMULTIYAW 2.0f



// this make the controls mirrored in inverted mode
//#define NATIVE_INVERTED_MODE

// 3d throttle - center off
//#define THREE_D_THROTTLE

// deadzone in center of 3d throttle 0.0 - 1.0
#define THREE_D_THROTTLE_DEADZONE 0.2


// disable inbuilt expo functions
#define DISABLE_EXPO

// use if your tx has no expo function
// also comment out DISABLE_EXPO to use
// -1 to 1 , 0 = no exp
// positive = less sensitive near center
#define EXPO_XY 0.3f
#define EXPO_YAW 0.0f




// Hardware gyro LPF filter frequency
// gyro filter 0 = 250hz delay 0.97mS
// gyro filter 1 = 184hz delay 2.9mS
// gyro filter 2 = 92hz delay 3.9mS
// gyro filter 3 = 41hz delay 5.9mS (Default)
// gyro filter 4 = 20hz
#define GYRO_LOW_PASS_FILTER 3

// software gyro lpf ( iir )
// set only one below
//#define SOFT_LPF_1ST_023HZ
//#define SOFT_LPF_1ST_043HZ
//#define SOFT_LPF_1ST_100HZ
//#define SOFT_LPF_2ND_043HZ
#define SOFT_LPF_2ND_088HZ
//#define SOFT_LPF_4TH_088HZ
//#define SOFT_LPF_4TH_160HZ
//#define SOFT_LPF_4TH_250HZ
//#define SOFT_LPF_NONE

// this works only on newer boards (non mpu-6050)
// on older boards the hw gyro setting controls the acc as well
#define ACC_LOW_PASS_FILTER 5



// Channel assignments
//


// CH_FLIP - 0 - flip
// CH_EXPERT - 1 - expert
// CH_HEADFREE - 2 - headfree
// CH_RTH - 3 - headingreturn
// CH_AUX1 - 4 - AUX1 ( gestures <<v and >>v)
// CH_AUX2 - 5 - AUX2+ (  up - up - up    )
// CH_AUX3 - gravity channel ( on if inverted , updated at zero throttle)
// CH_PIT_TRIM - 6 - Pitch trims
// CH_RLL_TRIM - 7 - Roll trims
// CH_THR_TRIM - 8 - Throttle trims
// CH_YAW_TRIM - 9 - Yaw trims
// CH_INV 10 - Inverted mode
// CH_VID 7 -
// CH_PIC 8 -
// CH_ON - 10 - on always
// CH_OFF - 11 - off always
//
// devo can use DEVO_CHAN_5 - DEVO_CHAN_10
// Multiprotocol can use MULTI_CHAN_5 - MULTI_CHAN_10


// rates / expert mode
#define RATES CH_EXPERT


// channel for inverted mode ( default - CH_AUX3 - gravity based)
#define INVERTEDMODE CH_AUX3

// leds on / off channel
#define LEDS_ON CH_ON


// toggle is a block with an input and an output
// uncomment input to enable ( aux 2 is gesture up - up - up )
//#define TOGGLE_IN CH_AUX2
#define TOGGLE_OUT CH_AUX4

// Channel to turn a GPIO pin on/off. Can be used to switch
// a FPV camera on/off . Select the FPV_PIN in hardware.h
//#define FPV_ON CH_VID // DEVO_CHAN_8

// Airmode keeps the PID loop stabilizing the quads orientation even at zero throttle.
// To stop the motors on ground a switch on the remote control is necessary.
//#define AIRMODE_HOLD_SWITCH CH_INV // DEVO_CHAN_5


// aux1 channel starts on if this is defined, otherwise off.
#define AUX1_START_ON
//#define AUX4_START_ON

// use yaw/pitch instead of roll/pitch for gestures
//#define GESTURES_USE_YAW

// comment out if not using ( disables trim as channels, will still work with stock tx except that feature )
// devo/tx module incompatible
//#define USE_STOCK_TX

// automatically remove center bias ( needs throttle off for 1 second )
//#define STOCK_TX_AUTOCENTER


// throttle angle compensation in level mode
//#define AUTO_THROTTLE


// enable auto lower throttle near max throttle to keep control
// comment out to disable 
//#define MIX_LOWER_THROTTLE
#define MIX_INCREASE_THROTTLE

// brushless lower throttle type 3 - use only 1 lower throttle method
// fast acting
//#define MIX_LOWER_THROTTLE_3

// lowers the rates when motor limits exceeded
//#define RATELIMITER_ENABLE

// options for mix throttle lowering if enabled
// 0 - 100 range ( 100 = full reduction / 0 = no reduction )
#define MIX_THROTTLE_REDUCTION_PERCENT 100


// battery saver ( only at powerup )
// does not start software if battery is too low
// flashes 2 times repeatedly at startup
#define STOP_LOWBATTERY

// voltage too start warning
// volts
#define VBATTLOW 3.5f

// compensation for battery voltage vs throttle drop
// increase if battery low comes on at max throttle
// decrease if battery low warning goes away at high throttle
// in volts
#define VDROP_FACTOR 0.70f

// determine VDROP_FACTOR automatically in-flight, set factor ignored
#define AUTO_VDROP_FACTOR

// voltage hysteresys
// in volts
#define HYST 0.10f

// lower throttle to keep voltage above set treshold
//#define LVC_PREVENT_RESET
#define LVC_PREVENT_RESET_VOLTAGE 2.85

// lower throttle when battery below treshold
//#define LVC_LOWER_THROTTLE
#define LVC_LOWER_THROTTLE_VOLTAGE 3.30
#define LVC_LOWER_THROTTLE_VOLTAGE_RAW 2.70
#define LVC_LOWER_THROTTLE_KP 3.0

//#define PID_VOLTAGE_COMPENSATION

// enable motor filter
// hanning 3 sample fir filter
#define MOTOR_FILTER

// lost quad beeps using motors
//#define MOTOR_BEEPS

// clip feedforward attempts to resolve issues that occur near full throttle
//#define CLIP_FF

// motor transient correction applied	to throttle stick
//#define THROTTLE_TRANSIENT_COMPENSATION



// motor curve to use
// the pwm frequency has to be set independently
// 720 motors use curve none and pwm 8K
#define MOTOR_CURVE_NONE
//#define MOTOR_CURVE_6MM_H101_490HZ
//#define MOTOR_CURVE_6MM_490HZ
//#define MOTOR_CURVE_85MM_8KHZ
//#define MOTOR_CURVE_85MM_8KHZ_OLD
//#define MOTOR_CURVE_85MM_32KHZ
//#define CUSTOM_MOTOR_CURVE 0.3

// pwm frequency for motor control
// a higher frequency makes the motors more linear
//#define PWM_490HZ
//#define PWM_8KHZ
//#define PWM_16KHZ
//#define PWM_24KHZ
#define PWM_32KHZ


// failsafe time in uS
#define FAILSAFETIME 1000000  // one second

// uncomment to enable buzzer. Select the BUZZER_PIN in hardware.h
//#define BUZZER_ENABLE




// 0 - 3 transmit power
#define TX_POWER_TELEMETRY 1

// rx protocol selection
#define RX_BAYANG_TELEMETRY
//#define RX_BAYANG_BLE
//#define RX_BAYANG_BLE_APP


// Comment out to disable pid tuning gestures
#define PID_GESTURE_TUNING
#define COMBINE_PITCH_ROLL_PID_TUNING






// ########################################
// things that are experimental / old / etc
// do not change things below

// serial prints with info
//#define SERIAL
// serial driver on SWCLK - 57600 default
//#define SERIAL_DRV

// invert yaw pid
//#define INVERT_YAW_PID

//some debug stuff
//#define DEBUG

// disable motors for testing
//#define NOMOTORS

// throttle direct to motors for thrust measure/ esc testing
//#define MOTORS_TO_THROTTLE

// time to change motor direction (uS)
#ifdef THREE_D_THROTTLE
// with 3d throttle a short timeout as it takes time to move the stick
#define BRIDGE_TIMEOUT 10000
#else
//otherwise a 0.05s pause
#define BRIDGE_TIMEOUT 50000
#endif

// level mode "manual" trims ( in degrees)
// pitch positive forward trim
// roll positive right trim
#define TRIM_PITCH 0.0
#define TRIM_ROLL 0.0

// inverted trims ( disabled)
//#define TRIM_PITCH_INV 0.0
//#define TRIM_ROLL_INV 0.0


// enable motors if pitch / roll controls off center (at zero throttle)
// possible values: 0 / 1
#define ENABLESTIX 0
#define ENABLESTIX_TRESHOLD 0.3
#define ENABLESTIX_TIMEOUT 1e6

// A deadband can be used to eliminate stick center jitter and non-returning to exactly 0.
//#define STICKS_DEADBAND 0.02f

// old calibration flash
//#define OLD_LED_FLASH


// limit minimum motor output to a value (0.0 - 1.0)
//#define MOTOR_MIN_ENABLE
#define MOTOR_MIN_VALUE 0.05

// limit max motor output to a value (0.0 - 1.0)
//#define MOTOR_MAX_ENABLE
#define MOTOR_MAX_VALUE 1.00

// under this voltage the software will not start
// if STOP_LOWBATTERY is defined
#define STOP_LOWBATTERY_TRESH 3.3f



// define logic

// don't stop software on low battery so buzzer will still sound
#ifdef BUZZER_ENABLE
#undef STOP_LOWBATTERY
#endif

// disable startup battery check so beacon can work after a reset
#ifdef RX_BAYANG_BLE
#undef STOP_LOWBATTERY
#endif

#ifdef RX_BAYANG_BLE_APP
#undef STOP_LOWBATTERY
#endif

// do not change
// only for compilers other than gcc
// some warnings, mainly double to float conversion
#ifndef __GNUC__

#pragma diag_warning 1035 , 177 , 4017
#pragma diag_error 260

#endif
// --fpmode=fast ON















