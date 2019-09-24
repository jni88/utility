MK_GOALS := $(sort debug $(MAKECMDGOALS))

$(MK_GOALS):
	@make -C jnu/ $@
	@make -C jnu_test/ $@
