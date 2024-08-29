CC=gcc
LD_FLAGS=-lm -lopenal
CC_FLAGS=-Wall -Wextra -Wpedantic -g

BUILD_DIR=build
SRC_DIR=src
TARGET_BINARIES=minimal simple buffering

SOURCES=$(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c))
OBJECTS=$(subst $(SRC_DIR), $(BUILD_DIR), $(SOURCES:.c=.o))
DEPENDS=$(subst $(SRC_DIR), $(BUILD_DIR), $(SOURCES:.c=.d))
TARGETS=$(addprefix $(BUILD_DIR)/, $(TARGET_BINARIES))


# phony rules:
.PHONY: all clean

all: $(TARGETS)

clean:
	rm $(OBJECTS) $(TARGETS) $(DEPENDS) 2>/dev/null || true


# rules for building objects:
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c
	$(CC) $(CC_FLAGS) -E -MT $(subst $(SRC_DIR), $(BUILD_DIR), $(<:.c=.o)) \
		-MM -MP -MF $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)/%.d
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# rules for building target executables:
build_target=$(CC) $(CC_FLAGS) -o $(1) $(2) $(LD_FLAGS)

$(BUILD_DIR)/buffering: $(BUILD_DIR)/buffering.o $(BUILD_DIR)/errors.o \
						$(BUILD_DIR)/tone.o
	$(call build_target, $@, $^)

$(BUILD_DIR)/simple: $(BUILD_DIR)/simple.o $(BUILD_DIR)/errors.o \
				     $(BUILD_DIR)/tone.o
	$(call build_target, $@, $^)

$(BUILD_DIR)/%: $(BUILD_DIR)/%.o
	$(call build_target, $@, $<)


# include all dependency files but don't raise error if .d files are missing
-include $(DEPENDS)
