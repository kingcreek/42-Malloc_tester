CXX := clang++
CC := clang
SHLIB_SUFFIX := so

ifeq ($(shell uname -s),Darwin)
	DARWIN_DYLIB_FLAGS := -shared -fPIC -compatibility_version 1 -current_version 1 -dynamiclib
	CCLIB := $(CC) $(DARWIN_DYLIB_FLAGS)
	SHLIB_SUFFIX := dylib
else
	CCLIB := $(CC) -shared -fPIC -lc
endif

# Directorios de origen y compilación
SRC_DIR := src
BUILD_DIR := build

# Lista de archivos .c en el directorio de origen
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Objetos generados en el directorio de compilación
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

all: mkdir build malloc_tester.$(SHLIB_SUFFIX)

clean:
	@rm -f malloc_tester.$(SHLIB_SUFFIX) $(OBJ_FILES)

mkdir:
	$(shell mkdir -p $(BUILD_DIR))

malloc_tester.$(SHLIB_SUFFIX): $(OBJ_FILES)
	$(CCLIB) -o $@ $^ -ldl

re: clean all

# Regla genérica para compilar archivos .c en objetos
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -I /include -fPIC -o $@ $<

.PHONY: all clean mkdir
