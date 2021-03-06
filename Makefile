# Makefile for platinet_car
#
# Copyright (C) 2015 Yarda <zbox AT atlas.cz>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

NAME = platinet_car
SED_VERSION_PATTERN = s/^\s*\#define\s\+VERSION\s\+\(\S\+\)$$/\1/ p
VERSION = $(shell sed -n '/define\s\+VERSION\s\+/ $(SED_VERSION_PATTERN)' platinet_car.h)

INSTALL := install
PREFIX ?= /usr
INSTALL_FOLDER ?= $(PREFIX)/bin
GCC = gcc
TAR = tar

SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL_LIBS = $(shell pkg-config --libs sdl2 || echo -lSDL2)

LIBS=-lbluetooth

.PHONY: clean archive

$(NAME): platinet_car.c platinet_car.h
	$(GCC) -o $(NAME) $(SDL_CFLAGS) $(CFLAGS) $(LIBS) $(SDL_LIBS) $(LDFLAGS) platinet_car.c

install:
	$(INSTALL) -m 755 -d							$(DESTDIR)$(INSTALL_FOLDER)
	$(INSTALL) -m 755 $(NAME)						$(DESTDIR)$(INSTALL_FOLDER)/

clean:
	rm -f $(NAME)

archive:
	$(TAR) -cjf $(NAME)-$(VERSION).tar.bz2 platinet_car.c platinet_car.h README.md COPYING
