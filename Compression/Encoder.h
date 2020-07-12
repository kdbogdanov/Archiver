#ifndef HUFFMAN_ENCODER_H
#define HUFFMAN_ENCODER_H

#include <iostream>
#include <fstream>

class Encoder {

public:
    /**
     * Sets prefix code for byte
     * @param code - code of byte
     * @param byte - byte
     */
    void setCode(std::string code, unsigned char byte);

    /**
     * Encodes file using specified prefix codes
     * @param inputFileName - path to file to encode
     * @param outputFileName - path to encoded file
     */
    void encodeFile(const std::string& inputFileName, const std::string& outputFileName);

private:

    /**
     * Encodes file as bytes
     */
    void encodeBytes();

private:
    std::ifstream _fin;
    std::ofstream _fout;
    std::string _codes[UCHAR_MAX + 1]{ "" };
};
#endif //HUFFMAN_ENCODER_H
