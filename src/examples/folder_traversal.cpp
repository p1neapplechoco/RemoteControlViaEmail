#include "../utils/GetWinDirectory.h"
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <stack>

// g++ folder_traversal.cpp "../utils/GetWinDirectory.cpp" -o folder_traversal; ./folder_traversal

int main()
{
    system("cls");
    FOLDER folder;
    folder = FOLDER::readCacheFile("cache_D.txt");

    FOLDER *ptr = &folder;
    std::stack<FOLDER *> stk{};

    while (true)
    {
        int i = 0;

        if (!stk.empty())
            std::cout << "Previous folder: " << stk.top()->path << std::endl;
        std::cout << "Current folder: " << ptr->path << std::endl;

        for (; i < ptr->subfolders.size(); i++)
            std::cout << i + 1 << ". " << ptr->subfolders[i].name << std::endl;

        for (auto file : ptr->files)
            std::cout << i++ + 1 << ". " << file.name << std::endl;

        int next_destination = 0;
        std::cin >> next_destination;

        if (next_destination == 0)
        {
            std::cout << "pai pai" << std::endl;
            return 1;
        }
        else if (next_destination == -1)
        {
            if (stk.empty())
                std::cout << "No parent" << std::endl;
            else
            {
                ptr = stk.top();
                stk.pop();
                system("cls");
            }
        }
        else if (next_destination <= ptr->subfolders.size() + ptr->files.size())
        {
            if (next_destination > ptr->subfolders.size())
                openFile(ptr->files[next_destination - ptr->subfolders.size() - 1].path);
            else
            {
                stk.push(ptr);
                ptr = &ptr->subfolders[next_destination - 1];
            }
            system("cls");
        }
    }
}