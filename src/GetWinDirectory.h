#pragma once

#ifndef GETWINDIRECTORY_H

#include <fstream>
#include <string>
#include <vector>


#define GETWINDIRECTORY_H

class FILE_
{
public:
    std::string path;

    explicit FILE_(const std::string &path)
    {
        this->path = path;
    }
};

class FOLDER
{
public:
    std::string path;
    std::vector<FILE_> files;
    std::vector<FOLDER> subfolders;

    FOLDER()
    {
        this->path = "";
        this->files = {};
        this->subfolders = {};
    }

    explicit FOLDER(const std::string &path)
    {
        this->path = path;
    }

    static FOLDER readCacheFile(const std::string &cacheFilePath);

    void printFolderStructure(const std::string &indent = "");
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
