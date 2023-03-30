default: build clean

PROJECT=brainfreak

build:
	cmake -Bbuild -H.
	cmake --build build --config Release -j 2 --target all --
	-mkdir out
	cp build/${PROJECT} out/
clean:
	rm -rf build
clear: clean
	rm -rf out