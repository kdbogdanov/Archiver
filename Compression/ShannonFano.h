#ifndef HUFFMAN_SHANNONFANO_H
#define HUFFMAN_SHANNONFANO_H

#include <vector>
#include "Node.h"

class ShannonFano {
    struct Symbol {
        friend ShannonFano;

        Symbol(int frequency, unsigned char symbol) : frequency(frequency), symbol(symbol) {};

        int frequency;
        unsigned char symbol;
        std::string code;
        Node* node;
    };

public:

    /**
     * Creates code tree
     */
    void buildTree();

    /**
     * Adds symbol to create prefix code
     * @param x - frequency of symbol
     * @param ch - symbol
     */
    void addChance(int chance, unsigned char symbol);

    /**
     * Returns code for symbol with index i
     * @param i - index of symbol
     * @return - code of symbol
     */
    std::string get(int i);


    /**
     * Stores code tree to file
     * @param filename - path to file for code tree
     */
    void writeTreeToFile(const std::string& filename);

    ~ShannonFano() { delete _root; }

private:
    /**
     * Recursively picks up codes for symbols
     * from _symbols in the range [begin;end]
     * @param begin - index of the first symbol
     * @param end - index of the last symbol
     * @param node - current node in the code tree
     */
    void fano(int begin, int end, Node* node);

    /**
     * Finds index of median so sum of frequences _symbols[begin..med]
     * is closest to sum of frequences of _symbols[med + 1..end]
     * @param begin - index of the beginning
     * @param end - index of the end
     * @return index of the median
     */
    int getMedianIndex(int begin, int end);

private:
    std::vector<Symbol> _symbols;
    Node* _root = nullptr;
};

#endif //HUFFMAN_SHANNONFANO_H
