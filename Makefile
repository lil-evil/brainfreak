default: build-linux

PROJECT=brainfreak

build-linux:
	cmake -Bbuild -H.
	cmake --build build --config Release -j 2 --target all --
	-mkdir out
	cp build/${PROJECT} out/
build-windows:
	i686-w64-mingw32-gcc -Isrc/include -o out/brainfreak.exe src/main.c src/bf.c src/opt.c -Os -s -std=c11

clean:
	rm -rf build
clear: clean
	rm -rf out

all : build-linux build-windows clean