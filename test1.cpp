#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>

using namespace std;

const int CHUNK = 100;

struct PatchModel {
    uint8_t key;
    int mem = 0;
    vector<char> data;
};

void CreatePatch(const string& ver1Path, const string& ver2Path, const string& patchPath) {
    ifstream ver1File(ver1Path, ios::binary);
    ifstream ver2File(ver2Path, ios::binary);
    ofstream patchFile(patchPath, ios::binary);

    int ver1Lim = (int)ver1File.seekg(0, ios::end).tellg() - CHUNK;
    int ver2Lim = (int)ver2File.seekg(0, ios::end).tellg() - CHUNK;

    vector<char> ver2Buf(CHUNK);

    vector<char> ver2Mismatch(sizeof(uint8_t));

    int ver1Shift = 0;
    int ver2Shift = 0;

    vector<PatchModel> patchData;
    vector<char> ver1Buf(CHUNK);

    vector<char> data;

    bool match = false;

    ver2File.seekg(0, ios::beg);

    while (ver2Shift <= ver2Lim) {
        ver2File.seekg(ver2Shift, ios::beg);
        ver2File.read(ver2Buf.data(), CHUNK);

        streamsize bytesRead2 = ver2File.gcount();
        if (bytesRead2 == 0) {
            cout << "Nothing to read in ver2" << endl;
            break;
        }

        ver1Shift = 0;

        while (ver1Shift <= ver1Lim) {
            ver1File.seekg(ver1Shift, ios::beg);
            ver1File.read(ver1Buf.data(), CHUNK);

            streamsize bytesRead1 = ver1File.gcount();
            if (bytesRead1 == 0) {
                cout << "Nothing to read in ver1" << endl;
                break;
            }

            else if (ver1Buf != ver2Buf) {
                match = false;
                ver1Shift++;
                ver1File.seekg(ver1Shift, ios::beg);
                continue;
            }

            else if (ver1Buf == ver2Buf) {
                match = true;
                cout << "Match found in " << ver1Shift << " byte in ver1" << endl;
                break;
            }
        }

        PatchModel patchBuf;

        if (!match) {
            ver2File.read(ver2Mismatch.data(), sizeof(uint8_t));
            data.insert(data.end(), ver2Mismatch.begin(), ver2Mismatch.end());
            ver2Shift++;

            cout << "Match didn't find in " << ver2Shift << " byte in ver2" << endl;
        }

        else if (match) {
            if (!data.empty()) {
                patchBuf.key = 0;
                patchBuf.mem = data.size();
                patchBuf.data = data;
                patchData.push_back(patchBuf);
                data = {};
            }

            cout << "Match found in " << ver2Shift << " byte in ver2" << endl;

            patchBuf.key = 1;
            patchBuf.mem = ver1Shift;
            patchBuf.data = {};
            patchData.push_back(patchBuf);
            ver2Shift += CHUNK;
        }

        // cin.ignore();
    }

    patchFile.write(reinterpret_cast<const char*>(&patchData), patchData.size() * sizeof(PatchModel));

    ver1File.close();
    ver2File.close();
    patchFile.close();
}

void ApplyPatch (const string& ver1Path, const string& patchPath, const string& patchedPath) {

}

int main() {
    string ver1Path = "test_files/Dir1/ver1.txt";
    string ver2Path = "test_files/Dir1/ver2.txt";
    string patchPath = "test_files/patch.bin";

    CreatePatch(ver1Path, ver2Path, patchPath);

    return 0;
}
