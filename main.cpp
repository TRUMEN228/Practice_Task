#include <iostream>
#include <fstream>
#include <vector>
#include <random>

using namespace std;

// Функция для создания бинарного патча на основе разницы между версиями ver1 и ver2
vector<char> CreateBinaryPatch(const string& ver1Path, const string& ver2Path)
{
    ifstream ver1File(ver1Path, ios::binary);
    ifstream ver2File(ver2Path, ios::binary);

    if (!ver1File || !ver2File)
    {
        cerr << "Не удалось открыть файлы" << endl;
        return {};
    }

    // Считываем данные из файлов ver1 и ver2
    vector<char> ver1Data(istreambuf_iterator<char>(ver1File), {});
    vector<char> ver2Data(istreambuf_iterator<char>(ver2File), {});

    // Создаем патч на основе разницы между ver2 и ver1
    vector<char> patchData;
    for (size_t i = 0; i < ver2Data.size(); ++i)
    {
        if (i < ver1Data.size())
        {
            char diff = ver2Data[i] - ver1Data[i];
            patchData.push_back(diff);
        }
        else
        {
            patchData.push_back(ver2Data[i]);
        }
    }

    return patchData;
}

// Функция для применения бинарного патча к версии ver1, чтобы получить версию ver2
vector<char> ApplyBinaryPatch(const string& ver1Path, const vector<char>& patchData)
{
    ifstream ver1File(ver1Path, ios::binary);

    if (!ver1File)
    {
        cerr << "Не удалось открыть файлы" << endl;
        return {};
    }

    // Считываем данные из файла ver1
    vector<char> ver1Data(istreambuf_iterator<char>(ver1File), {});

    // Применяем патч к версии ver1, чтобы получить версию ver2
    vector<char> ver2Data;
    for (size_t i = 0; i < ver1Data.size(); ++i)
    {
        if (i < patchData.size())
        {
            char updatedByte = ver1Data[i] + patchData[i];
            ver2Data.push_back(updatedByte);
        }
        else
        {
            ver2Data.push_back(ver1Data[i]);
        }
    }

    return ver2Data;
}

// Функция для создания бинарного файла с случайными данными
void CreateBinaryFile(const string& filePath, size_t fileSize)
{
    ofstream file(filePath, ios::binary);

    if (!file)
    {
        cerr << "Не удалось создать бинарный файл: " << filePath << endl;
        return;
    }

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(0, 255);

    vector<char> data(fileSize);
    for (size_t i = 0; i < fileSize; ++i)
    {
        data[i] = static_cast<char>(distribution(generator));
    }

    file.write(data.data(), fileSize);

    cout << "Бинарный файл создан успешно: " << filePath << endl;
}

//  Функция для обновления размера пропатченного файла на основе размера модифицированного файла
void UpdatePatchedFileSize(const string& patchedFilePath, size_t newSize, const string& originalFilePath)
{
    fstream patchedFile(patchedFilePath, ios::in | ios::out | ios::binary);
    ifstream originalFile(originalFilePath, ios::binary);

    if (!patchedFile || !originalFile)
    {
        cerr << "Не удалось открыть файлы" << endl;
        return;
    }

    patchedFile.seekp(0, ios::end);
    size_t currentSize = patchedFile.tellp();

    if (currentSize < newSize)
    {
        // Считываем оригинальные данные, которые не попали в патч
        vector<char> remainingData(newSize - currentSize);
        originalFile.seekg(currentSize);
        originalFile.read(remainingData.data(), newSize - currentSize);

        // Увеличиваем размер файла путем добавления оставшихся данных в конец
        patchedFile.write(remainingData.data(), remainingData.size());
    }
    else if (currentSize > newSize)
    {
        // Обрезаем файл до нужного размера
        patchedFile.close();

        ofstream truncateFile(patchedFilePath, ios::binary | ios::trunc);
        if (!truncateFile)
        {
            cerr << "Не удалось обрезать файл" << endl;
            return;
        }

        truncateFile.write("", newSize);
        truncateFile.close();
    }

    cout << "Размер файла успешно обновлен" << endl;
}

// Функция для получения размера файла
size_t GetFileSize(const string& filePath)
{
    ifstream file(filePath, ios::binary | ios::ate);
    if (!file)
    {
        cerr << "Не удалось открыть файл" << endl;
        return 0;
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    file.close();

    return fileSize;
}

int main()
{
    string ver1Path = "test_files/Dir1/ver1.bin";
    string ver2Path = "test_files/Dir1/ver2.bin";
    string patchPath = "test_files/patch.patch";

    CreateBinaryFile(ver1Path, 256);
    CreateBinaryFile(ver2Path, 512);

    // Создание патча
    vector<char> patchData = CreateBinaryPatch(ver1Path, ver2Path);

    // Сохранение патча в файл
    ofstream patchFile(patchPath, ios::binary);
    if (!patchFile)
    {
        cerr << "Не удалось создать патч-файл" << endl;
        return 1;
    }

    copy(patchData.begin(), patchData.end(), ostreambuf_iterator<char>(patchFile));

    cout << "Патч создан успешно" << endl;

    // Применение патча к версии ver1, чтобы получить версию ver2
    string patchedFilePath = "test_files/Dir2/patched.bin";

    vector<char> patchedData = ApplyBinaryPatch(ver1Path, patchData);

    // Сохранение версии ver2 в файл
    ofstream patchedFile(patchedFilePath, ios::binary);
    if (!patchedFile)
    {
        cerr << "Не удалось создать пропатченый файл" << endl;
        return 1;
    }
    
    size_t newSize = GetFileSize(ver2Path);

    UpdatePatchedFileSize(patchedFilePath, newSize, ver2Path);

    patchedFile.write(patchedData.data(), patchedData.size());

    cout << "Патч успешно применен" << endl;

    return 0;
}
