#include "Helper.hpp"
#include "HCTree.hpp"
#include <unordered_map>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
	// Check command line args
	if (argc != 3) error("Incorrect usage");

	// Save original file and compressed file names
	char* original_file = argv[1];
	char* compressed_file = argv[2];
	
	// Read original file and count occurrences 
	unordered_map<char, int> occur_map;
	FancyInputStream fin(original_file);
	if (fin.filesize() == 0) {
		FancyOutputStream fout(compressed_file);
		return 0;
	}
	char curr;
	while ((curr = fin.read_byte()) != EOF) {
		occur_map[curr]++;
	}

	for (auto& pair : occur_map)  {
		//cout << pair.first << " " << pair.second << endl;
	}

	// Construct Huffman tree
	HCNode* root = build_tree(occur_map);

	// Write file header to compressed file
	FancyOutputStream fout(compressed_file);
	fout.write_int(occur_map.size());
	//cout << "Number of symbols: " << occur_map.size() << endl;
	fout.write_byte(' ');
	//cout << "Serialization: ";
	serialize(root, fout);
	//cout << endl;
	fout.write_byte('\n');

	// Compression
	unordered_map<char, string> codes;
	get_codes(root, codes, "");
	//cout << "Codes: " << endl;
	for (auto& pair : codes)  {
		//cout << pair.first << " " << pair.second << endl;
	}

	fin.reset();
	string encoded = "";
	while ((curr = fin.read_byte()) != EOF) {
		//cout << curr << endl;
		string code = codes[curr];
		for (char c : code) {
			encoded += c;
			int bit = c - '0';
			//cout << bit;
			fout.write_bit(bit);
		}
		//cout << endl;
	}
	//cout << encoded << endl;

	// Clear memory
	recurse_clear(root);
	
	// Exit success
	return 0;
}
