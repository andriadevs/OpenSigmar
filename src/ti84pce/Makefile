# Makefile for building for TI-CE, use CMake for all other platforms.

NAME = Sigmar
ICON = ./assets/ti84pce/icon_ce.png
DESCRIPTION = "OpenSigmar"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz -Iinclude -DSIGMAR_TI=1
CXXFLAGS = -Wall -Wextra -Oz
SRCDIR = src/ti84pce
OBJDIR = build/obj
EXTRA_C_SOURCES = src/Sigmar.c

# ----------------------------

include $(shell cedev-config --makefile)