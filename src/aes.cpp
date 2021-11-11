#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <ctime>
#include <time.h>
#include <random>

#include "utils.h"


using namespace std;

const int Nr = 10; // Number of rounds.
const int Nk = 4; // Number of key word length.
const int Nl = 16; // Number of byte length.
const int Nf = 8192; // Length of random files, with unit byte.
// const int Nf = 2;

Byte SBox[16][16] = {
    {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 
	 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76},
	{0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 
	 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0},
	{0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 
	 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15},
	{0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 
	 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75},
	{0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 
	 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84},
	{0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 
	 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF},
	{0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 
	 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8},
	{0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 
	 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2},
	{0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 
	 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73},
	{0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 
	 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB},
	{0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 
	 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79},
	{0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 
	 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08},
	{0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 
	 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A},
	{0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 
	 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E},
	{0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 
	 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF},
	{0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 
	 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16}
};

Byte Inv_SBox[16][16] = {
    {0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 
	 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB},
	{0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 
	 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB},
	{0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 
	 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E},
	{0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 
	 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25},
	{0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 
	 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92},
	{0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 
	 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84},
	{0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 
	 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06},
	{0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 
	 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B},
	{0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 
	 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73},
	{0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 
	 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E},
	{0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 
	 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B},
	{0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 
	 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4},
	{0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 
	 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F},
	{0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 
	 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF},
	{0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 
	 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61},
	{0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 
	 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D}
};

Byte ColMix[4][4] = {
    {0x02, 0x03, 1, 1},
    {1, 0x02, 0x03, 1},
    {1, 1, 0x02, 0x03},
    {0x03, 1, 1, 0x02},
};

Byte InvColMix[4][4] = {
    {0x0e, 0x0b, 0x0d, 0x09},
    {0x09, 0x0e, 0x0b, 0x0d},
    {0x0d, 0x09, 0x0e, 0x0b},
    {0x0b, 0x0d, 0x09, 0x0e},
};

Word Rcon[10] = {0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 
				 0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000};


// declaration
void TransToState(Byte* Arr, Byte (*State)[Nk]);
Word RotWord(Word w);
Word SubWord(Word w);
Byte SubByte(Byte b);
void KeyExpansion(Byte *key, Word *keywords);
void PrintState(Byte (*State)[Nk]);


void TransToState(Byte* Arr, Byte (*State)[Nk]){
    /*
        Transfrom the origional random messgae to 4*4 State.
    */
    for(int i=0; i<Nk; i++){
        for(int j=0; j<Nk; j++){
            State[j][i] = Arr[i*4+j];
        }
    }
}


void StateTransback(Byte (*State)[Nk], Byte* Arr){
    /*
        Trans the 2d State to 1d Message.
    */
    int i = 0;
    for(int j=0; j<Nk; j++){
        for(int k=0; k<Nk; k++){
            Arr[i] = State[k][j];
            i += 1;
        }
    }
}


Word RotWord(Word w){
    Word tempLeft, tempRight, temp;
    tempLeft = w << 8;
    tempRight = w >> 24;
    temp = tempLeft | tempRight;
    // cout << "RotWord: " << hex << temp.to_ulong() << endl;
    return temp;
}


Word SubWord(Word w){
    Byte bytes[4] = {};
    Word temp;
    WordToBytes(w, bytes);
    for(int i=0; i < 4; i++){
        bytes[i] = SubByte(bytes[i]);
    }
    temp = BytesToWord(bytes[0], bytes[1], bytes[2], bytes[3]);

    return BytesToWord(bytes[0], bytes[1], bytes[2], bytes[3]);
}


Byte SubByte(Byte b){
    /*
        Using S_Box to substitue the byte.
    */
    int row, col;
    row = b[7]*8 + b[6]*4 + b[5]*2 + b[4];
    col = b[3]*8 + b[2]*4 + b[1]*2 + b[0];

    return SBox[row][col];
}


Byte InvSubByte(Byte b){
    /*
        Using S_Box to substitue the byte.
    */
    int row, col;
    row = b[7]*8 + b[6]*4 + b[5]*2 + b[4];
    col = b[3]*8 + b[2]*4 + b[1]*2 + b[0];

    return Inv_SBox[row][col];
}


void SubState(Byte (*State)[Nk]){
    /*
        Using S_Box to substitue all the bytes in State.
    */
   for(int i=0; i<Nk; i++){
       for(int j=0; j<Nk; j++){
           State[i][j] = SubByte(State[i][j]);
       }
   }
}


void InvSubState(Byte (*State)[Nk]){
    /*
        Using S_Box to substitue all the bytes in State.
    */
   for(int i=0; i<Nk; i++){
       for(int j=0; j<Nk; j++){
           State[i][j] = InvSubByte(State[i][j]);
       }
   }
}


