Description:
============

This is an attempt to reverse engineer the protocol used by Platinet cars.


Requirements:
=============

bluez-libs-devel
SDL-devel


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


Controls:
=========

KEY_UP    - go forward & increase speed, brake if going backward
KEY_DOWN  - go backward, increase speed, brake if going forward

If key is released, car maintain its speed. Use opposite key
to brake. You need to release the key and press it again to
to accelerate in opposite direction.

KEY_LEFT  - turn left
KEY_RIGHT - turn righ

The protocol support progressive wheel, but not all cars have
HW support for it, i.e. they only support full left & full right.

KEY_l     - switch front lights on/off if car is not moving
KEY_b     - switch rear (stop) lights on/off if car is not moving

These two are protocol hacks :) and works only if car is not moving.


Todo:
=====

- Improve controls
- Make speed increment non-linear
- Real braking, currently it only switch off motor and let the rest
  on car firmware, which seems to only lights stoplights. This is not
  enough on flat (slippery) surfaces.


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
