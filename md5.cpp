#include "md5.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>
#include <sstream>

static void normalize_message(std::vector<unsigned char> &message);
static void block_process(std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end);
static void round_process(unsigned int &a, unsigned int &b, unsigned &c, unsigned int &d, int i, std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end);
static unsigned int left_rotate(unsigned int num, unsigned int shift);
static unsigned int endian_convert(unsigned int num);

static unsigned int r[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

static unsigned int a = 0x67452301;
static unsigned int b = 0xefcdab89;
static unsigned int c = 0x98badcfe;
static unsigned int d = 0x10325476;

struct K {
    constexpr K() : k() {
        for(int i = 0; i < 64; i++){
            k[i] = floor(fabs(sin(i + 1.0)) * pow(2, 32));
        }
    }
    unsigned int k[64];
} k;

void normalize_message(std::vector<unsigned char> &message) {
    // the end of string '\0' doesn't include in message size
    uint64_t bit_count = (message.size()) * 8;
    uint64_t resize_bit_count = (((bit_count + 64) / 512)) * 512 + 448;
    message.resize(message.size() + 1, 128);
    message.resize(resize_bit_count / 8, '\0');
    
    for(int i = 1;i <= 8; i++){
        unsigned int byte_data = (bit_count % (1 << (8 * i)) >> 8 * (i - 1));
        message.resize(resize_bit_count / 8 + i, byte_data);
        bit_count -= byte_data;
    }
}

void block_process(std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end) {
        unsigned int temp_a = a;
        unsigned int temp_b = b;
        unsigned int temp_c = c;
        unsigned int temp_d = d;

        for(int i = 0; i < 64; i++){
            round_process(temp_a, temp_b, temp_c, temp_d, i, begin, end);
        }

        a += temp_a;
        b += temp_b;
        c += temp_c;
        d += temp_d;
}

static void round_process(unsigned int &a, unsigned int &b, unsigned &c, unsigned int &d, int i, std::vector<unsigned char>::iterator begin, std::vector<unsigned char>::iterator end) {
    unsigned int f;
    unsigned int g;
    if (i >= 0 && i < 16) {
        f = (b & c) | (~b & d);
        g = i % 16;
    } else if (i >= 16 && i < 32) {
        f = (d & b) | (~d & c);
        g = (5 * i + 1) % 16;
    } else if (i >= 32 && i < 48) {
        f = b ^ c ^ d;
        g = (3 * i + 5)% 16;
    } else if (i >= 48 && i < 64) {
        f = c ^ (b | ~d);
        g = (7 * i) % 16;
    }
    unsigned int chunck = 0;
    for (int j = 0; j < 4; j++) {
        chunck += *(begin + g * 4 + j) << (j * 8);
    }

    unsigned int temp = d;
    d = c;
    c = b;
    b = left_rotate(a + f + chunck + k.k[i], r[i]) + b;
    a = temp;
}

unsigned int left_rotate(unsigned int num, unsigned int shift){
    return num << shift | num >> (32 - shift);
}

static unsigned int endian_convert(unsigned int num) {
    unsigned int result = 0;
    result |= (num & 0x000000ff) << 24;
    result |= (num & 0x0000ff00) << 8;
    result |= (num & 0x00ff0000) >> 8;
    result |= (num & 0xff000000) >> 24;
    return result;
}

std::string md5(std::string &origin_message){
    std::vector<unsigned char> message;
    for (unsigned char c: origin_message){
        message.push_back(c);
    }
    
    normalize_message(message);

    for(int i = 0;i < message.size() / 64;i++) {
        block_process(message.begin() + i * 64, message.begin() + (i + 1) * 64);
    }

    a = endian_convert(a);
    b = endian_convert(b);
    c = endian_convert(c);
    d = endian_convert(d);

    std::stringstream ss;
    ss << std::hex << a << b << c << d << std::endl;
    std::string result;
    ss >> result;
    return result;
}