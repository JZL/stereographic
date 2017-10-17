CC = gcc
CFLAGS = -O3 -g -Wall -Wvla -lm -pthread #-Werror -Wno-error=unused-variable

all: img

img: stereo
	./stereo && ./makeIcosahedronNet.sh

stereo: RayPolygon coords.c
	$(CC) $(CFLAGS) -O3 -o stereo stereo.c RayPolygon.o -lm

RayPolygon:
	$(CC) $(CFLAGS) -c RayPolygon.c

coords.c: icosahedronCoord.py
	python icosahedronCoord.py

clean:
	$(RM) *.o stereo
	$(RM) out/*pgm out/png/*png out/png/rot/*
	$(RM) net.png cropped.png withSpaces.pbm in.pbm
