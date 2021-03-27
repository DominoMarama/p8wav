# p8wav
Converts wav files to lua for use on pico 8

usage: p8wav [OPTIONS] [FILE]...
	-u 8 bit uncompressed
	-f 4 bit Fibonacci Delta
	-c copy to clipboard

## Build

	git clone --recurse-submodules --depth=1 git://github.com/DominoMarama/p8wav.git
	cd p8wav
	mkdir build
	cd build
	cmake ..
	make
	make install

## TO DO:

Implement fibonacci delta encoding

