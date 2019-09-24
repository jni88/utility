DIR_HOME ?= .
DIR_INCLUDE ?= $(DIR_HOME)/include
DIR_SRC ?= $(DIR_HOME)/src
DIR_BUILD ?= build
DIR_DEBUG_BUILD ?= $(DIR_BUILD)/debug
DIR_RELEASE_BUILD ?= $(DIR_BUILD)/release
DIR_DEBUG_BIN ?= $(DIR_HOME)/bin
DIR_RELEASE_BIN ?= $(DIR_HOME)/bin

BIN_EXT ?=
BIN_PFX ?=
BIN_DEBUG_NAME ?= out_d
BIN_RELEASE_NAME ?= out

SRC_EXT ?= .cc
CC_BIN ?= g++
CC_STD ?= c++17
CC_MARCH ?= x86-64
CC_WARN ?= -Wall -Wpedantic
CC_INCLUDE_EXT ?=
CC_FLAGS_EXT ?=
CC_FLAGS_DEBUG ?= -g
CC_FLAGS_RELEASE ?= -O2

CC_FLAGS := $(CC_WARN) -std=$(CC_STD) -march=$(CC_MARCH) -I$(DIR_INCLUDE) \
$(patsubst %, -I%, $(CC_INCLUDE_EXT)) $(CC_FLAGS_EXT) -MMD -MP -MF

TMP_DIRS_BUILD := $(DIR_DEBUG_BUILD) $(DIR_RELEASE_BUILD) $(DIR_BUILD)
TMP_DIRS_BIN := $(DIR_DEBUG_BIN) $(DIR_RELEASE_BIN)
TMP_DIRS := $(sort $(TMP_DIRS_BUILD) $(TMP_DIRS_BIN))

FILES_SRC := $(wildcard $(DIR_SRC)/*$(SRC_EXT))

BIN_DEBUG := $(DIR_DEBUG_BIN)/$(BIN_PFX)$(BIN_DEBUG_NAME)$(BIN_EXT)
BIN_RELEASE := $(DIR_RELEASE_BIN)/$(BIN_PFX)$(BIN_RELEASE_NAME)$(BIN_EXT)
FILES_BIN := $(BIN_DEBUG) $(BIN_RELEASE)

OBJS_DEBUG := $(patsubst $(DIR_SRC)/%$(SRC_EXT), \
$(DIR_DEBUG_BUILD)/%.o, $(FILES_SRC))

DEPS_DEBUG := $(patsubst $(DIR_SRC)/%$(SRC_EXT), \
$(DIR_DEBUG_BUILD)/%.d, $(FILES_SRC))

OBJS_RELEASE := $(patsubst $(DIR_SRC)/%$(SRC_EXT), \
$(DIR_RELEASE_BUILD)/%.o, $(FILES_SRC))

DEPS_RELEASE := $(patsubst $(DIR_SRC)/%$(SRC_EXT), \
$(DIR_RELEASE_BUILD)/%.d, $(FILES_SRC))

.PHONY: debug release all clean

debug: $(BIN_DEBUG)
$(BIN_DEBUG): | $(DIR_DEBUG_BIN)

release: $(BIN_RELEASE)
$(BIN_RELEASE): | $(DIR_RELEASE_BIN)

all: debug release

clean:
	@rm -f $(FILES_BIN)
	@rm -f $(DIR_DEBUG_BUILD)/*.o $(DIR_DEBUG_BUILD)/*.d
	@rm -f $(DIR_RELEASE_BUILD)/*.o $(DIR_RELEASE_BUILD)/*.d
	@rm -df $(TMP_DIRS_BUILD) 2> /dev/null | true
	@rm -df $(TMP_DIRS_BIN) 2> /dev/null | true

$(DIR_DEBUG_BUILD)/%.o: $(DIR_SRC)/%$(SRC_EXT) | \
												$(DIR_BUILD) $(DIR_DEBUG_BUILD)
	$(CC_BIN) $(CC_FLAGS_DEBUG) $(CC_FLAGS) $(@:.o=.d) -c $< -o $@

$(DIR_RELEASE_BUILD)/%.o: $(DIR_SRC)/%$(SRC_EXT) | \
													$(DIR_BUILD) $(DIR_RELEASE_BUILD)
	$(CC_BIN) $(CC_FLAGS_RELEASE) $(CC_FLAGS) $(@:.o=.d) -c $< -o $@

$(TMP_DIRS):
	@if [ ! -d "$@" ]; then mkdir "$@"; fi

ifeq (, $(filter $(MAKECMDGOALS), all))
-include $(DEPS_DEBUG)
-include $(DEPS_RELEASE)
else
ifeq (, $(filter $(MAKECMDGOALS), debug))
-include $(DEPS_DEBUG)
endif
ifeq (, $(filter $(MAKECMDGOALS), release))
-include $(DEPS_RELEASE)
endif
endif
