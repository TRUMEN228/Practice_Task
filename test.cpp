#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>

using namespace std;

const int CHUNK = 100;

struct PatchModel {
    uint8_t key;
    int mem;
    vector<char> data;
};

// void CreatePatch(const string& ver1Path, const string& ver2Path, const string& patchPath) {
//     ifstream ver1File(ver1Path, ios::binary);
//     ifstream ver2File(ver2Path, ios::binary);
//     ofstream patchFile(patchPath, ios::binary);
//     ofstream checkFile("test_files/Dir1/mod.txt", ios::binary);

//     vector<char> ver1Buf(CHUNK);
//     vector<char> ver2Buf(CHUNK);



//     ver1File.close();
//     ver2File.close();
//     patchFile.close();
// }

int main() {
    string ver1Path = "test_files/Dir1/ver1.txt";
    string ver2Path = "test_files/Dir1/ver2.txt";
    string patchPath = "test_files/patch.bin";

    // ifstream patchFile(patchPath, ios::binary);

    

    vector<char> mass;
    int x = 10;
    mass.push_back(x);
    x = 0;
    cout << mass.data() << endl;

    return 0;
}
