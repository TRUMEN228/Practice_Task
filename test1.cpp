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

        PatchModel patchBuf;

        if (!match) {
            ver2File.seekg(ver2Shift, ios::beg);
            ver2File.read(ver2Mismatch.data(), 1);
            data.insert(data.end(), ver2Mismatch.begin(), ver2Mismatch.end());
            ver2Shift++;
        }

        else if (match) {
            if (!data.empty()) {
                patchBuf.key = 0;
                patchBuf.mem = data.size();
                patchBuf.data = data;
                patchData.push_back(patchBuf);
                data.clear();
            }

            patchBuf.key = 1;
            patchBuf.mem = ver1Shift;
            patchBuf.data = {};
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

    vector<char> patchBuf(CHUNK);
    vector<char> patchData;

    int patchSize = (int)patchFile.seekg(0, ios::end).tellg() - 5;

    int patchShift = 0;
    patchFile.seekg(0, ios::beg);

    while (patchShift <= patchSize) {
        PatchModel patchInfo;

        patchFile.seekg(patchShift, ios::beg);
        patchFile.read(reinterpret_cast<char*>(&patchInfo.key), 1);
        patchFile.read(reinterpret_cast<char*>(&patchInfo.mem), 4);
        patchShift += 5;

        if (patchInfo.key == 1) {
            ver1File.seekg(patchInfo.mem, ios::beg);
            cout << ver1File.tellg() << endl;
            ver1File.read(patchBuf.data(), CHUNK);
        }

        else if (patchInfo.key == 0) {
            patchFile.read(patchBuf.data(), patchInfo.mem);
            patchShift += patchInfo.mem;
        }

        patchData.insert(patchData.end(), patchBuf.begin(), patchBuf.end());
    }

    patchedFile.write(patchData.data(), patchData.size());
    
    ver1File.close();
    patchFile.close();
    patchedFile.close();
}

int main() {
    string ver1Path = "test_files/Dir1/ver1.txt";
    string ver2Path = "test_files/Dir1/ver2.txt";
    string patchPath = "test_files/patch.bin";
    string patchedPath = "test_files/Dir2/patched.txt";

    CreatePatch(ver1Path, ver2Path, patchPath);
    ApplyPatch(ver1Path, patchPath, patchedPath);

    return 0;
}
