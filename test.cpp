#include <stdint.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

const int CHECK_SIZE = 100;

struct PatchInfo {
    uint8_t key = 0;
    int mem = 0;
    vector<char> data;
};

void CreatePatch(const string& ver1Path, const string& ver2Path, const string& patchPath) {
    ifstream ver1(ver1Path, ios::binary | ios::ate);
    ifstream ver2(ver2Path, ios::binary | ios::ate);
    ofstream patch(patchPath, ios::binary);

    vector<char> ver1Data(CHECK_SIZE);
    vector<char> ver2Data(CHECK_SIZE);

    // for (int i = 0; !ver1.eof(); ++i) {
    //     ver1 >> ver1Data[i];
    // }

    // for (int i = 0; !ver2.eof(); ++i) {
    //     ver2 >> ver2Data[i];
    // }
    
    vector<PatchInfo> patchData;

    while (ver2) {
        ver1.read(ver1Data.data(), CHECK_SIZE);
        ver2.read(ver2Data.data(), CHECK_SIZE);

        if (ver1Data == ver2Data) {

            if (!patchData.empty()) {
                patch.write(reinterpret_cast<char*>(patchData.data()), patchData.size() * sizeof(PatchInfo));
                patchData.clear();
            }

            PatchInfo match;
            match.key = 1;
            match.mem += CHECK_SIZE;
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

    // if (!patchData.empty()) {
    //     patch.write(reinterpret_cast<char*>(&key), sizeof(uint8_t));
    //     patch.write(reinterpret_cast<char*>(&mem), sizeof(int));
    //     patch.write(patchData.data(), mem);
    // }

    if (!patchData.empty()) {
        patch.write(reinterpret_cast<char*>(patchData.data()), patchData.size() + sizeof(PatchInfo));
    }
    
//     for (int i = 0; i < ver2Data.size(); i += CHECK_SIZE) {
//         int remainingSize = ver2Data.size() - i;
//         int compareSize = min(CHECK_SIZE, remainingSize);

//         if (compareSize < CHECK_SIZE) {
//             mem = 0;
//             patchData.emplace_back(PatchElement{0, mem, {}});
//             patchData.back().data.insert(patchData.back().data.end(), ver2Data.begin() + i, ver2Data.end());
//             break;
//         }

//         if (ver1Data.data() + mem != ver2Data.data() + i) {
//             mem = 1;
//             while (ver1Data.data() + mem != ver2Data.data() + i) {
//                 if (mem + compareSize >= ver1Data.size()) {
//                     mem = 0;
//                     patchData.emplace_back(PatchElement{0, mem, {}});
//                     patchData.back().data.insert(patchData.back().data.end(), ver2Data.begin() + i, ver2Data.begin() + i + compareSize);
//                     break;
//                 }
//                 mem++;
//             }
//             if (mem != 0) {
//                 patchData.emplace_back(PatchElement{1, mem, {}});
//             }
//         }

//         if (mem != 0) {
//             patchData.back().data.insert(patchData.back().data.end(), ver2Data.begin() + i, ver2Data.begin() + i + compareSize);
//         }

//         mem += compareSize;
//     }

    cout << "Патч успешно создан" << endl;

    return;
}

void ApplyPatch(const string& ver1Path, const string& patchPath, const string& patchedPath) {
    ifstream ver1(ver1Path, ios::binary);
    ifstream patch(patchPath, ios::binary);
    ofstream patched(patchedPath, ios::binary);

    vector<char> ver1Data((istreambuf_iterator<char>(ver1)), (istreambuf_iterator<char>()));
    // vector<PatchInfo> patchData;

    // while (patch) {
    //     PatchInfo patchBuffer;
    //     patch.read(reinterpret_cast<char*>(&patchBuffer.key), sizeof(uint8_t));
    //     patch.read(reinterpret_cast<char*>(&patchBuffer.mem), sizeof(int));

    //     if (patchBuffer.key == 0) {
    //         int dataSize;
    //         patch.read(reinterpret_cast<char*>(&dataSize), sizeof(int));
    //         patchBuffer.data.resize(dataSize);
    //         patch.read(patchBuffer.data.data(), dataSize);
    //     }

    //     patchData.push_back(patchBuffer);
    // }

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

    // for (const auto& patchBuffer : patchData) {
    //     if (patchBuffer.key == 0) {
    //         patched.write(patchBuffer.data.data(), patchBuffer.data.size());
    //     }

    //     else if (patchBuffer.key == 1) {
    //         patched.write(ver1Data.data() + patchBuffer.mem, ver1Data.size() - patchBuffer.mem);
    //     }
    // }

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
