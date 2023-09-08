CXX := clang++
CC := clang

ifeq ($(shell uname -s),Darwin)
SHLIB_SUFFIX := dylib
DARWIN_DYLIB_FLAGS := -shared -fPIC -compatibility_version 1 -current_version 1 \
                      -dynamiclib
CCLIB        := $(CC) $(DARWIN_DYLIB_FLAGS)
else
SHLIB_SUFFIX := so
CCLIB        := $(CC) -shared -fPIC
endif

all: malloc_tester.$(SHLIB_SUFFIX)

clean:
	@rm -f malloc_tester.$(SHLIB_SUFFIX)

malloc_tester.$(SHLIB_SUFFIX): malloc_tester.c interpose.h
	$(CCLIB) -I../../include --std=c99 -o $@ $< -ldl


.PHONY: all clean test