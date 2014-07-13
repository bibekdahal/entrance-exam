#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <map>
#include <iomanip>
#include <vector>

struct AnswerInfo
{
    std::string roll;
    int correctanswers;
    int incorrectanswers;
};

int g_maxnamewidth = 0;

std::map<std::string, AnswerInfo> g_answers;

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

    if ((int)strlen(name) > g_maxnamewidth) g_maxnamewidth = strlen(name);
    AnswerInfo info = { username, correctanswers, incorrectanswers };
    auto inf = g_answers.find(name);
    if (inf != g_answers.end())
    {
        if (inf->second.correctanswers + inf->second.incorrectanswers < correctanswers + incorrectanswers)
            g_answers[name] = info;
    }
    else
        g_answers[name] = info;

    /*g_text << "\tRoll No.: " << username << "\n";
    g_text << "\tName: " << name << "\n";
    g_text << "\t\tCorrect Answers: " << correctanswers << "\n";
    g_text << "\t\tIncorrect Answers: " << incorrectanswers << "\n\n";*/

    delete[] username;
    delete[] name;
}

void OpenFiles()
{
    std::vector<std::wstring> folders = { L"shift1", L"shift2", L"shift3", L"Shift4" };
    for (unsigned i = 0; i < folders.size(); ++i)
    {
        WIN32_FIND_DATA FindFileData;
        std::wstring fname = L"Answers/" + folders[i] + L"/*.fba";
        HANDLE hFind = FindFirstFile(fname.c_str(), &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
            return;
        else do{
            ProcessFile(L"Answers/" + folders[i] + L"/" + FindFileData.cFileName);
        } while (FindNextFile(hFind, &FindFileData));
        FindClose(hFind);
    }
    return;
}

int main()
{
    OpenFiles();

    g_text
        << std::left << std::setw(g_maxnamewidth) << "\tName" << "\tRoll No.\tCorrect Answers\tIncorrect Answers\n"
        << std::left << std::setw(g_maxnamewidth) << "\t______" << "\t_________\t_______________\t_________________\n";

    for (auto it = g_answers.begin(); it != g_answers.end(); ++it)
    {
        if (it->first != "")
        g_text << std::left << "\t" << std::setw(g_maxnamewidth) << it->first << "\t" << std::setw(9) << std::right << it->second.roll << "\t"
            << std::setw(15) << it->second.correctanswers << "\t" << std::setw(17) << it->second.incorrectanswers << "\n";
    }


    std::fstream ofile;
    ofile.open("Result.txt", std::ios::out);
    ofile << g_text.str();
    ofile.close();

   
   // std::cout << g_text.str();
    return 0;
}