Description:
============

This is an attempt to reverse engineer the protocol used by Platinet cars.


Requirements:
=============

- bluez-libs-devel
- SDL-devel


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

KEY_UP    - increase speed

KEY_DOWN  - decrease speed

KEY_a     - change gear to forward

KEY_z     - change gear to rear

KEY_SPACE - break, quickly stops and light brake lights

KEY_LEFT  - turn left

KEY_RIGHT - turn righ

The protocol supports progressive wheel, but it seems that not all cars have
HW support for it, i.e. they only do full left / right.

KEY_l     - switch front lights on/off if car is not moving, this is
            protocol hack :)


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


Todo:
=====

- Add support for gamepad


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
