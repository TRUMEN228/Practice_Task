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

    vector<char> ver1Buf(CHUNK);
    vector<char> ver2Buf(CHUNK);

    vector<char> ver2Mismatch(sizeof(uint8_t));

    int ver1Shift = 0;
    int ver2Shift = 0;

    vector<PatchModel> patchData;

    PatchModel patchBuf;
    patchBuf.mem = 0;

    bool match = false;

    ver1File.seekg(0, ios::beg);
    ver2File.seekg(0, ios::beg);

    while (!ver2File.eof()) {
        ver2File.read(ver2Buf.data(), CHUNK);
        cout << "This is " << ver2Shift << "'th byte in ver2" << endl;
        streamsize bytesRead2 = ver2File.gcount();

        if (bytesRead2 == 0) {
            cout << "Nothing to read in ver2" << endl;
            break;
        }

        while (!ver1File.eof()) {
            ver1File.read(ver1Buf.data(), CHUNK);
            cout << "This is " << ver1Shift << "'th byte in ver1" << endl;
            streamsize bytesRead1 = ver1File.gcount();

            if (bytesRead1 == 0) {
                cout << "Nothing to read in ver1" << endl;
                break;
            }

            if (ver1Buf.data() != ver2Buf.data()) {
                match = false;
                ver1Shift++;
                ver1File.seekg(ver1Shift, ios::beg);
            }

            else if (ver1Buf.data() == ver2Buf.data()) {
                match = true;
                cout << "Match found in " << ver1Shift << " byte in ver1" << endl;
                break;
            }
        }

        if (!match) {
            ver2File.seekg(patchBuf.mem, ios::beg);
            ver2File.read(ver2Mismatch.data(), sizeof(uint8_t));
            patchBuf.data.insert(patchBuf.data.end(), ver2Mismatch.begin(), ver2Mismatch.end());
            patchBuf.key = 0;
            patchBuf.mem++;
            
            cout << "Match didn't find in " << ver2Shift << " byte in ver2" << endl;
            ver2Shift++;
        }

        else if (match) {
            if (!patchBuf.data.empty()) {
                patchData.push_back(patchBuf);
                patchBuf.data.clear();
            }

            cout << "Match found in " << ver2Shift << " byte in ver2" << endl;

            patchBuf.key = 1;
            patchBuf.mem = ver1Shift;
            patchData.push_back(patchBuf);
            ver2Shift += CHUNK;
        }

        ver1Shift = 0;
        ver2File.seekg(ver2Shift, ios::beg);
        ver1File.seekg(1, ios::beg);
    }

    patchFile.write(reinterpret_cast<const char*>(&patchData), sizeof(patchData));

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
