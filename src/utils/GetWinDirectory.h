#pragma once

#ifndef GETWINDIRECTORY_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#define GETWINDIRECTORY_H

class FILE_;
class FOLDER;

bool openFile(const std::string &file_path);
void printFolderStructure(const FOLDER &folder, const std::string &indent = "");

class FILE_
{
public:
    std::string name;
    std::string path;

    FILE_()
    {
        this->path = "";
        this->name = "";
    }

    explicit FILE_(const std::string &path)
    {
        this->path = path;
    }
};

class FOLDER
{
public:
    std::string name;
    std::string path;
    std::vector<FILE_> files;
    std::vector<FOLDER> subfolders;

    FOLDER()
    {
        this->path = "";
        this->name = "";
        this->files = {};
        this->subfolders = {};
    }

    explicit FOLDER(const std::string &path)
    {
        this->path = path;
    }

    static FOLDER readCacheFile(const std::string &cacheFilePath);
};

class GetWinDirectory
{
public:
    std::vector<std::string> disks;

    std::vector<std::string> listDisks();

    static void scanOneLevel(const std::string &pathToScan);

    static void fullScan(const std::string &pathToScan, std::ofstream &outputFile, const std::string &indent = "    ");
};

#endif // GETWINDIRECTORY_H
