#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

const string READ_FILE = "read.txt";
const string ENCODED_FILE = "encoded.txt";
const char NEW_LINE = '\n';

struct Node {
	int count;
	char symbol;
	Node* left;
	Node* right;
};

bool compareNodesByCount(const Node* n1, const Node* n2) {
	return n1->count < n2->count;
}

Node* createNode(int count, char symbol,Node* left, Node* right) {
	Node* node = new Node;
	node->count = count;
	node->symbol = symbol;
	node->left = left;
	node->right = right;
	return node;
}
Node* createNode(int count, Node* left, Node* right) {
	Node* node = new Node;
	node->count = count;
	node->left = left;
	node->right = right;
	return node;
}

void printFrequencies(map<char, int>& frequencies) {
	for (auto itr = frequencies.begin(); itr != frequencies.end(); ++itr) {
		if (itr->first == NEW_LINE)
			cout << "/n";
		else
			cout << itr->first;
		cout << " -> " << itr->second << endl;
	}
}

void printCodes(map<char, string>& codes) {
	for (auto itr = codes.begin(); itr != codes.end(); ++itr) {
		if (itr->first == NEW_LINE)
			cout << "/n";
		else
			cout << itr->first;
		cout << " -> " << itr->second << endl;
	}
}

string readFile() {
	string rv = "";

	ifstream readStream;
	readStream.open(READ_FILE);

	string line;
	while (getline(readStream,line)) {
		rv += line;
		rv += "\n";
	}

	readStream.close();

	return rv;
}

map<char, int> getFrequencies(string data) {
	map<char, int> frequencies;
	for (int i = 0; i < data.length(); i++) {
		char current = data[i];
		if (frequencies.find(current) == frequencies.end())
			frequencies[current] = 0;
		frequencies[current]++;
	}
	return frequencies;
}

Node* buildHuffmanTree(map<char, int>& frequencies) {
	vector<Node*> nodes;
	for (auto itr = frequencies.begin(); itr != frequencies.end(); ++itr) {
		char symbol = itr->first;
		int count = itr->second;

		Node* node = createNode(count, symbol, NULL, NULL);
		nodes.push_back(node);
	}
	sort(nodes.begin(), nodes.end(), compareNodesByCount);

	while (nodes.size() != 1) {
		Node* first = nodes[0];
		Node* second = nodes[1];

		Node* root = createNode(first->count + second->count, first, second);
		nodes.erase(nodes.begin());
		nodes.erase(nodes.begin());

		nodes.push_back(root);

		sort(nodes.begin(), nodes.end(), compareNodesByCount);

	}
	return nodes[0];
}

void getCodes(Node* root,map<char,string>& codes,string currentCode) {
	if (root == NULL)
		return;
	if (root->left == NULL && root->right == NULL) {
		char symbol = root->symbol;

		if (codes.find(symbol) == codes.end())
			codes[symbol] = "";
		codes[symbol] = currentCode;

		return;
	}
	getCodes(root->left, codes, currentCode + "0");
	getCodes(root->right, codes, currentCode + "1");
}

void freeTree(Node* root) {
	if (root == NULL)
		return;
	freeTree(root->left);
	freeTree(root->right);
	delete root;
}

string encodeData(string data, map<char, string>& codes) {
	string code;
	for (int i = 0; i < data.length(); i++) {
		char current = data[i];
		code += codes[current];
	}
	return code;
}

void writeInEncodedFile(map<char,string>& codes, string code) {
	ofstream writeFile;
	writeFile.open(ENCODED_FILE);
	
	int codesCount = codes.size();
	writeFile << codesCount << endl;
	for (auto itr = codes.begin(); itr != codes.end(); ++itr) {
		char current = itr->first;
		string currentCode = itr->second;
		
		if (current == NEW_LINE)
			writeFile << "NEW_LINE" << endl;
		else
			writeFile << current << endl;
		
		writeFile << currentCode<<endl;
	}
	writeFile << code;

	writeFile.close();
}

pair<map<char,string>,string>  readEncodedFile() {
	ifstream readStream;
	readStream.open(ENCODED_FILE);

	map<char, string> codes;

	string line;
	getline(readStream, line);
	int codeCount = stoi(line);

	for (int i = 0; i < codeCount; i++) {
		string first; 
		getline(readStream, first);

		char symbol;
		if (first == "NEW_LINE")
			symbol = NEW_LINE;
		else
			symbol = first[0];

		string currentCode;
		getline(readStream, currentCode);

		codes[symbol] = currentCode;
	}
	string dataCode;
	readStream >> dataCode;
	readStream.close();

	return pair<map<char, string>, string>(codes, dataCode);
}

Node* buildEncodedTreeFromCodes(map<char, string>& codes) {
	Node* root = createNode(0, NULL, NULL);
	for (auto itr = codes.begin(); itr != codes.end(); ++itr) {
		char symbol = itr->first;
		string code = itr->second;

		Node* current = root;
		for (int i = 0; i < code.length(); i++) {
			if (code[i] == '0') {
				if(current->left == NULL)
					current->left = createNode(0, NULL, NULL);
				current = current->left;
			}
			else {
				if(current->right == NULL)
					current->right = createNode(0, NULL, NULL);
				current = current->right;
			}
		}
		current->symbol = symbol;
	}
	return root;
}

void parseEncodedFile() {
	pair<map<char, string>, string> readData = readEncodedFile();

	map<char, string> codes = readData.first;
	string code = readData.second;

	Node* root = buildEncodedTreeFromCodes(codes);
	
	Node* tmp = root;
	string data = "";

	for (int i = 0; i < code.length(); i++) {
		if (code[i] == '0')
			tmp = tmp->left;
		else
			tmp = tmp->right;


		if (tmp->left == NULL && tmp->right == NULL) {
			data += string(1,tmp->symbol);
			tmp = root;
		}
	}
	cout << data << endl;
	freeTree(root);
}


/*
	read.txt - Write the text you want to encode
	encoded.txt - encoded text is written in this file / parseEncodedFile() parses and converts it back
*/
int main() {

	/* Read data from file */
	string data = readFile();
	cout << data << endl;

	/* Get frequencies of each char in data */
	map<char, int> frequencies = getFrequencies(data);

	/* Build an encoding tree */
	Node* root = buildHuffmanTree(frequencies);

	/* Get a code for each char in data */
	map<char, string> codes; 
	getCodes(root, codes, "");
	//printCodes(codes);

	/* Free the space of pointers */
	freeTree(root);

	/* Get a code of data */
	string code = encodeData(data, codes);

	/* Write in a file */
	writeInEncodedFile(codes, code);

	cout << "----------" << endl << endl;
	/* Other */
	parseEncodedFile();

	return 0;
}
