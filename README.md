Description:
============

This is an attempt to reverse engineer the protocol used by Platinet cars.


Requirements:
=============

- bluez-libs-devel
- SDL2-devel (or SDL-devel)


Compilation:
============

$ make

By default it is compiled with SDL2. It can be also compiled with SDL1:

$ make CFLAGS=-DSDL1 SDL_LIBS=-lSDL


Usage:
======

Give HCI interface up:

 hciconfig hci0 up

Connect to car with default bluetooth name "iS650":

 platinet_car

Connect to car with bluetooth name "CAR":

 platinet_car -n CAR

Connect to car with bluetooth address "ADDR":

 platinet_car -a ADDR

Connect to car with bluetooth address "ADDR" and use channel "CHAN":

 platinet_car -a ADDR -c CHAN

Use speed limiter and limit speed to 50 % (for novice drivers or
for small apartments :), limit can be set to 0 - 255):

 platinet_car -l 128

It is also possible to run the application in the simulation mode to test
the controls. The car is not needed in this mode:

 platinet_car -s


Controls:
=========

KEY_UP    - accelerate

KEY_DOWN  - decelerate

KEY_a     - change gear to forward

KEY_z     - change gear to reverse

KEY_SPACE - break, quickly stops and light brake lights

KEY_LEFT  - steer left

KEY_RIGHT - steer righ

The protocol supports progressive wheel, but it seems that not all cars have
HW support for it, i.e. they only do full left / right.

KEY_l     - switch front lights on/off if car is not moving, this is
            protocol hack :)

Analog and digital joysticks / gamepads are also supported. Buttons / axis
mappings are configurable. Default mappings is the following:

JOY_UP (analog axis 1 / y-axis)    - change gear to forward, accelerate

JOY_DOWN (analog axis 1 / y-axis)  - change gear to rear, decelerate

JOY_LEFT (analog axis 2 / x-axis)  - stear left

JOY_RIGHT (analog axis 2 / x-axis) - stear right

JOY_BUT_0                          - accelerate

JOY_BUT_2                          - decelerate

JOY_BUT_3                          - stear left

JOY_BUT_1                          - stear right

JOY_BUT_6                          - break, quickly stops and light brake lights

JOY_BUT_5                          - change gear to forward

JOY_BUT_7                          - change gear to reverse


Joystick configuration:
=======================

Joystick can be configured from the command line. Arbitrary mappings
of axis / buttons can be selected. Axis can be inverted. The following
command line options are currently supported:

-X, --joystick-x-axis XAXIS          - axis number for x-axis

-Y, --joystick-y-axis YAXIS          - axis number for y-axis

-x, --joystick-invert-x-axis         - invert x-axis

-y, --joystick-invert-y-axis         - invert y-axis

-F, --joystick-forward-button FBUT   - button number for forward button

-B, --joystick-backward-button BBUT  - button number for backward button

-L, --joystick-left-button LBUT      - button number for left button

-R, --joystick-right-button RBUT     - button number for right button

-G, --joystick-light-button GBUT     - button number for button controlling lights

-K, --joystick-brake-button KBUT     - button number for button controlling brake

-U, --joystick-gear-up-button UBUT   - button number for button changing gears up

-D, --joystick-gear-down-button DBUT - button number for button changing gears down

There is the -s, --simulate option which allows checking the mappings
without the car (i.e. not to crash due to bad mappings :).


Hacking:
========

If accelerating very slowly on surface with big friction (e.g. carpet),
the engine may overheat. It seems there is protection in the firmware
which disconnects the engine if the car doesn't achieve some minimal
speed in defined time. In such case you need to reset the protection
by sending idle command, otherwise the car will not respond to other
commands. I tried to workaround this problem by using non-linear
control of speed, but you may still trigger this problem under some
adverse conditions. In such case just hit the brake to send the idle
command.

It seems that the original Platinet application for Android
(as of version 1.2) sometimes send packets where the last (checksum?)
byte has different value than calculated by this code. I think this is
due to bug in the Android application


License:
========

Copyright (C) 2015 Yarda <zbox AT atlas.cz>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Full text of the license is enclosed in COPYING file.
