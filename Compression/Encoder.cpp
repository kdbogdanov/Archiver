#include "Encoder.h"

using namespace std;

void Encoder::setCode(string code, unsigned char byte)
{
    _codes[byte] = code;
}

void Encoder::encodeFile(const string& inputFileName, const string& outputFileName)
{
    _fin.open(inputFileName, ios::binary | ios::ate);
    _fout.open(outputFileName, ios::binary | ios::app); //append info, because tree is already stored
    if (_fin.is_open() && _fout.is_open())
        encodeBytes();
    _fin.close();
    _fout.close();
}

void Encoder::encodeBytes()
{
    long long bytesLeft = _fin.tellg();
    long long readBufSize = 1024 * 1024;
    long long encBufSize = readBufSize;
    int readPos = 0;
    int encPos = 0;
    int codePos = 0;
    int bitsLeft = CHAR_BIT;
    char* readBuf = new char[readBufSize];
    char* encBuf = new char[encBufSize];

    _fout << bytesLeft << 'S';  //store file size and separating symbol
    _fin.seekg(0, ios::beg);

    while (bytesLeft > 0)
    {
        readBufSize = (bytesLeft < readBufSize) ? bytesLeft : readBufSize;
        bytesLeft -= readBufSize;
        _fin.read(readBuf, readBufSize);
        readPos = 0;

        while (readPos < readBufSize)
        {
            codePos = 0;
            while (codePos < _codes[(unsigned char)readBuf[readPos]].length())
            {
                if (encPos < encBufSize)
                {   //write byte
                    encBuf[encPos] <<= 1;
                    if (_codes[(unsigned char)readBuf[readPos]][codePos] == '1')
                        encBuf[encPos] |= 1;
                    ++codePos;
                    --bitsLeft;
                    if (bitsLeft == 0)
                    {
                        ++encPos;
                        bitsLeft = CHAR_BIT;
                    }
                }
                else
                {   //write buffer
                    _fout.write(encBuf, encPos);
                    encPos = 0;
                }
            }
            ++readPos;
        }
    }
    if (bitsLeft > 0)//change for encPos
    {
        encBuf[encPos] <<= bitsLeft;
        _fout.write(encBuf, encPos + 1);
    }
    delete[] readBuf;
    delete[] encBuf;
}