#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

// Функция проверки файлов на открытие
bool FilesAreOpened(const string origFile, const string modFile) {
    ifstream ver1(origFile);
    ifstream ver2(modFile);

    if (ver1.is_open() && ver2.is_open()) {
        return true;
    }

    else {
        return false;
    }
}

// Функция создания патч-файла
void CreatePatchFile(const string origFile, const string modFile, const string patchFile) {
    ifstream ver1(origFile);
    ifstream ver2(modFile);
    ofstream patch(patchFile);

    vector<string> orig_lines;
    string line;

    while (getline(ver1, line)) {
        orig_lines.push_back(line);
    }

    int line_number = 0;
    while (getline(ver2, line)) {
        if (line_number < orig_lines.size()) {
            if (line != orig_lines[line_number]) {
                patch <<line_number + 1 << ": " << line << endl;
            }
        }

        else {
            patch << line_number + 1 << ": " << line << endl;
        }

        line_number++;
    }

    ver1.close();
    ver2.close();
    patch.close();

    cout << "Патч-файл успешно создан" << endl;
}

// Функция проверки патч-файла и создания модифицированного файла
void ConfirmPatch(const string patchFile, const string origFile, const string modFile) {
    ifstream patch(patchFile);
    ifstream ver1(origFile);
    ofstream ver2(modFile);

    vector<string> orig_lines;
    string line;

    while (getline(ver1, line)) {
        orig_lines.push_back(line);
    }

    while (getline(patch, line)) {
        istringstream str_stream(line);
        int line_number = 0;
        string mod_line;

        if (!(str_stream >> line_number) || !getline(str_stream.ignore(2, ' '), mod_line)) {
            cout << "Неверный формат патч-файла: " << line << endl;
            continue;
        }

        if (line_number > orig_lines.size()) {
            orig_lines.push_back(mod_line);
            continue;
        }

        else {
            orig_lines[line_number - 1] = mod_line;
        }
    }

    for (const auto& mod_line : orig_lines) {
        ver2 << mod_line << endl;
    }

    patch.close();
    ver1.close();
    ver2.close();

    cout << "Модифицированный файл успешно создан" << endl;
}

int main() {
    string files_dir = "test_files/";

    string origFile1 = files_dir + "Dir1/ver1.txt";
    string modFile1 = files_dir + "Dir1/ver2.txt";

    string origFile2 = files_dir + "Dir2/ver1.txt";
    string modFile2 = files_dir + "Dir2/ver2.txt";

    string patchFile = files_dir + "patch.txt";

    // Копирование файла версии 1 в другую директорию (поскольку они одинаковые)
    system("copy test_files\\Dir1\\ver1.txt test_files\\Dir2\\ver1.txt");

    if (FilesAreOpened(origFile1, modFile1)) {
        CreatePatchFile(origFile1, modFile1, patchFile);
        ConfirmPatch(patchFile, origFile2, modFile2);
    }

    else {
        cout << "Не удалось открыть файлы" << endl;
    }

    return 0;
}