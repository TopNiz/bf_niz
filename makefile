all: bfcode

bfcode: main.c
	gcc \
	-Wall \
	-I/usr/local/opt/ncurses/include \
	-D_XOPEN_SOURCE_EXTENDED \
	-L/usr/local/opt/ncurses/lib \
	-lncursesw \
	main.c \
	-o bfcode \
	-g

clean:
	rm bfcode
