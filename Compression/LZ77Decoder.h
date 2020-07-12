#ifndef HUFFMAN_LZ77DECODER_H
#define HUFFMAN_LZ77DECODER_H

#include <iostream>
#include <fstream>
#include <algorithm>

class LZ77Decoder {
    const int BUFFER_SIZE = 1024 * 1024;

public:

    void decodeFile(const std::string& inputFileName, const std::string& outputFileName);

private:

    void decodeBytes();

    int readNumFromBits(int& curByte, int& curBit, int bitsInNum);

private:
    std::ifstream _fin;
    std::ofstream _fout;
    char* _readBuffer;
    char* _writeBuffer;
    int _bitsPerOffset;
    int _bitsPerLength;
};

#endif //HUFFMAN_LZ77DECODER_H
