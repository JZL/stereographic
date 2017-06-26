CC = gcc
CFLAGS = -g -Wall -Wvla -lm #-Werror -Wno-error=unused-variable

all: stereo

stereo: RayPolygon
	$(CC) $(CFLAGS) -o stereo stereo.c RayPolygon.o -lm

RayPolygon:
	$(CC) $(CFLAGS) -c RayPolygon.c

clean:
	$(RM) *.o stereo
