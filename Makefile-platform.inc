PLATFORM = $(shell gcc -dumpmachine)

ifneq (, $(findstring linux, $(PLATFORM)))
    OS = linux
else ifneq (, $(findstring mingw32, $(PLATFORM)))
    OS = windows
else
    OS =
endif

ifneq (, $(findstring x86_64, $(PLATFORM)))
    ARCH = x86_64
else ifneq (, $(findstring aarch64, $(PLATFORM)))
    ARCH = aarch64
else
    ARCH =
endif
