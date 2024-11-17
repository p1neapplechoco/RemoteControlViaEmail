#include "GetWinDirectory.h"
#include <dirent.h>
#include <iostream>
#include <stack>
#include <unistd.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>


void printFolderStructure(const FOLDER &folder, const std::string &indent)
{
    Sleep(100);
    std::cout << indent << "Path: " << folder.path << std::endl;

    for (const auto &subfolder: folder.subfolders)
        printFolderStructure(subfolder, indent + "    ");

    for (const auto &file: folder.files)
    {
        std::cout << indent << "    " << "Name: " << file.name << std::endl;
        std::cout << indent << "    " << "Path: " << file.path << std::endl;
    }
}

FOLDER FOLDER::readCacheFile(const std::string &cacheFilePath)
{
    std::ifstream inputFile(cacheFilePath);
    if (!inputFile.is_open())
        throw std::runtime_error("Failed to open cache file: " + cacheFilePath);

    std::string line;
    FOLDER root_folder;
    std::stack<std::pair<FOLDER *, size_t> > folder_stack;

    while (getline(inputFile, line))
    {
        size_t current_indent = line.find_first_not_of(' ');
        std::string name_of = line.substr(current_indent);

        if (current_indent == 0)
        {
            name_of.pop_back();
            root_folder.path = name_of;
            root_folder.name = name_of;
            folder_stack.emplace(&root_folder, current_indent);
            continue;
        }
        while (!folder_stack.empty() && current_indent <= folder_stack.top().second)
            folder_stack.pop();

        FOLDER *ptr = folder_stack.top().first;
        if (name_of.back() == '\\')
        {
            FOLDER new_folder;
            new_folder.path = ptr->path + name_of;
            new_folder.name = name_of;
            ptr->subfolders.push_back(new_folder);
            folder_stack.emplace(&ptr->subfolders.back(), current_indent);
        } else
        {
            FILE_ new_file;
            new_file.name = name_of;
            new_file.path = ptr->path + name_of;
            ptr->files.emplace_back(new_file);
        }
    }
    inputFile.close();
    return root_folder;
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

void fullScan(const std::string &pathToScan, std::vector<std::string> &buffer, const std::string &indent)
{
    DIR *dir = opendir(pathToScan.c_str());
    if (dir == nullptr) return;
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        std::string fullPath = pathToScan + "\\" + name;
        const DWORD attrs = GetFileAttributesA(fullPath.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_HIDDEN)) continue;
        struct stat info;
        if (stat(fullPath.c_str(), &info) != 0) continue;
        if (S_ISDIR(info.st_mode))
        {
            buffer.push_back(indent + name + "\\");
            fullScan(fullPath, buffer, indent + " ");
        } else if (name.find(".txt") != std::string::npos || name.find(".exe") != std::string::npos) buffer.push_back(indent + name);
    }
    closedir(dir);
}

bool openFile(const std::string &file_path)
{
    if (file_path.size() > 4 && file_path.substr(file_path.size() - 4) == ".exe")
    {
        std::cout << "Opening .exe file: " << file_path << std::endl;
        ShellExecuteA(nullptr, "open", file_path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    } else if (file_path.size() > 4 && file_path.substr(file_path.size() - 4) == ".txt")
    {
        std::cout << "Opening .txt file: " << file_path << std::endl;
        ShellExecuteA(nullptr, "open", "notepad.exe", file_path.c_str(), nullptr, SW_SHOWNORMAL);
    } else
    {
        return false;
    }
    return true;
}
