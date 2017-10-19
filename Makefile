CC = clang-3.9
CFLAGS = -O3 -g -Wall -Wvla -lm -pthread -lpng #-Werror -Wno-error=unused-variable

all: img

img: stereo
	./stereo && ./makeIcosahedronNet.sh

stereo: RayPolygon managePNG coords.c
	$(CC) $(CFLAGS) -o stereo stereo.c RayPolygon.o managePNG.o -lm

RayPolygon:
	$(CC) $(CFLAGS) -c RayPolygon.c

coords.c: icosahedronCoord.py
	python icosahedronCoord.py

managePNG: managePNG.c
	$(CC) $(CFLAGS) -c  managePNG.c

clean:
	$(RM) *.o stereo
	$(RM) out/*.p*m out/png/*.png* out/png/rot/* out/*.png
	$(RM) net.png cropped.png withSpaces.p*m in.p*m
