#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

#include <iostream>
#include <fstream>
/*
 * Class represents node of the code tree
 */
class Node {

    friend class Decoder;

    friend class ShannonFano;

public:

    Node() = default;

    Node(int frequency, unsigned char symbol);

    /**
     * Writes subtree to fout
     * @param fout - stream to write
     */
    void writeSubTree(std::ofstream& fout);

    ~Node();

private:
    int _frequency;
    std::string _code;  //only leafs of tree have it
    char _bit;
    unsigned char _symbol;
    Node* _left = nullptr;
    Node* _right = nullptr;
};
#endif //HUFFMAN_NODE_H
