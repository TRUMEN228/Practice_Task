#include <stdint.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const int BLOCK_SIZE = 100;

struct PatchInfo {
    uint8_t key;
    int mem;
    vector<char> data;
};

void CreatePatch(const string& ver1Path, const string& ver2Path, const string& patchPath) {
    ifstream ver1(ver1Path, ios::binary | ios::ate);
    ifstream ver2(ver2Path, ios::binary | ios::ate);
    ofstream patch(patchPath, ios::binary);

    vector<char> ver1Data(BLOCK_SIZE);
    vector<char> ver2Data(BLOCK_SIZE);
    
    vector<PatchInfo> patchData;

    while (ver2) {
        ver1.read(ver1Data.data(), BLOCK_SIZE);
        ver2.read(ver2Data.data(), BLOCK_SIZE);

        if (ver1Data == ver2Data) {

            if (!patchData.empty()) {
                patch.write(reinterpret_cast<char*>(patchData.data()), patchData.size() * sizeof(PatchInfo));
                patchData.clear();
            }

            PatchInfo match;
            match.key = 1;
            match.mem += BLOCK_SIZE;
            patchData.push_back(match);
        }

        else {
            PatchInfo mismatch;
            mismatch.key = 0;
            mismatch.mem = 0;
            // patchData.insert(patchData.end(), ver2Data.begin(), ver2Data.begin() + ver2.gcount());
            mismatch.data.assign(ver2Data.begin(), ver2Data.begin() + ver2.gcount());
            patchData.push_back(mismatch);
        }
    }

    if (!patchData.empty()) {
        patch.write(reinterpret_cast<char*>(patchData.data()), patchData.size() + sizeof(PatchInfo));
    }

    cout << "Патч успешно создан" << endl;

    return;
}

void ApplyPatch(const string& ver1Path, const string& patchPath, const string& patchedPath) {
    ifstream ver1(ver1Path, ios::binary);
    ifstream patch(patchPath, ios::binary);
    ofstream patched(patchedPath, ios::binary);

    vector<char> ver1Data((istreambuf_iterator<char>(ver1)), (istreambuf_iterator<char>()));

    while (patch) {
        PatchInfo patchBuffer;
        patch.read(reinterpret_cast<char*>(&patchBuffer), sizeof(PatchInfo));

        if (patchBuffer.key == 0) {
            patched.write(patchBuffer.data.data(), patchBuffer.data.size());
        }

        else if (patchBuffer.key == 1){
            patched.write(ver1Data.data() + patchBuffer.mem, ver1Data.size() + patchBuffer.mem);
        }
    }

    cout << "Патч успешно применен" << endl;
}

int main() {
    string ver1Path = "test_files/Dir1/ver1.txt";
    string ver2Path = "test_files/Dir1/ver2.txt";
    string patchPath = "test_files/patch.patch";
    string patchedPath = "test_files/Dir2/patched.txt";

    CreatePatch(ver1Path, ver2Path, patchPath);
    ApplyPatch(ver1Path, patchPath, patchedPath);

    return 0;
}
