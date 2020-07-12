#ifndef HUFFMAN_DECODER_H
#define HUFFMAN_DECODER_H

#include "Node.h"

class Decoder {
public:

    /**
     * Decodes file using code tree from it
     * @param inputFileName - file to decode
     * @param outputFileName - file to store decoded bytes
     */
    void decode(const std::string& inputFileName, const std::string& outputFileName);

    ~Decoder() { delete _root; }

private:

    /**
     * Recursively decodes tree from "_fout"
     * @return root of decoded tree
     */
    Node* decodeTree();

    /**
    * Decodes bytes from "_fin" and writes them to "_fout"
    * until there are codes to decode
    */
    void decodeBytes();


private:
    std::ifstream _fin;
    std::ofstream _fout;
    Node* _root = nullptr;  //root of the code tree
};
#endif //HUFFMAN_DECODER_H
