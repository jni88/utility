DIR_SELF := $(dir $(lastword $(MAKEFILE_LIST)))

BIN_EXT := .a
BIN_PFX := lib
-include $(DIR_SELF)/common.mk

$(BIN_DEBUG): $(OBJS_DEBUG)
	ar rsc $@ $^

$(BIN_RELEASE): $(OBJS_RELEASE)
	ar rsc $@ $^
