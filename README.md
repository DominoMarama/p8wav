# p8wav
Converts wav files to lua for use on pico 8

## Building

	git clone --recurse-submodules --depth=1 git://github.com/DominoMarama/p8wav.git
	cd p8wav
	mkdir build
	cd build
	cmake ..
	make
	make install

## Using
	usage: p8wav [OPTIONS] [FILE]...
		-u 8 bit uncompressed
		-f 4 bit Fibonacci Delta
		-c copy to clipboard

Your source wav files should be mono. This is because the options of combining stereo channels or using only the right or left channel requires human intervention for best results.

The command line is processed serially so you can combine different compression types into a single output file.

	p8wav -c -u dontcompress.wav -f compress.wav

This will save both wav files and the decompression and playback scripts into a file called 'pcm_audio.lua' with the samples called "dontcompress" and "compress".

The output will also be copied to the clipboard so you can paste directly into a Pico 8 code tab when the -c option is used.


## Example Pico 8 script:
Once you have pasted the pcm_audio into your cartridge, you can use the samples easily from your code:

	function _init()
		cls()
		print("loading samples")
		init_pcm()
	end

	function _update60()
		if btnp(❎) then
			play_pcm("sample_name")
		end
		update_pcm()
	end

	function _draw()
		cls()
		if (pcm) then
			print("playing:"..pcm.k)
		else
			print("press ❎ to play pcm")
		end
	end

Note: init_pcm() is only required if you are using compression. If you only use uncompressed samples it will be omitted from the lua script to save tokens.

## License

Due to the use of a GPL3 library, this repository is licensed under GPL3. However any generated lua code from the compiled program is licensed CC0. Feel free to use the output in your own Pico 8 cartridges under whatever license your own code or audio uses. A mention would be nice, but is not required.