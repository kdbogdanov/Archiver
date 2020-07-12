#include "ShannonFano.h"

using namespace std;

void ShannonFano::buildTree()
{
    delete _root;
    _root = new Node();
    _root->_code = "";
    fano(0, _symbols.size() - 1, _root);
}

void ShannonFano::addChance(int chance, unsigned char symbol)
{
    _symbols.emplace_back(Symbol(chance, symbol));
}

string ShannonFano::get(int i)
{
    return _symbols[i].code;
}

void ShannonFano::fano(int begin, int end, Node* node)
{
    if (begin >= end)
    {
        node->_symbol = _symbols[end].symbol;
        node->_left = nullptr;
        node->_right = nullptr;
        return;
    }

    int medIndex = getMedianIndex(begin, end);
    for (int i = begin; i <= end; ++i)
        _symbols[i].code += (i > medIndex) ? "0" : "1";

    Node* left = new Node();
    left->_bit = '0';
    Node* right = new Node();
    right->_bit = '1';

    node->_left = left;
    node->_right = right;
    fano(begin, medIndex, left);
    fano(medIndex + 1, end, right);
}

int ShannonFano::getMedianIndex(int begin, int end)
{
    int leftSum = 0;
    for (int i = begin; i < end; ++i)
        leftSum += _symbols[i].frequency;
    int rightSum = _symbols[end].frequency;

    int med = end;
    int delta;
    do
    {
        delta = leftSum - rightSum;
        --med;
        leftSum -= _symbols[med].frequency;
        rightSum += _symbols[med].frequency;
    } while (abs(leftSum - rightSum) <= delta);

    return med;
}

void ShannonFano::writeTreeToFile(const string& filename)
{
    ofstream fout;
    fout.open(filename, ios::binary);
    _root->writeSubTree(fout);
    fout.close();
}