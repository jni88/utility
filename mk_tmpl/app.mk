DIR_SELF := $(dir $(lastword $(MAKEFILE_LIST)))

-include $(DIR_SELF)/common.mk
-include $(DIR_SELF)/link.mk
