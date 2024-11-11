#include "IniParser.h"
#include <fstream>

std::string IniParser::get(const std::string &section, const std::string &key)
{
    return data.at(section).at(key);
}

void IniParser::parseFile(const std::string &filename)
{
    std::ifstream infile(filename);
    std::string line, currentSection;

    while (std::getline(infile, line))
    {
        // Remove whitespaces from beginning and end
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip comments and empty lines
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        // Section
        if (line.front() == '[' && line.back() == ']')
        {
            currentSection = line.substr(1, line.size() - 2);
        }
        // Key-value pair
        else
        {
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos)
            {
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);
                data[currentSection][key] = value;
            }
        }
    }
}

