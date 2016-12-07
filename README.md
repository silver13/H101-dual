[![Build Status](https://travis-ci.org/silver13/H101-acro.png)](https://travis-ci.org/silver13/H101-acro)

# Floureon H101 and Eachine H8S acro firmware ( red board )

*now with dual mode as well*

__DO NOT ACCIDENTALLY FLASH THE H8MINI FIRMWARE TO THE H101__ (it may break it)


Select folder for the correct firmware:
* __H101_Acro__: Acro only, inverted flight possible (manual on a switch)
* __H101_dual__: either/both modes, inverted flight possible (manual on a switch)

Note for H101_dual code: By default motor direction is automatically set at takeoff / zero-throttle, based on orientation.


For programming the ground pad is marked (B-) on the pcb(near the edge) .It is also close to some other components. Note there is also a (B+) pad,do not connect this. 

It works with Devo, stock or H8 mini tx.

You could also use diy [nrf24_multipro](https://github.com/goebish/nrf24_multipro) module by Goebish or [DIY-Multiprotocol-TX-Module](https://github.com/pascallanger/DIY-Multiprotocol-TX-Module) which supports telemetry.

 * GigaDevice GD32F130G6 cortex-M3 32k
 * Invensense gyro + accelerometer
 * XN297 transceiver
 * 8 Nfets and 8 Pfets. The P fets are controlled together in 2 groups of 4.


Cpu datasheet: [Pdf](https://app.box.com/s/3zi661iffmit1rwda499wu8vycv03biv) Cpu Documentation: [Pdf](https://app.box.com/s/pehsanvluc40qu8k2036sbjk5ti08y2m)

The firmware needs Keil.GD32F1xx_DFP.1.1.0.pack which adds support for the cpu to Keil (5.15 used). Later Keil versions will ask for the pack to be installed if not present, and will download it automatically.


The firmware flash procedure is the same as the H8.

Firmware thread featuring flashing info : [rcgroups.com](http://www.rcgroups.com/forums/showthread.php?t=2512604)


###Changing settings

__config.h__ - all settings: rates, switches/buttons config, other options

__pid.c__ - pids ( tuned for standard H101)

__angle_pid.c__ (*dual mode only*) level mode pid, also uses acro pids in level mode, so it should fly ok in acro mode first.


After changing settings, remember to click compile first, not just upload.

#####Stock tx:
On the stock tx only the rate (expert) button works. Trims are not functional on the stock tx, but the buttons can be used for controlling functions such as acro / level switch.

#####Devo tx:
Channels work as intended except the rate/expert channel which is always on. Dynamic trims are not used, and trims should not be required.

Assign the extra channels to the desired functions in config.h. A setting such as "#define LEVELMODE DEVO_CHAN_9" could be used, for example.

#####Gyro calibration
Gyro calibration runs automatically after power up, and usually completes within 2-4 seconds. If for some reason the calibration fails to complete, such as if there is movement, it will eventually time out in 15 seconds.

During calibration the leds glow from low brightness to high brightness. If movement is detected the flashing stops. The flashing resumes when movement stops.

*The quad should be standing still during gyro calibration for best results*


#####Accelerometer calibration ***(level mode only)***
For accelerometer calibration move the pitch stick down 3 times within about 1- 2 seconds. Wait a couple of seconds after a failed attempt. Throttle has to be low, and roll centered. Flashing lights indicate the calibration process. This is saved so it has to be done only once.

Note, the acc calibration also saves gyro biases which are used in some cases. The flash pattern is similar to the gyro calibration pattern.

*Calibration has to be done on a horizontal surface*

#####Led error codes
In some cases the leds are used to indicate error conditions, and as such they flash a number of times, then a brake occurs, then the pattern repeats. In all such cases the quadcopter will not respond to commands, a power cycle will be required.

The most common of this is 2 flashes = low battery, usually caused by an in-flight reset due to low battery. All other flashes are non user serviceable. The description is in main.c.

#####Led flash patterns
At startup the leds should flash a gyro calibration pattern for 2 - 15 seconds, with a glow like pattern. Movement stops the flashing while it occurs.

Following should be a fast (20 times/sec) flash indicating that the quad is waiting for bind. 

If binding is completed the leds should light up continuously, while if tx connection is lost they will flash a short time a couple of times / second.

Overriding all this patterns except gyro calibration, is the low battery flash which is a slow, equally spaced on and off flash. 

###Linux support
See post by :
http://www.rcgroups.com/forums/showpost.php?p=34293596&postcount=1248

Read [INSTALL.md](INSTALL.md) for more information.

###Wiki
http://sirdomsen.diskstation.me/dokuwiki/doku.php?id=start

### 01.12.16
* added telemetry for devo tx

### 31.10.16
* gcc compilation fix for issue introduced by the high angle update
* RGB strip driver added for ws2812 ws2813 leds (settings in hardware.h)
* file hardware.h added for compatibility with other builds

### .10.16
* High angle update (level mode works up to 90 degrees)
* level mode drift bug fix
* auto voltage drop calculation added
* acro to level mode handling improved, now flips around roll or pitch if needed
* buzzer functionality added, on programming pins
* serial "out" can be used on one of the programming pins
* pwm deadtime bug fix when toggling direction

###14.08.16
* bluetooth beacon added
* flips
* other code changes from H8

###14.07.16
* some rx changes (hopping code)
* motor curve default changed
* pids changed to version by SirDomsen

###19.05.16
* added 3d throttle option to dual mode
* (older) added "native inverted mode" to dual mode code

###15.02.16
* added inverted flight to dual mode

###15.02.16
* added dual mode
* fixed calibration bug introduced yesterday

###update 1: 6.12.15
* added stock tx support



