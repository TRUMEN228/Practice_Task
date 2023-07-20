#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>

using namespace std;

const int CHUNK = 100;
int BUF_SIZE = CHUNK;

struct PatchModel {
    uint8_t key;
    int mem = 0;
    vector<char> data;
};

void CreatePatch(const string& ver1Path, const string& ver2Path, const string& patchPath) {
    ifstream ver1File(ver1Path, ios::binary);
    ifstream ver2File(ver2Path, ios::binary);
    ofstream patchFile(patchPath, ios::binary);

    int ver1Size = (int)ver1File.seekg(0, ios::end).tellg();
    int ver2Size = (int)ver2File.seekg(0, ios::end).tellg();

    int ver1Lim = ver1Size - CHUNK;
    // int ver2Lim = ver2Size - CHUNK;
    
    vector<char> ver1Buf(CHUNK);
    vector<char> ver2Buf(CHUNK);

    vector<char> ver2Mismatch(1);

    int ver1Shift = 0;
    int ver2Shift = 0;

    vector<PatchModel> patchData;
    
    vector<char> data;

    bool match = false;

    while (ver2Shift <= ver2Size) {
        ver2File.seekg(ver2Shift, ios::beg);
        ver2File.read(ver2Buf.data(), BUF_SIZE);
        
        streamsize bytesRead2 = ver2File.gcount();
        if (bytesRead2 == 0) {
            cout << "Nothing to read in ver2" << endl;
            break;
        }

        ver1Shift = 0;

        while (ver1Shift <= ver1Lim) {
            ver1File.seekg(ver1Shift, ios::beg);
            ver1File.read(ver1Buf.data(), BUF_SIZE);

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
            patchBuf.data = vector<char>(sizeof(int));
            memcpy(patchBuf.data.data(), &BUF_SIZE, sizeof(int));
            patchData.push_back(patchBuf);

            ver2Shift += CHUNK;

            if (ver2Size - ver2Shift < CHUNK && ver2Size - ver2Shift > 0) {
                BUF_SIZE = ver2Size - ver2Shift;
                ver1Buf = vector<char>(BUF_SIZE);
                ver2Buf = vector<char>(BUF_SIZE);
            }
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
    int buf_size;

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
            patchFile.read(reinterpret_cast<char*>(&buf_size), 4);
            patchBuf = vector<char>(buf_size);
            ver1File.read(patchBuf.data(), buf_size);
            patchShift += 4;
            // cout << "From ver1: " << patchInfo.mem << "byte || " << patchBuf.data() << endl;
        }

        else if (patchInfo.key == 0) {
            patchBuf.resize(patchInfo.mem);
            patchFile.read(patchBuf.data(), patchInfo.mem);
            patchShift += patchInfo.mem;
            // cout << "From patch: " << patchInfo.mem << "byte || " << patchBuf.data() << endl;
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
