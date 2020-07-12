#include "LZ77.h"

using namespace std;

LZ77::LZ77(int histBufSize, int prevBufSize)
{
    _histBufSize = histBufSize - 1;
    _prevBufSize = prevBufSize - 1;
    _bitsPerLength = log2(prevBufSize);
    _bitsPerOffset = log2(histBufSize);
}

void LZ77::encodeFile(const std::string& inputFileName, const std::string& outputFileName)
{
    _fin.open(inputFileName, ios::binary | ios::ate);
    _fout.open(outputFileName, ios::binary);
    encodeBytes();
    _fin.close();
    _fout.close();
}

void LZ77::encodeBytes()
{
    long long bytesLeft = _fin.tellg();
    _fin.seekg(0, ios::beg);
    _fout << bytesLeft << ':' << _bitsPerOffset << ':' << _bitsPerLength << ':' << _histBufSize << 'S';
    int readBufSize = (bytesLeft < BUFFER_SIZE) ? bytesLeft : BUFFER_SIZE;
    _readBuffer = new char[readBufSize];
    _writeBuffer = new char[BUFFER_SIZE];
    int histBegin = -_histBufSize;
    int prevBegin = 0;
    int prevEnd = (bytesLeft < _prevBufSize) ? bytesLeft - 1 : _prevBufSize - 1;
    int writePos = 0;
    int bitsLeftInChar = CHAR_BIT;
    int length = 0;
    int offset = 0;
    char symbol;

    _fin.read(_readBuffer, readBufSize);

    while (bytesLeft > 0)
    {
        if (prevBegin + _prevBufSize > BUFFER_SIZE)
        {        
            for (int i = 0; i < BUFFER_SIZE - histBegin; ++i)
                _readBuffer[i] = _readBuffer[histBegin + i];
            _fin.read(&_readBuffer[BUFFER_SIZE - histBegin], (histBegin < bytesLeft) ? histBegin : bytesLeft);
            histBegin = 0;
            prevBegin = _histBufSize;
            prevEnd = _histBufSize + _prevBufSize - 1;
        }

        if (histBegin < 0)
            if (bytesLeft < _prevBufSize)//if number of bytes left less than _prevBufSize
                getNextTriple(offset, length, symbol, 0, prevBegin, prevBegin + bytesLeft - 1);
            else
                getNextTriple(offset, length, symbol, 0, prevBegin, prevEnd);
        else if (bytesLeft < _prevBufSize)
            getNextTriple(offset, length, symbol, histBegin, prevBegin, prevBegin + bytesLeft - 1);
        else
            getNextTriple(offset, length, symbol, histBegin, prevBegin, prevEnd);

        writeNumAsBits(offset, _bitsPerOffset, writePos, bitsLeftInChar);
        writeNumAsBits(length, _bitsPerLength, writePos, bitsLeftInChar);
        writeNumAsBits(symbol, CHAR_BIT, writePos, bitsLeftInChar);

        histBegin += length + 1;
        prevBegin += length + 1;
        prevEnd += length + 1;
        bytesLeft -= length + 1;
    }

    if (writePos > 0)
    {
        _writeBuffer[writePos] <<= bitsLeftInChar;
        _fout.write(_writeBuffer, writePos + 1);
    }

    delete[]_readBuffer;
    delete[]_writeBuffer;
}

void LZ77::writeNumAsBits(int num, int bitsNumber, int& writePos, int& bitsLeftInChar)
{
    unsigned int curBit = ((unsigned int)1) << (bitsNumber - 1);
    int bitsLeftInNum = bitsNumber;
    while (bitsLeftInNum > 0)
    {
        if (writePos < BUFFER_SIZE)
        {
            _writeBuffer[writePos] <<= 1;
            if ((num & curBit) > 0)
                _writeBuffer[writePos] |= 1;

            curBit >>= 1;
            --bitsLeftInChar;
            --bitsLeftInNum;
            if (bitsLeftInChar == 0)
            {
                ++writePos;
                bitsLeftInChar = CHAR_BIT;
            }
        }
        else
        {
            _fout.write(_writeBuffer, BUFFER_SIZE);
            writePos = 0;
        }
    }
}

//may avoid permanent creations and deletions of array
void LZ77::getNextTriple(int& offset, int& length, char& symbol, int histBegin, int prevBegin, int prevEnd)
{
    length = 0;
    offset = 0;
    symbol = _readBuffer[prevBegin];
    int prevBufLength = prevEnd - prevBegin + 1;
    int histBufLength = prevBegin - histBegin;
    int* prefixLengths = prefixFunction(histBegin, prevBegin, prevEnd);
    int pos = histBufLength + prevBufLength + 1;

    //take first pattern in history buffer
    for (int i = prevBufLength + 1; i < 2 * prevBufLength + histBufLength + 1; ++i)
    {
        if (length < prefixLengths[i] && i - prefixLengths[i] + 1 < pos && i - prefixLengths[i] + 1 >= 0)
        {
            length = prefixLengths[i];
            //histBufLength - (i - prevBufLength - 1 - (length - 1));
            offset = histBufLength - (i - prevBufLength - length);
            symbol = _readBuffer[prevBegin + length];//_readBuffer[i - 1 - prevBufLength + 1];
        }
    }
    delete[] prefixLengths;
}

//may need checking
int* LZ77::prefixFunction(int histBegin, int prevBegin, int prevEnd)
{
    int prevBufLength = prevEnd - prevBegin + 1;
    int histBufLength = prevBegin - histBegin;
    int size = 2 * prevBufLength + histBufLength + 1;  //size of array of max prefix lengths
    int* maxPrefixLengths = new int[size];  //normal indexing
    maxPrefixLengths[0] = 0;  //no prefix in substring of length == 1
    int prefLen = 0;  //length of current prefix

    for (int i = 1; i < prevBufLength; ++i)
    {
        while (prefLen > 0 && _readBuffer[prefLen + prevBegin] != _readBuffer[i + prevBegin])
            prefLen = maxPrefixLengths[prefLen - 1];

        if (_readBuffer[prefLen + prevBegin] == _readBuffer[i + prevBegin])
            ++prefLen;

        maxPrefixLengths[i] = prefLen;
    }

    prefLen = 0;
    maxPrefixLengths[prevBufLength] = prefLen;  //store the border of substr ending with artificial nonexistent symbol

    for (int i = prevBufLength + 1; i < size; ++i)
    {   //complicated indexing is used to avoid coping of buffer elements
        //prefLen != prevBufLength required to indicate that max prefix already found
        //(because nonexistent symbol is artificial)
        if (prefLen == prevBufLength)
            prefLen = maxPrefixLengths[prefLen - 1];

        while (prefLen > 0 &&
            _readBuffer[(prefLen < prevBufLength) ? prefLen + prevBegin : prefLen - prevBufLength - 1 + histBegin] !=
            _readBuffer[i - prevBufLength - 1 + histBegin])
            prefLen = maxPrefixLengths[prefLen - 1];

        if (_readBuffer[i - prevBufLength - 1 + histBegin] ==
            _readBuffer[(prefLen < prevBufLength) ? prefLen + prevBegin : prefLen - prevBufLength - 1 + histBegin])
            ++prefLen;

        maxPrefixLengths[i] = prefLen;
    }
    return maxPrefixLengths;
}