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

void CreatePatch(const string& ver1Path, const string& ver2Path, const string& patchPath) {
    ifstream ver1File(ver1Path, ios::binary);
    ifstream ver2File(ver2Path, ios::binary);
    ofstream patchFile(patchPath, ios::binary);
    ofstream checkFile("test_files/Dir1/mod.txt", ios::binary);
    
    if (!ver1File) {
        cout << "Can't open ver1 file" << endl;
        return;
    }

    if (!ver2File) {
        cout << "Can't open ver2 file" << endl;
        return;
    }

    vector<char> ver1Buf(CHUNK);
    vector<char> ver2Buf(CHUNK);

    ver1File.seekg(0, ios::beg);
    ver1File.read(ver1Buf.data(), CHUNK);
    ver1File.seekg(500, ios::beg);
    ver1File.read(ver1Buf.data(), CHUNK);
    // ver1File.seekg(0, ios::beg);
    ver1File.read(ver1Buf.data(), CHUNK);
    checkFile.write(ver1Buf.data(), CHUNK);

    // while (!ver2File.eof()) {
    //     ver2File.read(ver2Buf.data(), CHUNK);
        
    //     ver1File.seekg(0, ios::beg);

    //     while (!ver1File.eof()) {
    //         ver1File.read(ver1Buf.data(), CHUNK);
    //         checkFile.write(ver1Buf.data(), CHUNK);
    //         ver1File.seekg(100, ios::cur);
    //     }

    //     ver2File.seekg(1, ios::cur);
    //     cin.ignore();
    // }

    ver1File.close();
    ver2File.close();
    patchFile.close();
}

int main() {
    string ver1Path = "test_files/Dir1/ver1.txt";
    string ver2Path = "test_files/Dir1/ver2.txt";
    string patchPath = "test_files/patch.bin";

    CreatePatch(ver1Path, ver2Path, patchPath);

    return 0;
}
