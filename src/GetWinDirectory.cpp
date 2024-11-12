#include "GetWinDirectory.h"
#include <dirent.h>
#include <iostream>
#include <stack>
#include <unistd.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>

void FOLDER::printFolderStructure(const std::string &indent)
{
    std::cout << indent << path << std::endl;
    for (const auto &file: files)
        std::cout << indent << "    " << file.path << std::endl;

    for (auto &subfolder: subfolders)
        subfolder.printFolderStructure(indent + "    ");
}

FOLDER FOLDER::readCacheFile(const std::string &cacheFilePath)
{
    std::ifstream inputFile(cacheFilePath);
    if (!inputFile.is_open())
        throw std::runtime_error("Failed to open cache file: " + cacheFilePath);

    std::string line;
    FOLDER rootFolder;
    std::stack<std::pair<FOLDER *, size_t> > folderStack;
    size_t previousIndent = 0;

    while (getline(inputFile, line))
    {
        size_t currentIndent = line.find_first_not_of(' ');
        std::string trimmedLine = line.substr(currentIndent);

        if (currentIndent == 0)
        {
            rootFolder.path = trimmedLine;
            folderStack.emplace(&rootFolder, currentIndent);
        } else
        {
            while (!folderStack.empty() && currentIndent <= folderStack.top().second)
                folderStack.pop();

            FOLDER *parentFolder = folderStack.top().first;
            if (trimmedLine.back() == '\\')
            {
                trimmedLine.pop_back();
                std::string folderPath = parentFolder->path + "\\" + trimmedLine;
                FOLDER newFolder(folderPath);
                parentFolder->subfolders.push_back(newFolder);
                folderStack.emplace(&parentFolder->subfolders.back(), currentIndent);
            } else
            {
                std::string filePath = parentFolder->path + "\\" + trimmedLine;
                parentFolder->files.emplace_back(filePath);
            }
        }
    }

    inputFile.close();
    return rootFolder;
}

std::vector<std::string> GetWinDirectory::listDisks()
{
    char buffer[128];
    DWORD size = GetLogicalDriveStringsA(sizeof(buffer) - 1, buffer);

    for (const char *drive = buffer; *drive; drive += 4)
        disks.emplace_back(drive);
    return disks;
}

void GetWinDirectory::scanOneLevel(const std::string &pathToScan)
{
    DIR *dir = opendir(pathToScan.c_str());
    if (dir == nullptr)
        return;

    dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;

        std::string fullPath = pathToScan + "\\" + name;
        const DWORD attrs = GetFileAttributesA(fullPath.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_HIDDEN))
            continue;

        struct stat info
        {
        };
        if (stat(fullPath.c_str(), &info) != 0)
            continue;

        if (S_ISDIR(info.st_mode))
            std::cout << name << "\\" << std::endl;
        else if (name.find(".txt") != std::string::npos || name.find(".exe") != std::string::npos)
            std::cout << name << std::endl;
    }

    closedir(dir);
}

void GetWinDirectory::fullScan(const std::string &pathToScan, std::ofstream &outputFile, const std::string &indent)
{
    DIR *dir = opendir(pathToScan.c_str());
    if (dir == nullptr)
        return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;

        std::string fullPath = pathToScan + "\\" + name;
        const DWORD attrs = GetFileAttributesA(fullPath.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_HIDDEN))
            continue;

        struct stat info
        {
        };
        if (stat(fullPath.c_str(), &info) != 0)
            continue;

        if (S_ISDIR(info.st_mode))
        {
            outputFile << indent << name << "\\" << std::endl;
            fullScan(fullPath, outputFile, indent + "    ");
        } else if (name.find(".txt") != std::string::npos || name.find(".exe") != std::string::npos)
            outputFile << indent << name << std::endl;
    }

    closedir(dir);
}
