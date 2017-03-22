COMPILER_TYPE= gnu
CC=gcc
PROG= penaltix
SRCS= main.c vbe.c video_gr.c Mouse.c Bitmap.c keyboard.c timer.c Menu.c Ball.c rtc.c game.c game_multi.c serial.c player.c goalkeeper.c About.c
CCFLAGS= -Wall
DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys
LDFLAGS+= -L .
MAN=
BINDIR?= /usr/sbin
.include <bsd.prog.mk>
.include <bsd.gcc.mk>
