#include "LZ77Decoder.h"

using namespace std;

void LZ77Decoder::decodeFile(const std::string& inputFileName, const std::string& outputFileName)
{
    _fin.open(inputFileName, ios::binary);
    _fout.open(outputFileName, ios::binary);
    decodeBytes();
    _fin.close();
    _fout.close();
}

void LZ77Decoder::decodeBytes()
{
    int bytesLeft;
    int histBufSize;
    char dummy;
    _fin >> bytesLeft;
    _fin >> dummy;
    _fin >> _bitsPerOffset;
    _fin >> dummy;
    _fin >> _bitsPerLength;
    _fin >> dummy;
    _fin >> histBufSize;
    _fin >> dummy;
    long long start = _fin.tellg();
    _fin.seekg(0, ios::end);
    long long end = _fin.tellg();
    _fin.seekg(start, ios::beg);
    int bitsPerTriple = _bitsPerOffset + _bitsPerLength + CHAR_BIT;

    _readBuffer = new char[BUFFER_SIZE];
    _writeBuffer = new char[BUFFER_SIZE];
    int readPos = 0;
    int writePos = 0;
    int writeBegin = 0;
    int curBit = 0;
    int offset = 0;
    int length = 0;
    char symbol;

    _fin.read(_readBuffer, BUFFER_SIZE);

    while (bytesLeft > 0)
    {
        if (bitsPerTriple >= CHAR_BIT * (BUFFER_SIZE - readPos) - curBit)
        {
            for (int i = 0; i < BUFFER_SIZE - readPos; ++i)
                _readBuffer[i] = _readBuffer[i + readPos];
            _fin.read(&_readBuffer[BUFFER_SIZE - readPos], readPos);
            readPos = 0;
        }

        offset = readNumFromBits(readPos, curBit, _bitsPerOffset);
        length = readNumFromBits(readPos, curBit, _bitsPerLength);
        symbol = (char)readNumFromBits(readPos, curBit, CHAR_BIT);

        if (writePos + length + 1 > BUFFER_SIZE)
        {
            _fout.write(&_writeBuffer[writeBegin], writePos - writeBegin);
            for (int i = 0; i < histBufSize; ++i)
                _writeBuffer[i] = _writeBuffer[writePos - histBufSize + i];
            writeBegin = histBufSize;
            writePos = histBufSize;
        }

        for (int i = 0; i < length; ++i)
            _writeBuffer[writePos + i] = _writeBuffer[writePos + i - offset];

        writePos += length;
        bytesLeft -= length + 1;
        _writeBuffer[writePos] = symbol;
        ++writePos;
    }

    if (writePos > 0)
        _fout.write(&_writeBuffer[writeBegin], writePos - writeBegin + bytesLeft);

    delete[]_readBuffer;
    delete[]_writeBuffer;
}

int LZ77Decoder::readNumFromBits(int& curByte, int& curBit, int bitsInNum)
{
    int number = 0;
    while (bitsInNum > 0)
    {
        if (bitsInNum >= CHAR_BIT - curBit)
        {
            int bitsWritten = CHAR_BIT - curBit;
            for (curBit; curBit < CHAR_BIT; ++curBit)
            {
                number <<= 1;
                if (_readBuffer[curByte] & (1 << CHAR_BIT - curBit - 1))
                    number |= 1;
            }
            curBit = 0;
            ++curByte;
            bitsInNum -= bitsWritten;//CHAR_BIT;
        }
        else
        {
            int end = curBit + bitsInNum;
            for (curBit; curBit < end; ++curBit)
            {
                number <<= 1;
                if (_readBuffer[curByte] & (1 << CHAR_BIT - curBit - 1))
                    number |= 1;
            }
            bitsInNum = 0;
        }
    }
    return number;
}