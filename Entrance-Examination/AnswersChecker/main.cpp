#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

std::stringstream g_text("");
void ProcessFile(std::wstring filename)
{
    std::fstream file;
    file.open(filename, std::ios::in | std::ios::binary);
    int setid;
    file.read((char*)&setid, sizeof(setid));

    std::fstream afile;
    std::stringstream afname(""); afname << "set" << setid << ".txt";
    afile.open(afname.str().c_str(), std::ios::in);
    std::string cans;
    afile >> cans;
    afile.close();

    int correctanswers = 0;
    int incorrectanswers = 0;

    unsigned int size;
    file.read((char*)&size, sizeof(size));
    char* username = new char[size + 1]; username[size] = '\0';
    file.read(username, size);

    file.read((char*)&size, sizeof(size));
    char* name = new char[size + 1]; name[size] = '\0';
    file.read(name, size);

    file.read((char*)&size, sizeof(size));
    for (unsigned int i = 0; i < size; ++i)
    {
        char ans;
        file.read(&ans, sizeof(ans));
        if (ans == cans[i] - 'a') correctanswers++;
        else if (ans != -1) incorrectanswers++;
    }
    file.close();
    file.close();

    g_text << "\tRoll No.: " << username << "\n";
    g_text << "\tName: " << name << "\n";
    g_text << "\t\tCorrect Answers: " << correctanswers << "\n";
    g_text << "\t\tIncorrect Answers: " << incorrectanswers << "\n\n";

    delete[] username;
    delete[] name;
}

void OpenFiles()
{
    WIN32_FIND_DATA FindFileData;
    std::wstring fname = L"Answers/*.fba";
    HANDLE hFind = FindFirstFile(fname.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
        return;
    else do{
        ProcessFile(L"Answers/" + std::wstring(FindFileData.cFileName));
    } while (FindNextFile(hFind, &FindFileData));
    FindClose(hFind);
    return;
}

int main()
{
    OpenFiles();


    std::fstream ofile;
    ofile.open("Result.txt", std::ios::out);
    ofile << g_text.str();
    ofile.close();

    std::cout << g_text.str();
    return 0;
}