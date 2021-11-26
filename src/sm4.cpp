#include <bitset>
#include <string>
#include <ctime>
#include <time.h>
#include <random>

#include "utils.h"

using namespace std;

// #define DEBUG

const int Nr = 32;
const int Nk = 4;
const int Nl = 16;
const int Nf = 8192;

Word FK[4] = {
    0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc
};

Word CK[32] = {
    0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269, 
    0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9, 
    0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
    0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
	0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
	0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
	0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
	0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
};

Byte SBox[32][32] = {
	{0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7,
	0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05}, 
	{0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3,
	0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99},
	{0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a,
	0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62},
	{0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95,
	0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6},
	{0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba,
	0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8},
	{0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b,
	0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35},
	{0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2,
	0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87},
	{0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52,
	0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e},
	{0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5,
	0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1},
	{0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55,
	0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3},
	{0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60,
	0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f},
	{0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f,
	0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51},
	{0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f,
	0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8},
	{0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd,
	0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0},
	{0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e,
	0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84},
	{0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20,
	0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48},
};

void KeyExpansion(Word *Key, Word *Keywords);
Word KeyTrans(Word w);
Word CipherTrans(Word w);
Word FFunc(Word& w1, Word& w2, Word& w3, Word& w4, Word& Rkey);
void Encryption(Word* Message, Word* Keywords, Word* CipherText);


void KeyExpansion(Word *Key, Word *Keywords){
    // for the first four elements.
    Word TmpKeys[36] = {};
    for(int i=0; i<4; i++){
        TmpKeys[i] = Key[i] ^ FK[i];
    }

    // for the last 28 keywords.
    for(int i=0; i<32; i++){

        TmpKeys[i+4] = TmpKeys[i] ^ KeyTrans(TmpKeys[i+1]^TmpKeys[i+2]^TmpKeys[i+3]^CK[i]);
        Keywords[i] = TmpKeys[i+4];
    }
}


Word KeyTrans(Word w){
    /*
        Transformation of keywords w.
    */
   Byte A[4] = {};
   Word B, tmpLeft, tmpRight, tmp1, tmp2;
   WordToBytes(w, A);
   int row, col;
   for(int i=0; i<Nk; i++){
       row = A[i][7]*8 + A[i][6]*4 + A[i][5]*2 + A[i][4];
       col = A[i][3]*8 + A[i][2]*4 + A[i][1]*2 + A[i][0];
       A[i] = SBox[row][col];
   }

   B = BytesToWord(A[0], A[1], A[2], A[3]);
   tmpLeft = B << 13;
   tmpRight = B >> 19;
   tmp1 = tmpLeft | tmpRight;

   tmpLeft = B << 23;
   tmpRight = B >> 9;
   tmp2 = tmpLeft | tmpRight;

   B ^= (tmp1 ^ tmp2);
   return B;
}


Word CipherTrans(Word w){
    /*
        Trans functon for encryption.
    */
    Byte A[4] = {};
    Word B, tmpLeft, tmpRight, tmp1, tmp2, tmp3, tmp4;
    WordToBytes(w, A);
    int row, col;
    for(int i=0; i<Nk; i++){
        row = A[i][7]*8 + A[i][6]*4 + A[i][5]*2 + A[i][4];
        col = A[i][3]*8 + A[i][2]*4 + A[i][1]*2 + A[i][0];
        A[i] = SBox[row][col];
    }

    B = BytesToWord(A[0], A[1], A[2], A[3]);

    tmpLeft = B << 2;
    tmpRight = B >> 30;
    tmp1 = tmpLeft | tmpRight;

    tmpLeft = B << 10;
    tmpRight = B >> 22;
    tmp2 = tmpLeft | tmpRight;

    tmpLeft = B << 18;
    tmpRight = B >> 14;
    tmp3 = tmpLeft | tmpRight;

    tmpLeft = B << 24;
    tmpRight = B >> 8;
    tmp4 = tmpLeft | tmpRight;

    B ^= (tmp1 ^ tmp2 ^ tmp3 ^ tmp4);

    return B;
}


