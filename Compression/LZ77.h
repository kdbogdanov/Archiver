#ifndef HUFFMAN_LZ77_H
#define HUFFMAN_LZ77_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

class LZ77 {

    const int BUFFER_SIZE = 1024 * 1024;

public:

    LZ77(int histBufSize, int prevBufSize);

    void encodeFile(const std::string& inputFileName, const std::string& outputFileName);

private:

    void encodeBytes();

    void writeNumAsBits(int num, int bitsNumber, int& writePos, int& bitsLef);

    void getNextTriple(int& offset, int& length, char& symbol, int histBegin, int prevBegin, int prevEnd);

    int* prefixFunction(int histBegin, int prevBegin, int prevEnd);

private:
    int _histBufSize;
    int _prevBufSize;
    int _bitsPerLength;
    int _bitsPerOffset;
    char* _readBuffer;
    char* _writeBuffer;
    std::ifstream _fin;
    std::ofstream _fout;
};


#endif //HUFFMAN_LZ77_H
