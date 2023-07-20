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

    PatchModel(uint8_t keyVal, int memVal, vector<char> dataVal) : key(keyVal), mem(memVal), data(dataVal) {}
};

void CreatePatch(const string& ver1Path, const string& ver2Path, const string& patchPath) {
    ifstream ver1File(ver1Path, ios::binary);
    ifstream ver2File(ver2Path, ios::binary);
    ofstream patchFile(patchPath, ios::binary);

    int ver1Lim = (int)ver1File.seekg(0, ios::end).tellg() - CHUNK;
    int ver2Lim = (int)ver2File.seekg(0, ios::end).tellg() - CHUNK;

    vector<char> ver2Buf(CHUNK);

    vector<char> ver2Mismatch(1);

    int ver1Shift = 0;
    int ver2Shift = 0;

    vector<PatchModel> patchData;
    vector<char> ver1Buf(CHUNK);

    vector<char> data;

    bool match = false;

    while (ver2Shift <= ver2Lim) {
        ver2File.seekg(ver2Shift, ios::beg);
        // cout << "Current position in ver2 is " << ver2File.tellg() << endl;
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

            if (ver1Buf != ver2Buf) {
                match = false;
                ver1Shift++;
                continue;
            }

            else if (ver1Buf == ver2Buf) {
                match = true;
                break;
            }
        }

        if (!match) {
            ver2File.seekg(ver2Shift, ios::beg);
            ver2File.read(ver2Mismatch.data(), 1);
            data.insert(data.end(), ver2Mismatch.begin(), ver2Mismatch.end());
            ver2Shift++;
        }

        else if (match) {
            if (!data.empty()) {
                PatchModel patchBuf(0, data.size(), data);
                
                patchData.push_back(patchBuf);
                data.clear();
            }

            PatchModel patchBuf(1, ver1Shift, {});
            patchData.push_back(patchBuf);
            ver2Shift += CHUNK;
        }
    }

    for (const PatchModel& data : patchData) {
        patchFile.write(reinterpret_cast<const char*>(&data.key), sizeof(uint8_t));
        patchFile.write(reinterpret_cast<const char*>(&data.mem), sizeof(int));
        patchFile.write(data.data.data(), data.data.size());
    }

    ver1File.close();
    ver2File.close();
    patchFile.close();

    return;
}

void ApplyPatch (const string& ver1Path, const string& patchPath, const string& patchedPath) {
    ifstream ver1File(ver1Path, ios::binary);
    ifstream patchFile(patchPath, ios::binary);
    ofstream patchedFile(patchedPath, ios::binary);

    // vector<char> patchBuf(1);
    vector<char> patchData;

    int patchShift = 0;
    patchFile.seekg(0, ios::beg);

    // while (!patchFile.eof()) {
    //     PatchModel patchInfo();
    //     patchFile.read(patchInfo().key, 1);

    //     if (patchBuf[0] == 1) {
            
    //     }
    // }
}

int main() {
    string ver1Path = "test_files/Dir1/ver1.txt";
    string ver2Path = "test_files/Dir1/ver2.txt";
    string patchPath = "test_files/patch.bin";
    string patchedPath = "test_files/Dir2/patched.txt";

    CreatePatch(ver1Path, ver2Path, patchPath);

    return 0;
}
