#include <iostream>
#include <cstdint>
#include <string>
#include <iomanip>
#include <sstream>
#include <initializer_list>
#include <vector>
#include <array>
#include <algorithm>
using namespace std;

class ripemd160 {
public:
    string hash(string x) {
        return bit512(x);
    }
private:
    uint32_t T,h0 = 0x67452301,h1 = 0xEFCDAB89,h2 = 0x98BADCFE,h3 = 0x10325476,h4 = 0xC3D2E1F0;
    vector<uint32_t>rr = {
        5,14,7,0,9,2,11,4,13,6,15,8,1,10,3,12,
        6,11,3,7,0,13,5,10,14,15,8,12,4,9,1,2,
        15,5,1,3,7,14,6,9,11,8,12,2,10,0,4,13,
        8,6,4,1,3,11,15,0,5,12,2,13,9,7,10,14,
        12,15,10,4,1,5,8,7,6,2,13,14,0,3,9,11
    };

    vector<uint32_t>rl = {
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
        7,4,13,1,10,6,15,3,12,0,9,5,2,14,11,8,
        3,10,14,4,9,15,8,1,2,7,0,6,13,11,5,12,
        1,9,11,10,0,8,12,4,13,3,7,15,14,5,6,2,
        4,0,5,9,7,12,2,10,14,1,3,8,11,6,15,13
        //    5,14,7,0,9,2,11,4,13,6,15,8,1,10,3,12
    };

    vector<uint32_t>sl = {
        11,14,15,12,5,8,7,9,11,13,14,15,6,7,9,8,
        7,6,8,13,11,9,7,15,7,12,15,9,11,7,13,12,
        11,13,6,7,14,9,13,15,14,8,13,6,5,12,7,5,
        11,12,14,15,14,15,9,8,9,14,5,6,8,6,5,12,
        9,15,5,11,6,8,13,12,5,12,13,14,11,8,5,6
    };

    vector<uint32_t>sr = {
        8,9,9,11,13,15,15,5,7,7,8,11,14,14,12,6,
        9,13,15,7,12,8,9,11,7,7,12,7,6,15,13,11,
        9,7,15,11,8,6,6,14,12,13,5,14,13,13,7,5,
        15,5,8,11,14,14,6,14,6,9,12,9,12,5,15,8,
        8,5,12,9,12,5,14,6,8,13,6,5,15,13,11,11
    };

    vector<int>k = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
    };

    vector<uint32_t>kl = {
        0x0,
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xA953FD4E
    };

    vector<uint32_t>kr = {
        0x50A28BE6,
        0x5C4DD124,
        0x6D703EF3,
        0x7A6D76E9,
        0x0
    };

    uint32_t f(int j,uint32_t b,uint32_t c,uint32_t d) {
        if (0 <= j and j <= 15) {
            return b ^ c ^ d;
        }
        else if (16 <= j and j <= 31) {
            return (b & c) | (~b & d);
        }
        else if (32 <= j and j <= 47) {
            return (b | ~c) ^ d;
        }
        else if (48 <= j and j <= 63) {
            return (b & d) | (c & ~d);
        }
        else {
            return b ^ (c | ~d);
        }
    }

    uint32_t rol(int s,uint32_t x) {
        return x << s | x >> (32 - s);
    }

    uint32_t bit2(const string& x) {
        uint32_t hasil = 0;
        for (int i = (int)x.size() - 1; i >= 0; i--) {
            hasil = (hasil << 1) | (x[i] == '1');
        }
        return hasil;
    }

    string biner(int x) {
        string hasil;
        for(int i = 0; i < 8; i++) {
            if (x >> i & 1) hasil += "1";
            else hasil += "0";
        }
        return hasil;
    }

    string bit512(string x) {
        string x_bit;
        for(int i = 0; i < x.size(); i++) {
            x_bit += biner((int)x[i]);
        }
       // int ukuran = x.size() * 8;
        x_bit += "1";
        while(x_bit.size() % 512 != 448) {
            x_bit += "0";
        }
        uint64_t ukuran = (uint64_t)x.size() * 8;
        for (int i = 0; i < 8; i++) {
            x_bit += biner((uint8_t)((ukuran >> (8 * i)) & 0xFF));
        }
        mainloop(x_bit);
        return x_bit;
    }

    void mainloop(string& x_bit) {
        vector<uint32_t> x;
        for (int i = 0; i < 16; i++) {
            string y;
            for(int j = (i+1) * 32 - 1; j >= i * 32; j--) y += x_bit[j];
            x.push_back(bit2(y));
        }
        uint32_t al = h0, bl = h1, cl = h2;
        uint32_t dl = h3, el = h4;
        uint32_t ar = h0, br = h1, cr = h2;
        uint32_t dr = h3, er = h4;

        uint32_t Tl,Tr;
        for(int i = 0; i < 80; i++) {
            Tl = rol(sl[i],(al + f(i,bl,cl,dl) + x[rl[i]] + kl[k[i]])) + el;
            al = el,el = dl, dl = rol(10,cl),cl = bl, bl = Tl;
            Tr = rol(sr[i],(ar + f(79 - i,br,cr,dr) + x[rr[i]] + kr[k[i]])) + er;
            ar = er,er = dr, dr = rol(10,cr),cr = br, br = Tr;
        }
        T = h1 + cl + dr;
        h1 = h2 + dl + er;
        h2 = h3 + el + ar;
        h3 = h4 + al + br;
        h4 = h0 + bl + cr;
        h0 = T;

        x_bit.clear();
        stringstream ss;
        for (uint32_t h : {
                    h0, h1, h2, h3, h4
                }) {
            for (int j = 0; j < 4; j++) {
                ss << hex << setfill('0') << setw(2)
                   << ((h >> (8 * j)) & 0xFF);
            }
        }
        x_bit = ss.str();
    }
};

void solve() {
    string x;
    getline(cin,x);
    ripemd160 y;
    x = y.hash(x);
    cout << x << endl;
}

int main() {
    solve();
    return 0;
}
    