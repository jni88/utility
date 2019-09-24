DIR_MK_TMPL := ../mk_tmpl
DIR_DEBUG_BIN := ../bin
DIR_RELEASE_BIN := ../bin
CC_INCLUDE_EXT := ../include
CC_LK_OBJS_DEBUG := $(DIR_DEBUG_BIN)/libjnu_d.a
CC_LK_OBJS_RELEASE := $(DIR_RELEASE_BIN)/libjnu.a
BIN_DEBUG_NAME := jnu_test_d
BIN_RELEASE_NAME := jnu_test

.PHONY: test_debug test_release

-include $(DIR_MK_TMPL)/app.mk

debug: test_debug
test_debug:
	@$(BIN_DEBUG)

release: test_release
test_release:
	@$(BIN_RELEASE)
