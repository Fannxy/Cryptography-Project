#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <ctime>
#include <time.h>
#include <random>
#include <math.h>

#include "utils.h"

using namespace std;

Word k_words[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

Word Digest[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372,
    0xa54ff53a, 0x510e527f, 0x9b05688c,
    0x1f83d9ab, 0x5be0cd19};


void padding(Byte* Message, Byte* Padded_Message, int M, int P, int N){
    /*
        Padding the input message Message to Padded_Message.
    */

    // First part: origional message
    for(int i=0; i<M; i++){
        Padded_Message[i] = Message[i];
    }

    // Second part: padded message.
    Padded_Message[M] = 0x80;
    for(int i=1; i<P; i++){
        Padded_Message[M+i] = 0x00;
    }

    //Final part.
    int l = 8*M;
    bitset<64> length = l;

    for(int i=7; i>=0; i--){
        Padded_Message[M+P+7].set(i, length[i]);
        Padded_Message[M+P+6].set(i, length[8+i]);
        Padded_Message[M+P+5].set(i, length[2*8+i]);
        Padded_Message[M+P+4].set(i, length[3*8+i]);
        Padded_Message[M+P+3].set(i, length[4*8+i]);
        Padded_Message[M+P+2].set(i, length[5*8+i]);
        Padded_Message[M+P+1].set(i, length[6*8+i]);
        Padded_Message[M+P].set(i, length[7*8+i]);
    }
}


void StringToBytes(string Mess, Byte* Message){
    int S_size = Mess.size();
    // Byte Message[S_size];
    for(int i=0; i<S_size; i++){
        Message[i] = Mess.c_str()[i];
    }
}


Word RightRotate(Word& x, int n){
    /*
        Right Rotate bits movement.
    */
    int rm = n, lm = 32-n;
    Word tmpRight, tmpLeft;

    tmpRight = x >> rm;
    tmpLeft = x << lm;
    return tmpRight | tmpLeft;
}


Word RightMove(Word& x, int n){
    /*
        Right move.
    */
    return x >> n;
}


Word Ch(Word& x, Word& y, Word& z){
    return (x & y) ^ ((~x) & z);
}


Word Maj(Word& x, Word& y, Word& z){
    return (x & y) ^ (x & z) ^ (y & z);
}


Word Sigma0(Word& x){
    return RightRotate(x, 2) ^ RightRotate(x, 13) ^ RightRotate(x, 22);
}


Word Sigma1(Word& x){
    return RightRotate(x, 6) ^ RightRotate(x, 11) ^ RightRotate(x, 25);
}


Word SigmaTrans0(Word& x){
    return RightRotate(x, 7) ^ RightRotate(x, 18) ^ RightMove(x, 3);
}


Word SigmaTrans1(Word& x){
    return RightRotate(x, 17) ^ RightRotate(x, 19) ^ RightMove(x, 10);
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


int main(int, char**){

    string mess = "abc";

    int M = mess.size();
    Byte Message[M];
    StringToBytes(mess, Message);
  
    int N = floor((M + 8)/64) + 1;
    int P = N * 64 - (M + 8);
    Byte Padded_Message[N*64]; // N*64 byte message.
    padding(Message, Padded_Message, M, P, N);

    Word MessBlock[N][16];
    for(int i=0; i<N; i++){
        for(int j=0; j<16; j++){
            MessBlock[i][j] = BytesToWord(Padded_Message[i*64+(4*j)], Padded_Message[i*64+(4*j+1)], Padded_Message[i*64+(4*j+2)], Padded_Message[i*64+(4*j+3)]);
        }
    }

    for(int i=0; i<N; i++){
        // Prepare the message schedule.
        Word MessageSchedule[64] = {};
        for(int j=0; j<16; j++){
            MessageSchedule[j] = MessBlock[i][j];
        }

        for(int j=16; j<64; j++){
            Word tmp1, tmp2, test;
            tmp1 = SigmaTrans1(MessageSchedule[j-2]);
            tmp2 = SigmaTrans0(MessageSchedule[j-15]);
            tmp1 = ModTwoAdd(tmp1, MessageSchedule[j-7]);
            tmp2 = ModTwoAdd(tmp2, MessageSchedule[j-16]);
            test = ModTwoAdd(tmp1, tmp2);
            MessageSchedule[j] = ModTwoAdd(tmp1, tmp2);
        }

        // Update working digest.
        Word a = Digest[0], b = Digest[1], c = Digest[2], d = Digest[3], e = Digest[4], f = Digest[5], g = Digest[6], h = Digest[7];
        Word T1, T2;
        // cout << "init working variable " << endl;
        // cout << "A: " << hex << a.to_ulong() << endl;
        // cout << "H: " << hex << h.to_ulong() << endl;

        Word LastHash[8] = {};
        for(int j=0; j<8; j++){
            LastHash[j] = Digest[j];
        }

        for(int t=0; t<64; t++){
            Word tmp1, tmp2;

            tmp1 = Sigma1(e);
            tmp1 = ModTwoAdd(h, tmp1);
            tmp2 = Ch(e, f, g);
            tmp2 = ModTwoAdd(tmp1, tmp2);
            tmp1 = ModTwoAdd(k_words[t], MessageSchedule[t]);
            T1 = ModTwoAdd(tmp2, tmp1);

            tmp1 = Sigma0(a);
            tmp2 = Maj(a, b, c);
            T2 = ModTwoAdd(tmp1, tmp2);

            h = g;
            g = f;
            f = e;
            e = ModTwoAdd(d, T1);
            d = c;
            c = b;
            b = a;
            a = ModTwoAdd(T1, T2);
        }

        Digest[0] = ModTwoAdd(a, LastHash[0]);
        Digest[1] = ModTwoAdd(b, LastHash[1]);
        Digest[2] = ModTwoAdd(c, LastHash[2]);
        Digest[3] = ModTwoAdd(d, LastHash[3]);
        Digest[4] = ModTwoAdd(e, LastHash[4]);
        Digest[5] = ModTwoAdd(f, LastHash[5]);
        Digest[6] = ModTwoAdd(g, LastHash[6]);
        Digest[7] = ModTwoAdd(h, LastHash[7]);
    }

    cout << "Final digest" << endl;
    for(int i=0; i<8; i++){
        cout << hex << Digest[i].to_ulong() << " ";
    }
    cout << endl;

    return 0;
}