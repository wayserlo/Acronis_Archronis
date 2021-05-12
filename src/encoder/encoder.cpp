#include "encoder.hpp"

#include <src/encoder/tree/tree.hpp>

#include <vector>
#include <sstream>
#include <string>
#include <iostream>

namespace archiver {

std::vector<detail::Node*> Counting(IInputStream& input, std::vector<byte>& copy) {
    std::vector<detail::Node*> counter(256);
    for (int i = 0; i < 256; i++) {
        counter[i] = new detail::Node(0);
        counter[i]->symb = i;
    }
    byte inp;
    while (input.Read(inp)) {
        copy.push_back(inp);
        counter[inp]->freq ++;
    }

    for (int i = 0; i < counter.size(); i++) {
        if (counter[i]->freq == 0) {
            delete counter[i];
            counter.erase(counter.begin() + i);
            i--;
        }
    }
    return counter;
}


std::pair<detail::Node*, detail::Node*> FindMin(std::vector<detail::Node*> &counter) {
    std::pair<detail::Node *, detail::Node *> result;
    std::pair<int, int> minindex;

    result.first = (counter[0]->freq < counter[1]->freq ? counter[0] : counter[1]);
    result.second = (counter[0]->freq < counter[1]->freq ? counter[1] : counter[0]);
    minindex.first = (counter[0]->freq < counter[1]->freq ? 0 : 1);
    minindex.second = (counter[0]->freq < counter[1]->freq ? 1 : 0);

    for (int i = 2; i < counter.size(); i++) {
        if (counter[i]->freq < result.first->freq) {
            result.second = result.first;
            result.first = counter[i];
            minindex.second = minindex.first;
            minindex.first = i;
        } else if (counter[i]->freq < result.second->freq) {
            result.second = counter[i];
            minindex.second = i;
        }
    }

    if (minindex.first < minindex.second) {
        counter.erase(counter.begin() + minindex.second);
        counter.erase(counter.begin() + minindex.first);
    } else {
        counter.erase(counter.begin() + minindex.first);
        counter.erase(counter.begin() + minindex.second);
    }

    return result;
}


detail::Node* BuildTree(std::vector<detail::Node*> counter) {
    while (counter.size() > 1) {
        std::pair<detail::Node*, detail::Node*> min = FindMin(counter);
        detail::Node* v = new detail::Node(min.first->freq + min.second->freq);
        v->left = min.first;
        v->right = min.second;
        counter.push_back(v);
    }

    detail::Node* root = counter[0];
    return root;
}


void BuildTable(detail::Node* v, std::vector<std::string>& table, std::string& code) {
    if (v->left == nullptr && v->right == nullptr) {
        table[v->symb] = code;
        return;
    }

    code += '0';
    BuildTable(v->left, table, code);
    code.back() = '1';
    BuildTable(v->right, table, code);
    code.pop_back();
}


class Converter {
    byte num;
    int size;
    IInputStream& input;
    IOutputStream& output;

    void Send() {
        if (size == 8) {
            output.Write(num);
            size = 0;
            num = 0;
        }
    }

    bool Get() {
        size = 8;
        return input.Read(num);
    }

public:
    bool flag = true;

    Converter(IOutputStream& output, IInputStream& input) : num(0), size(0), output(output), input(input) {};

    void Add(std::string code) {
        for (int i = 0; i < code.size(); i++) {
            num = num << 1;
            if (code[i] == '0') {
                num = num & 254u;
            } else {
                num = num | 1u;
            }
            size++;
            Send();
        }
    }

    void Add(byte code) {
        output.Write(code);
    }

    void ZeroAddition(int n) {
        for (int i = 0; i < n - 1; i++) {
            Add("0");
        }
        Add("1");
    }

    byte GetByte() {
        if (size == 8) {
            byte code = num;
            input.Read(num);
            return code;
        } else {
            byte code;
            input.Read(code);
            return code;
        }
    }

    byte GetBit() {
        byte code;
        code = num >> (size - 1);
        code = code & 1;
        size--;
        if (size == 0) {
            flag = Get();
        }
        return code;
    }

    void Delete() {
        byte code;
        code = GetBit();
        while (code == 0) {
            code = GetBit();
        }
    }
};

void MakeCode(detail::Node* v, std::string& code) {
    if (v->left == nullptr && v->right == nullptr) {
        code += '1';
        return;
    }
    code += '0';
    MakeCode(v->left, code);
    MakeCode(v->right, code);
}

void AddElements(detail::Node* v, Converter& conv) {
    if (v->left == nullptr && v->right == nullptr) {
        conv.Add(v->symb);
        return;
    }
    AddElements(v->left, conv);
    AddElements(v->right, conv);
}

void ClearTree(detail::Node* v) {
    if (v == nullptr) {
        return;
    }
    ClearTree(v->right);
    ClearTree(v->left);
    delete v;
}

void Encode(IInputStream& original, IOutputStream& compressed) {
    std::vector<byte> buff;
    std::vector<detail::Node*> counter = Counting(original, buff);

    detail::Node* root = BuildTree(counter);

    std::vector<std::string> codetable(256);
    std::string code;

    BuildTable(root, codetable, code);

    byte inp;

    int tablesize = 2 * counter.size() - 1;

    int codesize = 0;

    for (int i = 0; i < counter.size(); i++) {
        codesize += counter[i]->freq * codetable[counter[i]->symb].size();
    }

    std::string treecode;

    MakeCode(root, treecode);
    Converter conv(compressed, original);

    conv.Add(static_cast<byte>(counter.size()));
    conv.ZeroAddition(8 - (tablesize % 8));
    conv.Add(treecode);

    AddElements(root, conv);

    conv.ZeroAddition(8 - codesize % 8);

    for (auto it : buff) {
        conv.Add(codetable[it]);
    }

    ClearTree(root);
}

void RestoreTree(detail::Node* v, Converter& conv, int& size) {
    if (size == 0) {
        return;
    }
    byte code = conv.GetBit();
    if (code == 0) {
        v->left =  new detail::Node();
        RestoreTree(v->left, conv, size);
        v->right = new detail::Node();
        RestoreTree(v->right, conv, size);
    } else {
        size--;
    }
}

void FillLeaves (detail::Node* v, Converter& conv) {
    if (v->right == nullptr && v->left == nullptr) {
        v->symb = conv.GetByte();
        return;
    }
    FillLeaves(v->left, conv);
    FillLeaves(v->right, conv);
}

void Decode(IInputStream& compressed, IOutputStream& original)
{
    Converter conv(original, compressed);
    int number = conv.GetByte();

    int size = number;
    if  (number == 0) {
        number = 256;
    }

    conv.Delete();     // Very long

    detail::Node* root = new detail::Node();
    RestoreTree(root, conv, size);

    FillLeaves(root, conv);

    conv.Delete();

    detail::Node* v = root;
    do {
        byte code = conv.GetBit();
        if (code == 0) {
            v = v->left;
        } else {
            v = v->right;
        }

        if (v->left == nullptr && v->right == nullptr) {
            conv.Add(v->symb);
            v = root;
        }
    } while (conv.flag);

    ClearTree(root);
}

}   // namespace archiver

