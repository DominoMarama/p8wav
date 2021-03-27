#include <string>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>
#include "AudioFile.h"
#include <climits>
#include "SpeexResampler.h"
#include "clip.h"

using namespace speexport;

const int picorate=5512;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// Return the Fibonacci number
// with the smallest difference to num

int nearestFibonacci(int num)
{
	if (num == 0) return 0;
	int first = 0;
	int second = sgn(num);
	int third = first + second;

	while (abs(third) <= abs(num)) {
		first = second;
		second = third;
		third = first + second;
	}

	return (abs(third - num) >= abs(second - num)) ? second : third;
}

std::vector<int> FLUT = {-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

// Pico 8 ascii lookup

std::vector<std::string> P8SCII = {"\\000", "¹","²","³","⁴","⁵","⁶","⁷","⁸","\\t","\\n","ᵇ","ᶜ","\\r","ᵉ","ᶠ","▮","■","□","⁙","⁘","‖","◀","▶","「","」","¥","•","、","。","゛","゜"," ","!","\\\"","#","$","%","&","'","(",")","*","+",",","-",".","/","0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?","@","𝘢","𝘣","𝘤","𝘥","𝘦","𝘧","𝘨","𝘩","𝘪","𝘫","𝘬","𝘭","𝘮","𝘯","𝘰","𝘱","𝘲","𝘳","𝘴","𝘵","𝘶","𝘷","𝘸","𝘹","𝘺","𝘻","[","\\\\","]","^","_","`","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","{","|","}","~","○","█","▒","🐱","⬇️","░","✽","●","♥","☉","웃","⌂","⬅️","😐","♪","🅾️","◆","…","➡️","★","⧗","⬆️","ˇ","∧","❎","▤","▥","あ","い","う","え","お","か","き","く","け","こ","さ","し","す","せ","そ","た","ち","つ","て","と","な","に","ぬ","ね","の","は","ひ","ふ","へ","ほ","ま","み","む","め","も","や","ゆ","よ","ら","り","る","れ","ろ","わ","を","ん","っ","ゃ","ゅ","ょ","ア","イ","ウ","エ","オ","カ","キ","ク","ケ","コ","サ","シ","ス","セ","ソ","タ","チ","ツ","テ","ト","ナ","ニ","ヌ","ネ","ノ","ハ","ヒ","フ","ヘ","ホ","マ","ミ","ム","メ","モ","ヤ","ユ","ヨ","ラ","リ","ル","レ","ロ","ワ","ヲ","ン","ッ","ャ","ュ","ョ","◜","◝"};

std::vector<std::string> compressionCode = { "u8", "fd4" };
std::vector<bool> usedCodes = { false, false };
bool usedEncode = false;

int main(int argc, char* argv[]) {
    if (argc<2) {std::cout << "usage: " << argv[0] << " [OPTION] [FILE]...\n\t-u 8 bit uncompressed\n\t-f 4 bit Fibonacci Delta\n"; return 0;}
    int speex_err=0;
    bool copyClip = false;
    SpeexResampler resampler=SpeexResampler();
    int ccode=0;
	std::ofstream output;
	output.open("pcm_audio.lua");
	output << "-- pcm audio\n\npcm_buffer=0x4300\n\nsamples={}\n";

	for (int i = 1; i < argc; i++) {
	    std::string s(argv[i]);
	    if (s=="-u") {ccode=0; std::cout << "Mode: Uncompressed 8 bit" << std::endl; continue;}
   	    if (s=="-f") {ccode=1;  std::cout << "Mode: Fibonacci Delta 4 bit" << std::endl; continue;}
   	    if (s=="-c") {copyClip=true; continue;}
		AudioFile<float> a;
		std::cout << "Processing: " << s << std::endl;
		bool loadedOK = a.load (s);
		if (loadedOK) {
			std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
			std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
			std::cout << "Channels: " << a.getNumChannels() << std::endl;
			if (a.getNumChannels() !=1) {
				std::cout << std::endl << "Skipped: Requires mono file" << std::endl << std::endl;
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
			
			output << "samples[\"" << s.substr(0,s.length()-4) << "\"]={c=\"" << compressionCode[ccode] << "\",";
			output << "n=" << targSamples << ",s=\"";

			usedCodes[ccode] = true;
			if (ccode > 0) usedEncode = true;
			switch(ccode)
			{
				case 0: // uncompressed 8 bit
				{
					for (int o=0; o < targSamples; o++)
					{
						output << P8SCII[127*target[o]+127];
					}
					break;
				}
				case 1: // Fibonacci delta 4 bit
				{
					int prev = 127*target[0];
					output << P8SCII[prev+127];
					unsigned char encoded = 0;
					for (int o=1; o < targSamples; o++)
					{
						int current = 127*target[o];
						int fDelta = nearestFibonacci(current-prev);

						if (fDelta < -34)
						{
							fDelta = -34;
						}
						else if (fDelta > 21)
						{
							fDelta = 21;
						}
						
						encoded |= std::distance(FLUT.begin(), find(FLUT.begin(), FLUT.end(), fDelta));
						prev += fDelta;
						
						if (o & 1)
						{
							encoded <<= 4;
						}
						else
						{
							output << P8SCII[encoded];
							encoded = 0;
						}
					}
					if (targSamples & 1) output << P8SCII[encoded];
					break;
				}
			}
			
			output << "\"}\n";
			
			std::cout << "Added " << compressionCode[ccode] << " sample: " << s.substr(0,s.length()-4) << std::endl <<std::endl;		
		}
		else std::cout << std::endl;
	}
	if (usedEncode)
	{
		if (usedCodes[1]) output << R"(
flut={[0]=-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21}
)";
		output << R"(
function init_pcm()

 for k,v in pairs(samples) do
  decode_sample(v)
 end

end

function decode_sample(sample)

	if(sample.c=="u8" or sample.d) return;
	local s=""
	local v=0
)";
		if (usedCodes[1]) output << R"(	if(sample.c=="fd4") then
	 s=sub(sample.s,1)
	 v=ord(sample.s,1)
	 for i=2,sample.n do
	  local b=ord(sample.s,1+lshr(i,1))
	  if band(i,1) then
 	  b=band(b,0x0f)
	  else
	   b=band(lshr(b,4),0x0f)
	  end
	  v+=flut[b]
	  s=s..chr(v)
	 end
	end
)";
		output << R"(	sample.s=s
	sample.d=true

end
)""";
	}
	output << R"(
function play_pcm(sample)

 pcm={k=sample, p=0}

end

function update_pcm()

 while(pcm and stat(108)<256) do
  for i=0,191 do
   if (pcm.p < samples[pcm.k].n) then
    pcm.p+=1
    poke(pcm_buffer+i,ord(samples[pcm.k].s,pcm.p))
   else
    poke(pcm_buffer+i,127)
   end
  end
  serial(0x808,pcm_buffer,192)
  if (pcm.p >= samples[pcm.k].n) pcm=nil
 end

end
)";
	output.close();
	std::cout << "Saved \"pcm_audio.lua\"" << std::endl;

	if (copyClip)
	{
		clip::set_text("TODO: clip support");
	}
	return 0;
}
