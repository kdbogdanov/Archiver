#include "Node.h"

using namespace std;

Node::~Node()
{
    delete _left;
    delete _right;
}

Node::Node(const int frequency, unsigned char symbol)
{
    _frequency = frequency;
    _symbol = symbol;
}

void Node::writeSubTree(ofstream& fout)
{
    if (this->_left)
    {
        fout << 'I' << this->_bit;
        this->_left->writeSubTree(fout);
        this->_right->writeSubTree(fout);
    }
    else
        fout << 'L' << this->_bit << this->_symbol;
}