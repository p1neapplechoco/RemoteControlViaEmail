#pragma once
#ifndef INIPARSER_H

#include <map>
#include <string>

#define INIPARSER_H

class IniParser
{
public:
    explicit IniParser(const std::string &filename)
    {
        parseFile(filename);
    }

    ~IniParser();

    std::string get(const std::string &section, const std::string &key);

private:
    std::map<std::string, std::map<std::string, std::string> > data;

    void parseFile(const std::string &filename);
};

#endif //INIPARSER_H
