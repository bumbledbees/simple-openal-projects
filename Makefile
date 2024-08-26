CC=gcc
LD_FLAGS=-lm -lopenal
CC_FLAGS=-Wall -Wextra -Wpedantic -g

BUILD_DIR=build
SRC_DIR=src

VPATH=$(SRC_DIR)
SOURCES=$(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c))
OBJECTS=$(subst $(SRC_DIR), $(BUILD_DIR), $(SOURCES:.c=.o))

TARGET_BINARIES=minimal errors
TARGETS=$(addprefix $(BUILD_DIR)/, $(TARGET_BINARIES))

.PHONY: all clean

all: $(TARGETS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/%: $(BUILD_DIR)/%.o
	$(CC) $(CC_FLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	rm $(OBJECTS) $(TARGETS) 2>/dev/null || true
