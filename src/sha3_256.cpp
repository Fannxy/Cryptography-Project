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

const int M = 0, r = 136, c = 64;
const int N = floor(M/r) + 1;
const int W = 8;
const int l = 6;

void padding(Byte* Message, Byte* Padded_Message, int r){
    /*
        Message: the origional message in Bytes.
        Padded_Message: the target padded message, in bytes.
        M: the length of the origional message.
        r: rate of sha-3, default to 136. len(Padded_Message) = n*r.
    */
   int j = (floor(M/r) + 1)*r;
   for(int i=0; i<M; i++){
       Padded_Message[i] = Message[i];
   }
   Padded_Message[M] = 0x06;
   for(int i=M+1; i<(j-1); i++){
       Padded_Message[i] = 0x00;
   }
   Padded_Message[j-1] = 0x80;
}


void PrintState(Byte* State){
    cout << "Print state by byte, in the State matrix order." << endl;
    for(int y=0; y<5; y++){
        for(int x=0; x<5; x++){
            for(int z=0; z<W; z++){
                cout << hex << State[W*(5*y+x)+z].to_ulong() << " ";
            }
            cout << endl;
        }
    }
}


void InitState(Byte* State){
    for(int y=0; y<5; y++){
        for(int x=0; x<5; x++){
            for(int z=0; z<W; z++){
                State[W*(5*y+x)+z] = 0x00;
            }
        }
    }
}


void CopyState(Byte* State, Byte* CopyedState){
    for(int y=0; y<5; y++){
        for(int x=0; x<5; x++){
            for(int z=0; z<W; z++){
                CopyedState[W*(5*y+x)+z] = State[W*(5*y+x)+z];
            }
        }
    }
}


Byte FetchValue(Byte* State, int x, int y, int z){
    return State[W*(5*y+x)+z];
}


void UpdateValue(Byte* State, Byte val, int x, int y, int z){
    State[W*(5*y+x)+z] = val;
}

string StateToString(Byte* State){
    string state = "";
    for(int i=0; i<5*5*W; i++){
        state += State[i].to_string();
    }
    // cout << "state: " << state << endl;
    // cout << "size: " << state.size() << endl;
    return state;
}


void Theta(Byte* State){
    Byte C[5][W];
    Byte D[5][W];
    for(int x=0; x<5; x++){
        for(int z=0; z<W; z++){
            C[x][z] = FetchValue(State, x, 0, z) ^ FetchValue(State, x, 1, z) ^ FetchValue(State, x, 2, z) ^ FetchValue(State, x, 3, z) ^ FetchValue(State, x, 4, z);
        }
    }
    for(int x=0; x<5; x++){
        for(int z=0; z<W; z++){
            D[x][z] = C[(x-1)%5][z] ^ C[(x+1)%5][(z-1)%W];
        } 
    }
    for(int x=0; x<5; x++){
        for(int y=0; y<5; y++){
            for(int z=0; z<W; z++){
                Byte val = FetchValue(State, x, y, z) ^ D[x][z];
                UpdateValue(State, val, x, y, z);
            }
        }
    }
    // cout << "Test theta" << endl;
    // PrintState(State);
}


void rho(Byte* State){
    Byte copyedState[5*5*W];
    CopyState(State, copyedState);

    int x = 1, y = 0;
    int tmp;
    for(int t=0; t<24; t++){
        for(int z=0; z<W; z++){
            Byte val = FetchValue(copyedState, x, y, (z-(t+1)*(t+2)/2)%W);
            UpdateValue(State, val, x, y, z);
        }
        tmp = y;
        y = (2*x + 3*tmp) % 5;
        x = tmp;
    }
    // cout << "Test rho" << endl;
    // PrintState(State);
}


void pai(Byte* State){
    Byte copyedState[5*5*W];
    CopyState(State, copyedState);
    for(int x=0; x<5; x++){
        for(int y=0; y<5; y++){
            for(int z=0; z<W; z++){
                Byte val = FetchValue(copyedState, (x+3*y)%5, x, z);
                UpdateValue(State, val, x, y, z);
            }
        }
    }
    // cout << "Test pai" << endl;
    // PrintState(State);
}


void Chi(Byte* State){
    Byte one = 0x01;
    Byte copyedState[5*5*W];
    CopyState(State, copyedState);
    for(int x=0; x<5; x++){
        for(int y=0; y<5; y++){
            for(int z=0; z<W; z++){
                Byte val = GFMul((FetchValue(copyedState, x, y, z) ^ FetchValue(copyedState, (x+1)%5, y, z) ^ one), FetchValue(copyedState, (x+2)%5, y, z));
                UpdateValue(State, val, x, y, z);
            }
        }
    }
    // cout << "Test Chi" << endl;
    // PrintState(State);
}

bool rc(int t){
    /*
        Round Constant.
    */
   if((t % 255) == 0) return true;
   bitset<9> R = 0x080;
//    cout << "R = " << R.to_string() << endl;
   for(int i=1; i < (t%255)+1; i++){
       R.set(0, R[0]^R[8]);
       R.set(4, R[4]^R[8]);
       R.set(5, R[5]^R[8]);
       R >> 1;
   }
   return R[1];
}

void rl(Byte* State, int ir){
    /*
        int ir: round index.
    */
    Byte copyedState[5*5*W];
    CopyState(State, copyedState);
    bitset<8*8> RC = 0x00000000;
    for(int j=0; j<l; j++){
        RC.set((int) pow(2, j)-1, rc(j+7*ir));
    }
    Byte RCB[8];
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            RCB[i].set(j, RC[i*8+j]);
        }
    }
    for(int z=0; z<W; z++){
        Byte val = FetchValue(State, 0, 0, z) ^ RCB[z];
        UpdateValue(State, val, 0, 0, z);
    }
    // cout << "Test rl" << endl;
    // PrintState(State);
}

void keccakRnd(Byte* State, int ni){
    Theta(State);
    rho(State);
    pai(State);
    Chi(State);
    rl(State, ni);
}

void keccakF(Byte* State, int nr){
    int ir = 12 +  2*l - nr;
    for(ir; ir<12+2*l; ir++){
        keccakRnd(State, ir);
    }
}


int main(int, char**){
    // int M = 0, r = 136, c = 64;
    // int N = floor(M/r) + 1;
    // const int W = 8;
    int nr = 12 + 2*l;
    int d = 256;

    Byte Message[M];
    Byte Padded_Message[N*r];

    Byte State[5*5*W];
    padding(Message, Padded_Message, r);


    InitState(State);

    // sponge stage.
    for(int t=0; t<N; t++){
        // the beginning message block for this round.
        Byte roundMessage[r+c];
        for(int i=0; i<r; i++){
            roundMessage[i] = Padded_Message[t*N+i];
        }
        for(int i=r; i<c; i++){
            roundMessage[i] = 0x00;
        }

        // XOR the input message with the State.
        for(int i=0; i<5*5*W; i++){
            State[i] = State[i] ^ roundMessage[i];
        }
        keccakF(State, nr);
    }

    // generate stage.
    string Z = "";
    while(Z.size() < d){
        string state = StateToString(State);
        state = state.substr(0, r*8);
        Z = Z + state;
        keccakF(State, nr);
    }

    cout << "Final result: " << Z.substr(0, d) << endl;

    return 0;
}