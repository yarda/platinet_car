/*
 * Header file for platinet_car.
 *
 * Copyright (C) 2015 Yarda <zbox AT atlas.cz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define NAME platinet_car
#define VERSION 0.1
#define YEAR 2015
#define AUTHOR Yarda

#define xstr(s) str(s)
#define str(s) #s

#define BTNAME "iS650"
#define BTCHAN 6
#define SPEED_FACTOR 50
#define WHEEL_STEP 5
// 255 is max speed, i.e. no limit
#define SPEED_LIMIT 255