void RowBuffer(Byte* row, int n, Byte* buffer){
    for(int i=0; i<n; i++){
        buffer[i] = row[i];
    }
}


void ShiftRows(Byte (*State)[Nk]){
    /*
        Shift rows.
    */
    Byte temprow[Nk] = {};

    // shift one element in the second row.
    RowBuffer(State[1], Nk, temprow);
    for(int i=0; i<Nk; i++){
        State[1][i] = temprow[(i+1) % Nk];
    }

    // shift two elements in the third row.
    RowBuffer(State[2], Nk, temprow);
    for(int i=0; i<Nk; i++){
        State[2][i] = temprow[(i+2) % Nk];
    }

    // shift three elements in the third row.
    RowBuffer(State[3], Nk, temprow);
    for(int i=0; i<Nk; i++){
        State[3][i] = temprow[(i+3) % Nk];
    }
}


void InvShiftRows(Byte (*State)[Nk]){
    /*
        Inv Shift rows.
    */
    Byte temprow[Nk] = {};

    // shift one element in the second row.
    RowBuffer(State[1], Nk, temprow);
    for(int i=0; i<Nk; i++){
        State[1][i] = temprow[(i+3) % Nk];
    }

    // shift two elements in the third row.
    RowBuffer(State[2], Nk, temprow);
    for(int i=0; i<Nk; i++){
        State[2][i] = temprow[(i+2) % Nk];
    }

    // shift three elements in the third row.
    RowBuffer(State[3], Nk, temprow);
    for(int i=0; i<Nk; i++){
        State[3][i] = temprow[(i+1) % Nk];
    }

}


void MixColumns(Byte (*State)[Nk]){
    Byte temp;
    Byte arr[Nk] = {};
    for(int k=0; k<Nk; k++){
        for(int col=0; col<Nk; col++){
            arr[col] = State[col][k];
        }
        for(int i=0; i<Nk; i++){
            temp = 0;
            for(int j=0; j<Nk; j++){
                temp  = temp ^ GFMul(ColMix[i][j], arr[j]);
            }
            State[i][k] = temp;
        }
    }
}


void InvMixColumns(Byte (*State)[Nk]){
    Byte temp;
    Byte arr[Nk] = {};
    for(int k=0; k<Nk; k++){
        for(int col=0; col<Nk; col++){
            arr[col] = State[col][k];
        }
        for(int i=0; i<Nk; i++){
            Byte temp = 0;
            for(int j=0; j<Nk; j++){
                temp  = temp ^ GFMul(InvColMix[i][j], arr[j]);
            }
            State[i][k] = temp;
        }
    }
}


void PrintState(Byte (*State)[Nk]){

    for(int i=0; i<Nk; i++){
        for(int j=0; j<Nk; j++){
            cout << hex << State[i][j].to_ulong() << " ";
        }
        cout << endl;
    }

}


void KeyExpansion(Byte *key, Word *keywords){
    Word temp;
    int i = 0;
    

    for(i=0; i<Nk; i++){
        keywords[i] = BytesToWord(key[4*i], key[4*i+1], key[4*i+2], key[4*i+3]);
    }
    for(i = Nk; i<4*(Nr+1); i++){
        temp = keywords[i-1];
        if(i % 4 == 0){
            keywords[i] = keywords[i-Nk] ^ SubWord(RotWord(temp)) ^ Rcon[i/4-1];
        }else{
            keywords[i] = keywords[i-Nk] ^ temp;
        }
    }
}




void AddRoundKey(Byte (*State)[Nk], Word* key){
    /*
        Message: 16 bytes array.
        key: 4 word array (4*4 = 16 bytes).
    */
    Byte KeyBytes[4][4] = {};
    for(int i=0; i<4; i++){
        WordToBytes(key[i], KeyBytes[i]);
    }

    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            // cout << "ori: " << hex << State[i][j].to_ulong() << " ";
            State[i][j] = State[i][j] ^ KeyBytes[j][i];
        }
    }
}


void encrypt(Byte (*State)[Nk], Word* keywords){

    Word key[4];
    for(int i=0; i<Nk; i++){
        key[i] = keywords[i];
    }
    AddRoundKey(State, key); // update state

    // Nr rounnds encryption
    for(int i=1; i<Nr; i++){
        SubState(State);
        ShiftRows(State);
        MixColumns(State);

        for(int j=0; j<Nk; j++){
            key[j] = keywords[4*i+j];
        }
        AddRoundKey(State, key);
    }

    // last round.
    SubState(State);
    ShiftRows(State);

    for(int j=0; j<Nk; j++){
        key[j] = keywords[4*Nr+j];
    }
    AddRoundKey(State, key);
}


