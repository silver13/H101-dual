# Floureon H101 acro firmware 

This is an acro firmware for the Floureon H101 quadcopter.

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

###update 1: 6.12.15
*added stock tx support



