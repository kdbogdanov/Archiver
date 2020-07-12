#include "Decoder.h"

using namespace std;

void Decoder::decode(const string& inputFileName, const string& outputFileName)
{
    _fin.open(inputFileName, ios::binary);
    _fout.open(outputFileName, ios::binary);

    if (_fin.is_open() && _fout.is_open())
    {
        _root = decodeTree();
        decodeBytes();
    }
    _fin.close();
    _fout.close();
}

Node* Decoder::decodeTree()
{
    char ch;
    _fin >> ch;
    Node* node = new Node();
    if (ch == 'L')
    {
        _fin >> node->_bit;
        _fin.get(ch);
        node->_symbol = (unsigned char)ch;
        return node;
    }
    else
    {
        _fin >> node->_bit;
        node->_left = decodeTree();
        node->_right = decodeTree();
        return node;
    }
}

void Decoder::decodeBytes()
{
    Node* node = _root;
    int shift = 7;
    long long codesLeft;  //stores number of encoded symbols
    long long readBufSize = 1024 * 1024;
    long long writeBufSize = readBufSize;
    char* readBuf = new char[readBufSize];
    char* writeBuf = new char[writeBufSize];
    int readPos = readBufSize;
    int writePos = 0;

    _fin >> codesLeft;
    _fin >> readBuf[0];  //extract separating symbol

    //get number of bytes to read
    long long beg = _fin.tellg();
    _fin.seekg(0, ios::end);
    long long end = _fin.tellg();
    _fin.seekg(beg, ios::beg);
    end -= beg;

    while (codesLeft > 0)
    {
        if (readPos >= readBufSize)
        {
            readBufSize = (end < readBufSize) ? end : readBufSize;
            end -= readBufSize;
            _fin.read(readBuf, readBufSize);
            readPos = 0;
        }
        if (writePos >= writeBufSize)
        {
            _fout.write(writeBuf, writePos);
            writePos = 0;
        }
        if (shift < 0)
        {
            shift = 7;
            ++readPos;
        }
        else
        {
            if ((readBuf[readPos] & (1 << shift)) == (1 << shift))//just check > 0?
                node = node->_left;
            else
                node = node->_right;
            --shift;
            if (!node->_left)
            {
                writeBuf[writePos] = node->_symbol;
                ++writePos;
                --codesLeft;
                node = _root;
            }
        }
    }

    if (writePos != 0)
        _fout.write(writeBuf, writePos);
    delete[] readBuf;
    delete[] writeBuf;
}
