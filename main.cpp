#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <vector>
#include <bitset>
#include <cstring>
#include <cmath>
#include "Node.h"

using namespace std;
const int maxchar = 300;
int pInt[maxchar];

struct CompFreq {
	bool operator()(Node* const& first, Node* const& second) {
		return first->Frequancy > second->Frequancy;
	}
};
priority_queue<Node *, vector<Node *>, CompFreq> X;
void addToX(long long fileSize, float &H) {
    for (int i = 0; i < maxchar; i++) {
        if (pInt[i] != 0) {
            float p = pInt[i] / float(fileSize);
            H += (p * log2(p));
            Node *leaf = new Node;
            leaf->Frequancy = pInt[i];
            leaf->c = i;
            leaf->right = NULL;
            leaf->left = NULL;
            X.push(leaf);
        }
    }
}
void Decompress(string fileName, string fileNameBin, Node* root, long long originalsize) {
    ifstream infile;
    ofstream outfile;
    string str = "";
    Node* temp = new Node;
    temp = root;
    infile.open(fileNameBin.c_str(), ios::binary | ios::in);
    outfile.open(fileName.c_str());
    if (outfile.is_open() && infile.is_open()) {
        int base = 128;
        unsigned __int8 test;
        infile.read(reinterpret_cast<char*>(&test), sizeof(unsigned __int8));
        while (!infile.eof() && str.size() < originalsize) {
            if (base < 1) {
                infile.read(reinterpret_cast<char*>(&test), sizeof(unsigned __int8));
                base = 128;
            }
            if (!infile.eof()) {
                while (temp->right && base > 0) {
                    if (test / base == 0)
                        temp = temp->right;
                    else
                        temp = temp->left;
                    test %= base;
                    base /= 2;
                }
                if (!temp->right)
                {
                    str += char(temp->c);
                    temp = root;
                }
            }
        }
        for (int i = 0; i < originalsize; i++)
            outfile << str[i];
        outfile.close();
        infile.close();
    }
    else cout << "The .bin file does not exist" << endl;
}
void CreateCompressedFile(string fileName, string Bin, string *a) {
    ifstream infile;
    ofstream outfile;
    outfile.open(Bin.c_str(), ios::binary | ios::out);
    infile.open(fileName.c_str());
    if (outfile.is_open() && infile.is_open()) {
        char c;
        string s = "";
        int size = 0;
        while (!infile.eof()) {
            infile.get(c);
            if (!infile.eof()) {
                s += a[int(c)];
                size++;
            }
        }
        int index = 0;
        int base = 1;
        int counter = 1;
        while (index < s.size()) {
            unsigned __int8 x = 0;
            base = 1;
            for (int i = 0; i < 8; i++) {
                if ((8 * counter - 1 - i) < 0 || (8 * counter - 1 - i) >= s.size())
                    x += base;
                else
                    x += (s[8 * counter - 1 - i] - '0')*base;
                base *= 2;
                index++;
            }
            outfile.write(reinterpret_cast<const char*>(&x), sizeof(unsigned __int8));
            counter++;
        }
        outfile.close();
        infile.close();
    }
    else {
        cout << "Binary File Not Found" << endl;
    }
}
void CheckHeap() {
    while (!X.empty()) {
        Node* right = X.top();
        X.pop();
        if (X.empty()) {
            root = right;
        }
        else {
            Node *left = new Node;
            left = X.top();
            X.pop();
            Node *n = new Node();
            n->c = -1;
            n->Frequancy = right->Frequancy + left->Frequancy;
            n->left = left;
            n->right = right;
            X.push(n);
            root = n;
        }
    }
}
void CalculateCharachtersFrequancy(string fileName, int *arr, long long &originalsize) {
    originalsize = 0;
    for (int i = 0; i < maxchar; i++)
        arr[i] = 0;
    ifstream infile;
    infile.open(fileName.c_str());
    if (infile.is_open()) {
        char c;
        while (!infile.eof()) {
            infile.get(c);
            if (!infile.eof()) {
                arr[int(c)]++;
                originalsize++;
            }
        }
        infile.close();
    }
    else cout << "The file is not found" << endl;
}
void CharCode(Node *root, string str, string *arr) {
    if (root->right == NULL) {
        arr[root->c] = str;
        return;
    }
    else {
        CharCode(root->right, str + "0", arr);
        CharCode(root->left, str + "1", arr);
    }
}
int main() {
	long long fileSize;
	char choice;
	cout << "Please Select Operation.. C: Compression, D: Decompression, E: Exit\n";
	cin >> choice;
	while (choice != 'E') {
		if (choice == 'C') {
			string fileName, fileNameBin;
			cout << "Enter the name of the file you want to compress" << endl;
			cin >> fileName;
			cout << "Enter the name of the binary file" << endl;
			cin >> fileNameBin;
			CalculateCharachtersFrequancy(fileName, pInt, fileSize);
			float H = 0;
            addToX(fileSize, H);
			bool SingleCharacter = false;
			if (X.size() == 1)
				SingleCharacter = true;
			H = -1 * H;
			CheckHeap();
			string CodesArray[maxchar];
			string str = "";
			if (SingleCharacter) str = "0";
			CharCode(root, str, CodesArray);
			float L = 0;
			for (int i = 0; i < maxchar; i++) {
				L += CodesArray[i].size() * pInt[i] / float(fileSize);
			}
			CreateCompressedFile(fileName, fileNameBin, CodesArray);
			cout << "Efficiency: " << H / L << endl;
			cout << "Ratio:  = " << L / 8.0 << endl;
		}
		else if (choice == 'D') {
			string DeCompFileName, BinFile;
			cout << "Enter the Compressed file name\n" << endl;
			cin >> BinFile;
			cout << "Enter the output file name\n" << endl;
			cin >> DeCompFileName;
			Decompress(DeCompFileName, BinFile, root, fileSize);
		}
		cout << "For compression press C, For Decompression press D\n";
		cin >> choice;
	}
	system("pause");
	return 0;
}
