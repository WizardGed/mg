# $OpenBSD: Makefile,v 1.33 2016/09/16 17:17:40 tedu Exp $

PROG=	mg

LDADD+=	`pkg-config --libs ncurses` -lutil
DPADD+=	${LIBNCURSES} ${LIBUTIL}

# (Common) compile-time options:
#
#	REGEX		-- create regular expression functions.
#	STARTUPFILE	-- look for and handle initialization file.
#
CFLAGS+=-Wall -DREGEX `pkg-config --cflags-only-I ncurses`

SRCS=	autoexec.c basic.c bell.c buffer.c cinfo.c dir.c display.c \
	echo.c extend.c file.c fileio.c funmap.c help.c kbd.c keymap.c \
	line.c macro.c main.c match.c modes.c paragraph.c \
	re_search.c region.c search.c spawn.c tty.c ttyio.c ttykbd.c \
	undo.c util.c version.c window.c word.c yank.c

#
# More or less standalone extensions.
#
SRCS+=	cmode.c cscope.c dired.c grep.c tags.c

OS!=	uname

.if ${OS:MDragonFly}
SRCS+=	reallocarray.c
.endif

afterinstall:
	${INSTALL} -d -o root -g wheel ${DESTDIR}${DOCDIR}/mg
	${INSTALL} ${INSTALL_COPY} -o ${DOCOWN} -g ${DOCGRP} -m ${DOCMODE} \
	    ${.CURDIR}/tutorial ${DESTDIR}${DOCDIR}/mg

.include <bsd.prog.mk>
