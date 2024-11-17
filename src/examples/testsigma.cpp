#include "../utils/GetWinDirectory.h"
#include <dirent.h>
#include <iostream>
#include <deque>
#include <stack>
#include <unistd.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>

// g++ testsigma.cpp "../utils/GetWinDirectory.cpp" -o testsigma; ./testsigma

int main()
{
    std::ifstream inputFile("cache_D_demo.txt");
    if (!inputFile.is_open())
        return 1;

    std::string line;
    FOLDER rootFolder;
    std::stack<std::pair<FOLDER *, size_t>> folder_stack;

    while (std::getline(inputFile, line))
    {
        size_t current_indent = line.find_first_not_of(' ');
        std::string name_of = line.substr(current_indent);
        if (current_indent == 0)
        {
            rootFolder.path = name_of;
            rootFolder.name = name_of;
            folder_stack.push({&rootFolder, current_indent});
            continue;
        }

        while (!folder_stack.empty() && current_indent <= folder_stack.top().second && current_indent > 0)
            folder_stack.pop();

        FOLDER *ptr = folder_stack.top().first;

        if (name_of.back() == '\\')
        {
            // std::cout << ptr->path << std::endl;
            FOLDER new_folder;
            new_folder.parent = ptr;
            new_folder.name = name_of;
            // std::cout << new_folder.name << "   " << new_folder.parent->path << std::endl;

            new_folder.path = ptr->path + name_of;

            ptr->subfolders.push_back(new_folder);
            folder_stack.push({&ptr->subfolders.back(), current_indent});
        }
        else
        {
            FILE_ new_file;
            new_file.path = ptr->path + name_of;
            new_file.name = name_of;

            ptr->files.push_back(new_file);
        }
    }
    inputFile.close();

    printFolderStructure(rootFolder);
}