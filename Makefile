# Copyright (C) 2026 p1k0chu
# SPDX-License-Identifier: GPL-3.0-or-later

CFLAGS += -Wall -Wextra -MMD -iquote ./include -std=gnu23	\
	-Werror=implicit-int -Wconversion			\
	-Werror=implicit-fallthrough

SRCS := mtime.c object.c utils.c target.c compile.c		\
	compilemyself.c filename.c custom_target.c pkgconfig.c
OUT_STATIC := libcbuild.a
OUT_SHARED := libcbuild.so

objs = $(SRCS:.c=.o)
dep = $(SRCS:.c=.d)

shared: $(OUT_SHARED)
static: $(OUT_STATIC)
.PHONY: static shared

all: static shared
.PHONY: all

$(OUT_STATIC): $(objs)
	$(AR) rcs $@ $^

$(OUT_SHARED): $(objs)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) $(CFLAGS) -shared -o $@

-include $(dep)

clean:
	-$(RM) -v $(OUT_STATIC) $(OUT_SHARED) $(objs) $(dep)
.PHONY: clean

clean-objs:
	-$(RM) -v $(objs) $(dep)
.PHONY: clean-objs
