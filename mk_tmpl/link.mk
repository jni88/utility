CC_LK_LIBS ?=
CC_LK_FLAGS_EXT ?=
CC_LK_OBJS_RELEASE ?=
CC_LK_OBJS_DEBUG ?= $(CC_LK_OBJS_RELEASE)

CC_LK_FLAGS := $(CC_LK_FLAGS_EXT) $(patsubst %, -l%, $(CC_LK_LIBS))

$(BIN_DEBUG): $(OBJS_DEBUG) $(CC_LK_OBJS_DEBUG)
	$(CC_BIN) $(CC_FLAGS_DEBUG) -o $@ $^ $(CC_LK_FLAGS)

$(BIN_RELEASE): $(OBJS_RELEASE) $(CC_LK_OBJS_RELEASE)
	$(CC_BIN) $(CC_FLAGS_RELEASE) -o $@ $^ $(CC_LK_FLAGS)
