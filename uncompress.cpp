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
	char* uncompressed_file = argv[2];

    unordered_map<char, int> occur_map;
    FancyInputStream fin(original_file);
    if (fin.filesize() == 0) {
		FancyOutputStream fout(uncompressed_file);
		return 0;
	}
    int num_symbols = fin.read_int();
    //cout << "Number of symbols: " << num_symbols << endl;
    char delim = fin.read_byte(); // space
    int total = 0;
    int i;
    for (i = 0; i < num_symbols; i++) {
        char symbol = fin.read_byte();
        //cout << symbol << " ";
        delim = fin.read_byte();
        int freq = fin.read_int();
        //cout << freq << endl;
        occur_map[symbol] = freq;
        total += freq;
        delim = fin.read_byte(); // space
     }
     //cout << total << endl;
     delim = fin.read_byte(); // newline

    // Construct Huffman tree
	HCNode* root = build_tree(occur_map);

    unordered_map<char, string> codes;
	get_codes(root, codes, "");
    unordered_map<string, char> symbols;
    // Invert map <code, symbol>
    for (auto& pair : codes) {
        symbols[pair.second] = pair.first;
        //cout << pair.second << " " << pair.first << endl;
    }

    // Decompression
    FancyOutputStream fout(uncompressed_file);
    int bit;
    string code = "";
    while ((bit = fin.read_bit()) != -1) {
        //cout << bit;
        if (bit == 1) {
            code += "1";
            goto check_code;
        }
        else code += "0";
        check_code:
            if (symbols.count(code)) {
                fout.write_byte(symbols[code]);
                //cout << symbols[code] << endl;
                code = "";
                total--;
                if (total == 0) break;
            }
    }

    // Clear memory
    recurse_clear(root);

    // Exit success
    return 0;
}
