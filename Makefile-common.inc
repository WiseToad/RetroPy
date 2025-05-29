include Makefile-platform.inc

# Default build type may be reassigned when launching make:
# make ... BUILD={release|debug}
BUILD = release
BUILD_DIR = build/$(BUILD)

CXX = g++
CXXFLAGS = -std=c++17 -fPIC -Wall
CXXFLAGS_release = -O2 -DNDEBUG
CXXFLAGS_debug = -g3

LNK = g++
LNKFLAGS =
LNKFLAGS_release = -s
LNKFLAGS_debug =

LIBS = 
