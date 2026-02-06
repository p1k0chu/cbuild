# Copyright (C) 2026 p1k0chu
# SPDX-License-Identifier: GPL-3.0-or-later

CFLAGS += -Wall -Wextra -MMD -iquote ./include -std=gnu23	\
	-Werror=implicit-int -Wconversion			\
	-Werror=implicit-fallthrough

SRCS := error.c mtime.c object.c utils.c target.c compile.c	\
	compilemyself.c filename.c
OUT := libcbuild.a

objs = $(SRCS:.c=.o)
dep = $(SRCS:.c=.d)

$(OUT): $(objs)
	$(AR) rcs $@ $^

-include $(dep)

clean:
	-$(RM) -v $(OUT) $(objs) $(dep)
.PHONY: clean

clean-objs:
	-$(RM) -v $(objs) $(dep)
.PHONY: clean-objs
