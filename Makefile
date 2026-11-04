# Makefile for building for TI-CE, use CMake for all other platforms.

NAME = Sigmar
ICON = ./assets/texture/icon_ce.png
DESCRIPTION = "OpenSigmar"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz -D SIGMAR_TI=1
CXXFLAGS = -Wall -Wextra -Oz
OBJDIR = build/obj

# ----------------------------

include $(shell cedev-config --makefile)