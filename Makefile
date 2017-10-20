CC = clang
CFLAGS = -O3 -ggdb -Wall -Wvla -lm -pthread  #-fPIC -fsanitize=memory -fPIE -pie -fsanitize-memory-track-origins #fPIC for sanitize=memory
#export MSAN_SYMBOLIZER_PATH="/usr/lib/llvm-3.8/bin/llvm-symbolizer" Not in bin bc not unadorned https://stackoverflow.com/questions/38079761/why-does-asan-symbolizer-path-no-longer-work-with-version-adorned-binaries

all: img

img: stereo
	./stereo && ./makeIcosahedronNet.sh

stereo: RayPolygon managePNG coords.c
	#Needs to end with lpthread bc gcc is picky and wants specific order
	$(CC) $(CFLAGS) -o stereo stereo.c RayPolygon.o managePNG.o -lm -lpng # -fno-omit-frame-pointer -fsanitize=address #

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
