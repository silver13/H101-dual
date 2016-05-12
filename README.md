# Floureon H101 acro firmware 

*now with dual mode as well*


Select folder for the correct firmware:
* H101_Acro: Acro only, inverted flight possible (manual on a switch)
* H101_dual: either/both modes, inverted flight possible (manual on a switch)


For programming the ground pad is marked (B-) on the pcb(near the edge) .It is also close to some other components. Note there is also a (B+) pad,do not connect this. 

It works with Devo , stock or H8 mini tx.You could also use the diy [nrf24_multipro](https://github.com/goebish/nrf24_multipro) module by Goebish.

*** DO NOT ACCIDENTALLY FLASH THE H8MINI FIRMWARE TO THE H101*** (it may break it)


 * GigaDevice GD32F130G6 cortex-M3 32k
 * Invensense gyro + accelerometer
 * XN297 transceiver
 * 8 Nfets and 8 Pfets. The P fets are controlled commonly in groups of 4.


Cpu datasheet: [Pdf](https://app.box.com/s/3zi661iffmit1rwda499wu8vycv03biv) Cpu Documentation: [Pdf](https://app.box.com/s/pehsanvluc40qu8k2036sbjk5ti08y2m)

The firmware needs Keil.GD32F1xx_DFP.1.1.0.pack which adds support for the cpu to Keil (5.15 used).


The firmware flash procedure is the same as the H8.

Firmware thread featuring flashing info : [rcgroups.com](http://www.rcgroups.com/forums/showthread.php?t=2512604)


#####Accelerometer calibration ***(level mode only)***

For accelerometer calibration move the pitch stick down 3 times within about 1- 2 seconds. Wait a couple of seconds after a failed attempt. Throttle has to be low, and roll centered. Flashing lights indicate the calibration process. This is saved so it has to be done only once.

Note, the acc calibration also saves gyro biases which are used in some cases. The flash pattern is similar to the gyro calibration pattern.

*Calibration has to be done on a horizontal surface*

###Wiki
http://sirdomsen.diskstation.me/dokuwiki/doku.php?id=start

###15.02.16
* added inverted flight to dual mode

###15.02.16
* added dual mode
* fixed calibration bug introduced yesterday

###update 1: 6.12.15
* added stock tx support