Word FFunc(Word& w1, Word& w2, Word& w3, Word& w4, Word& Rkey){
    /*
        The Round F-Function for encryption.
    */
   return w1 ^ CipherTrans(w2 ^ w3 ^ w4 ^ Rkey);
}


void Encryption(Word* Message, Word* Keywords, Word* CipherText){
    /*
        Encryption of sm4.    
    */
   Word TmpMessage[36] = {};
   for(int i=0; i<4; i++){
       TmpMessage[i] = Message[i];
   }
   for(int i=0; i<32; i++){
       TmpMessage[i+4] = FFunc(TmpMessage[i], TmpMessage[i+1], TmpMessage[i+2], TmpMessage[i+3], Keywords[i]);
   }
   // Last round inverse.
   for(int i=0; i<4; i++){
       CipherText[i] = TmpMessage[35-i];
   }
}

void Decryption(Word* CipherText, Word* Keywords, Word* Message){
    /*
        Decryption of sm4.    
    */
    Word TmpMessage[36] = {};
    for(int i=0; i<4; i++){
        TmpMessage[i] = CipherText[i];
    }
    for(int i=0; i<32; i++){
        TmpMessage[i+4] = FFunc(TmpMessage[i], TmpMessage[i+1], TmpMessage[i+2], TmpMessage[i+3], Keywords[31-i]);
    }

    // Last round inverse
    for(int i=0; i<4; i++){
        Message[i] = TmpMessage[35-i];
    }
}


void TEST_SM4(){
    Word Key[Nk] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};
    Word Messages[Nk] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};
    Word Keywords[Nr] = {};
    Word CipherText[Nk] = {};
    // key expansion.
    // GenerateRandomWords(Key, Nk); // get the initial 4 words key.
    KeyExpansion(Key, Keywords); // Generate the round keys

    Encryption(Messages, Keywords, CipherText);
    cout << "Cipher Text: " << endl;
    for(int i=0; i<Nk; i++){
        cout <<  hex << CipherText[i].to_ulong() << " ";
    }
    cout << endl;

    Decryption(CipherText, Keywords, Messages);
    cout << "Decrypted Text: " << endl;
    for(int i=0; i<Nk; i++){
        cout << hex << Messages[i].to_ulong() << " ";
    }
    cout << endl;
}

void SM4_CBC(){
    /*
        SM4 in CBC mode.
    */
    Word Key[Nk] = {};
    Word Messages[Nk] = {};
    Word Keywords[Nr] = {};
    Word CipherText[Nk] = {};
    Word InitialVector[Nk] = {};
    Word IVInUse[Nk] = {};

    // Generate key
    GenerateRandomWords(Key, Nk);
    KeyExpansion(Key, Keywords);

    // Generate initial vector
    GenerateRandomWords(InitialVector, Nk);
    for(int i=0; i<Nk; i++){
        IVInUse[i] = InitialVector[i];
    }

    // CBC Mode
    float time_e = 0.0;
    float time_d = 0.0;
    for(int i=0; i<Nf; i++){
        GenerateRandomWords(Messages, Nk);
        for(int j=0; j<Nk; j++){
            Messages[j] ^= IVInUse[j];
        }

        clock_t encryption_begin = clock();
        Encryption(Messages, Keywords, CipherText);
        clock_t encryption_end = clock();

        clock_t decryption_begin = clock();
        Decryption(CipherText, Keywords, Messages);
        clock_t decryption_end = clock();

        time_e += (encryption_end - encryption_begin) * 1.0 / CLOCKS_PER_SEC;
        time_d += (decryption_end - decryption_begin) * 1.0 / CLOCKS_PER_SEC;
    }
    time_e = time_e / Nf;
    time_d = time_d / Nf;
    cout << "Efficiency of encryption: " << ((128)/time_e)*1e-3 << "Mbps" << endl;
    cout << "Efficiency of decryption: " << ((128)/time_d)*1e-3 << "Mbps" << endl;
}

int main(int, char**){
    
    #ifdef DEBUG
        TEST_SM4();
    #endif

    #ifndef DEBUG
        SM4_CBC();
    #endif

    return 0;
}