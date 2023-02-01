CORE_DIR := $(realpath $(CURDIR)/../../core)
CORE_SRC_DIR := $(CORE_DIR)/src
BUILD_DIR := ./build
CORE_BUILD_DIR := $(BUILD_DIR)/core

TARGET = vid2led

SRC = $(wildcard $(CURDIR)/*.c)
OBJ = $(subst $(CURDIR),$(BUILD_DIR),$(SRC:.c=.o))

SRC_CORE = $(wildcard $(CORE_SRC_DIR)/*.c)
OBJ_CORE = $(subst $(CORE_SRC_DIR),$(CORE_BUILD_DIR),$(SRC_CORE:.c=.o))

OBJS = $(OBJ) $(OBJ_CORE)
DEPS = $(OBJS:.o=.d)

INCLUDES = -I$(CORE_DIR)/include

CFLAGS = -MMD -Wall -Wextra -O0 -g3 $(INCLUDES)
LDFLAGS = -lm

-include $(DEPS)

all: make_dir $(TARGET)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(CORE_BUILD_DIR)/%.o: $(CORE_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJ) $(OBJ_CORE)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)

make_dir:
	@mkdir -p ./build
	@mkdir -p ./build/core

.PHONY: clean
clean:
	rm -rf ./build vid2led
