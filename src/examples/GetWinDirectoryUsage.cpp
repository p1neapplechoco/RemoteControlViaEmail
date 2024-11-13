#include "../utils/GetWinDirectory.h"
#include <iostream>

void fullScanDemo();
void readCacheFileDemo();

// g++ GetWinDirectoryUsage.cpp "../utils/GetWinDirectory.cpp" -o GetWinDirectoryUsage; ./GetWinDirectoryUsage

int main()
{
    fullScanDemo();
    // readCacheFileDemo();
}

void fullScanDemo()
{
    GetWinDirectory getWinDir;

    getWinDir.listDisks();
    std::cout << "Available Disks: " << std::endl;

    for (const auto &disk : getWinDir.disks)
        std::cout << disk << std::endl;

    for (const auto &disk : getWinDir.disks)
    {
        std::string fileName = "cache_" + std::string(1, disk[0]) + ".txt";
        std::ofstream file(fileName);

        if (!file.is_open())
        {
            std::cerr << "Error opening file " << fileName << std::endl;
            continue;
        }

        file << disk << '\\' << std::endl;
        GetWinDirectory::fullScan(disk, file);
        file.close();

        std::cout << "Full scan of " << disk << " has been written to " << fileName << std::endl;
    }
}

void readCacheFileDemo()
{
    std::vector<std::string> diskCacheFiles = {"cache_D_demo.txt"};

    std::vector<FOLDER> rootFolders;

    for (const auto &fileName : diskCacheFiles)
    {
        try
        {
            FOLDER recreatedFolder = FOLDER::readCacheFile(fileName);
            rootFolders.push_back(recreatedFolder);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    for (auto &disk : rootFolders)
    {
        std::cout << "Path of " << disk.path << "'s subfolders: " << std::endl;
        printFolderStructure(disk);
    }
}