void decrypt(Byte (*State)[Nk], Word* keywords){

    Word key[4];
    for(int i=0; i<Nk; i++){
        key[i] = keywords[4*Nr+i];
    }
    AddRoundKey(State, key); // update state

    // Nr rounnds encryption
    for(int i=Nr-1; i>0; i--){
        InvShiftRows(State);
        InvSubState(State);
        // InvMixColumns(State);

        for(int j=0; j<Nk; j++){
            key[j] = keywords[4*i+j];
        }
        AddRoundKey(State, key);
        InvMixColumns(State);
    }

    // last round.
    InvShiftRows(State);
    InvSubState(State);
    for(int j=0; j<Nk; j++){
        key[j] = keywords[j];
    }

    AddRoundKey(State, key);
}


void CBC_AES(){

    // Byte Message[Nf] = {};
    Byte Key[Nl] = {};
    Byte DataBlock[Nl] = {};
    Byte InitialVector[Nl] = {};
    Byte IVInUse[Nl] = {};

    ofstream outfile;
    ifstream inflie;
    char buffer[129];
    

    Byte State[Nk][Nk] = {};
    Word KeyWords[4*(Nr+1)] = {};

    // Generate origional key.
    GenerateRandomBytes(Key, Nl);
    KeyExpansion(Key, KeyWords);

    // Generate initial vector.
    GenerateRandomBytes(InitialVector, Nl);
    for(int i=0; i<Nl; i++){
        IVInUse[i] = InitialVector[i];
    }

    // Generare the test file.
    outfile.open("./test/Message.txt");
    if(outfile.is_open()){
        for(int i=0; i<Nf; i++){
            GenerateRandomBytes(DataBlock, Nl);
            // cout << "Generation" << endl;
            for(int j=0; j<Nl; j++){
                outfile << DataBlock[j];
                // cout << hex << DataBlock[j].to_ulong() << endl;
            }
            outfile << endl;
        }
        outfile.close();
    }else{
        cout << "Create File Error" << endl;
    }

    // CBC Mode Encryption.
    outfile.open("./test/Cipher.txt");
    inflie.open("./test/Message.txt");
    if(!inflie.is_open()){
        cout << "Load File Error" << endl;
    }

    for(int i=0; i<Nf; i++){

        // load file.
        inflie.getline(buffer, 129);

        // trans to bytes.
        CharToByte(DataBlock, buffer);

        // xor with iv.
        for(int j=0; j<16; j++){
            DataBlock[j] ^= IVInUse[j];
        }

        // AES Encryption.
        TransToState(DataBlock, State);
        encrypt(State, KeyWords);

        // Update iv
        StateTransback(State, DataBlock);
        
        // // test datablock ???
        // for(int j=0; j<16; j++){
        //     cout << hex << DataBlock[j].to_ulong() << " ";
        // }
        // cout << endl;

        for(int j=0; j<16; j++){
            IVInUse[j] = DataBlock[j];
        }

        // Save the cipher-test
        if(outfile.is_open()){
            for(int j=0; j<Nl; j++){
                outfile << DataBlock[j];
            }
            outfile << endl;
        }else{
            cout << "Save cipher-test Error" << endl;
        }
    }
    outfile.close();
    inflie.close();


    // CBC Mode Decryption.
    outfile.open("./test/Decryption.txt");
    inflie.open("./test/Cipher.txt");
    if(!inflie.is_open()){
        cout << "Load File Error" << endl;
    }

    for(int i=0; i<Nf; i++){

        // flush IV.
        for(int j=0; j<Nl; j++){
            IVInUse[j] = InitialVector[j];
        }

        // load file.
        inflie.getline(buffer, 129);

        // trans to bytes.
        CharToByte(DataBlock, buffer); // Now store the cipher-text.

        // Save the IV for next round.
        for(int j=0; j<Nl; j++){
            InitialVector[j] = DataBlock[j];
        }

        // AES Decryption.
        TransToState(DataBlock, State);
        decrypt(State, KeyWords);

        StateTransback(State, DataBlock);

        // xor with IV.
        for(int j=0; j<16; j++){
            DataBlock[j] ^= IVInUse[j];
        }

        // Save the decrypted info.
        if(outfile.is_open()){
            for(int j=0; j<Nl; j++){
                outfile << DataBlock[j];
            }
            outfile << endl;
        }else{
            cout << "Save decryption Error" << endl;
        }
    }
    outfile.close();
    inflie.close();

}

int main(int, char**){

    CBC_AES();
    return 0;
}