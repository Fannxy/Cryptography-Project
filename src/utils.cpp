#include "utils.h"
using namespace std;

Word BytesToWord(Byte& k1, Byte& k2, Byte& k3, Byte& k4){
    Word w;
    for(int j=0; j<8; j++){
        w.set(8*3+j, k1[j]);
        w.set(8*2+j, k2[j]);
        w.set(8*1+j, k3[j]);
        w.set(j, k4[j]);
    }
    // cout << "here words: " << w.to_string() << endl;
    return w;
}


void WordToBytes(Word w, Byte* ByteS){
    Byte tmp;
    // cout << "check word2bytes" << endl;
    // cout << w.to_string() << endl;

    int keynum = 3;
    for(int i=0; i<8; i++){
        ByteS[0].set(i, w[8*3+i]);
        ByteS[1].set(i, w[8*2+i]);
        ByteS[2].set(i, w[8*1+i]);
        ByteS[3].set(i, w[i]);
    }
}

void GenerateRandomBytes(Byte *Arr, int n){
    /*
        Used to generate random byte array.
    */
    // srand((unsigned)time(NULL));
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // cout << "seed: " << (time_t)ts.tv_nsec << endl;
    srand((time_t)ts.tv_nsec);

    default_random_engine generator;
    uniform_real_distribution<float> distribution(0.0, 1.0);
    float randNum;
    bool randBit;
    for(int i=0; i<n; i++){
        for(int j=0; j<8; j++){
            randNum = distribution(generator);
            randBit = (randNum < 0.5) ? 0 : 1;
            Arr[i].set(j, randBit);
        }
    }
}

void GenerateRandomWords(Word *Arr, int n){
    /*
        Used to generate random words array, containing n words.
    */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // cout << "seed: " << (time_t)ts.tv_nsec << endl;
    srand((time_t)ts.tv_nsec);

    default_random_engine generator;
    uniform_real_distribution<float> distribution(0.0, 1.0);
    float randNum;
    bool randBit;
    for(int i=0; i<n; i++){
        for(int j=0; j<32; j++){
            randNum = distribution(generator);
            randBit = (randNum < 0.5) ? 0 : 1;
            Arr[i].set(j, randBit);
        }
    }
}


void CharToByte(Byte* Block, char* buffer){
    bool val;
    for(int i=0; i<16; i++){
        for(int j=0; j<8; j++){
            val = (buffer[i*8+j] == '1') ? 1 : 0;
            // cout << val << " ";
            Block[i].set(7-j, val);
        }
    }
}


Byte GFMul(Byte a, Byte b) { 
    /*
        The multiplication of byte.
    */
	Byte p = 0;
	Byte hi_bit_set;
	for (int counter = 0; counter < 8; counter++) {
		if ((b & Byte(1)) != 0) {
			p ^= a;
		}
		hi_bit_set = (Byte) (a & Byte(0x80));
		a <<= 1;
		if (hi_bit_set != 0) {
			a ^= 0x1b; /* x^8 + x^4 + x^3 + x + 1 */
		}
		b >>= 1;
	}
	return p;
}


Word ModTwoAdd(Word x, Word y){
    /*
        Mod 2^n addition.
    */
    Word res = x;
    while(y.any()){
        Word t = x;
        x = x^y;
        y = (t & y) <<  1;
        res = x;
    }

    return res;
}


string ReadFileToString(string path){
    /*
        Load file's coonstant to string.
    */
   ifstream input_file(path);
   if(!input_file.is_open()){
       cerr << "load fiile error" << endl;
       exit(EXIT_FAILURE);
   }
   return string((istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>());
}

void StringToBytes(string Mess, Byte* Message){
    int S_size = Mess.size();
    // Byte Message[S_size];
    for(int i=0; i<S_size; i++){
        Message[i] = Mess.c_str()[i];
    }
}