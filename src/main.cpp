#include <string>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>
#include "AudioFile.h"
#include <climits>
#include "SpeexResampler.h"

using namespace speexport;

const int picorate=5512;

// Return the Fibonacci number
// with the smallest difference to num

int nearestFibonacci(int num)
{
	if (num == 0) return 0;  
	int first = 0;
	int second = 1;
	int third = first + second;

	while (third <= num) {
		first = second;
		second = third;
		third = first + second;
	}

	return (abs(third - num) >= abs(second - num)) ? second : third;
}

// Pico 8 ascii lookup

std::vector<std::string> P8SCII = {"\\000", "¹","²","³","⁴","⁵","⁶","⁷","⁸","\\t","\\n","ᵇ","ᶜ","\\r","ᵉ","ᶠ","▮","■","□","⁙","⁘","‖","◀","▶","「","」","¥","•","、","。","゛","゜"," ","!","\\\"","#","$","%","&","'","(",")","*","+",",","-",".","/","0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?","@","𝘢","𝘣","𝘤","𝘥","𝘦","𝘧","𝘨","𝘩","𝘪","𝘫","𝘬","𝘭","𝘮","𝘯","𝘰","𝘱","𝘲","𝘳","𝘴","𝘵","𝘶","𝘷","𝘸","𝘹","𝘺","𝘻","[","\\\\","]","^","_","`","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","{","|","}","~","○","█","▒","🐱","⬇️","░","✽","●","♥","☉","웃","⌂","⬅️","😐","♪","🅾️","◆","…","➡️","★","⧗","⬆️","ˇ","∧","❎","▤","▥","あ","い","う","え","お","か","き","く","け","こ","さ","し","す","せ","そ","た","ち","つ","て","と","な","に","ぬ","ね","の","は","ひ","ふ","へ","ほ","ま","み","む","め","も","や","ゆ","よ","ら","り","る","れ","ろ","わ","を","ん","っ","ゃ","ゅ","ょ","ア","イ","ウ","エ","オ","カ","キ","ク","ケ","コ","サ","シ","ス","セ","ソ","タ","チ","ツ","テ","ト","ナ","ニ","ヌ","ネ","ノ","ハ","ヒ","フ","ヘ","ホ","マ","ミ","ム","メ","モ","ヤ","ユ","ヨ","ラ","リ","ル","レ","ロ","ワ","ヲ","ン","ッ","ャ","ュ","ョ","◜","◝"};

std::vector<std::string> compressionCode = { "u8", "fd4" };

int main(int argc, char* argv[]) {
    if (argc==1) {std::cout << "usage: " << argv[0] << " [OPTION] [FILE]...\n\t-u 8 bit uncompressed\n\t-f 4 bit Fibonacci Delta\n"; return 0;}
    int speex_err=0;
    SpeexResampler resampler=SpeexResampler();
    int ccode=0;
	for (int i = 1; i < argc; i++) {
	    std::string s(argv[i]);
	    if (s=="-u") {ccode=0; std::cout << "Mode: 8 bit uncompressed" << std::endl; continue;}
   	    if (s=="-f") {ccode=1;  std::cout << "Mode: 4 bit Fibonacci Delta" << std::endl; continue;}
		AudioFile<float> a;
		bool loadedOK = a.load (s);
		if (loadedOK) {
			std::cout << "Processing: " << s << std::endl;
			std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
			std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
			std::cout << "Num Channels: " << a.getNumChannels() << std::endl;
			if (a.getNumChannels() !=1) {
				std::cout << std::endl << "Skipped: Requires mono file\n" << std::endl;
				continue;				
			}
			unsigned int numSamples = a.getNumSamplesPerChannel();
			std::cout << "Input Samples: " << numSamples << std::endl;
			
			unsigned int targSamples = (float)numSamples * (float)picorate / (float)a.getSampleRate();
			std::cout << "Target Samples: " << targSamples << std::endl;
			if(targSamples > 32767) std::cout <<"Warning: Pico 8 maximum integer exceeded by target samples" << std::endl;
			
			std::vector<float> target(targSamples, 0.0f);
			
			resampler.init(1, a.getSampleRate(), picorate, 10, &speex_err);
			
			resampler.process(0, a.samples[0].data(), &numSamples, target.data(), &targSamples);
			
			s.replace(s.end()-3,s.end(),"lua");
			std::ofstream output;
			output.open(s);
			output << s.substr(0,s.length()-4);
			output << "={c=\"" << compressionCode[ccode] << "\",n=" << targSamples << ",s=\"";
			
			for (int o=0; o < targSamples; o++)
			{
				output << P8SCII[127*target[o]+127];
			}
			
			output << "\"}" << std::endl;
			output.close();
			
			std::cout << std::endl << "Saved: " << s << std::endl <<std::endl;		
		}
		else std::cout << std::endl;
	}
	return 0;
}
