#pragma once
//includes
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <regex>

//defines
#define USE_CLASS_NAME "ucn"
#define NO_VERB 0

#define NO_INCLUDE 2
#define IMPLE_STRUCT 3


/* ~~~~~~~~~~~~~~~~~~~~~~~*/

/*Messeges*/
const std::string INVALIDMSG = "invalid argument , refer -help for more info!\n";

const std::string HELPERMSG =
    "#### HELP ####\n"
    "-header : takes header file name\n-imple : takes implementation file name\n "
    "usage: cmi -header <filename> -imple <filename> <additional options>\n"
    "additional options:"
    "\n-noguard -> disables include guard"
    "\n-noinclude -> doesn't include header file"
    "\n#### END ####\n";
/*~~~~~~~~~~~~~~~~~~~~~~~~*/


/*Unsuccesfull Run define*/
#define FAILED_EXIT()        \
    std::cout << INVALIDMSG; \
    exit(1)
#define SUCCESS_EXIT() \
    exit(0)
using line = std::string;
const std::string FUNCTION_SIGNATURE_REG = "\\s*(\\w*)\\s*(\\w*)\\((.*)\\)\\s*(const)?;";

namespace utils
{
    std::string ReadFile(const std::string &filepath)
    {
        std::ifstream file;
        file.open(filepath);

        if (!file.is_open())
            {
             throw std::runtime_error("no file found / error opening file!!\n");
             FAILED_EXIT();
            }

        std::stringstream ss;

        while (!file.eof())
        {
            std::string temp;
            std::getline(file, temp);
            ss << temp << '\n';
        }
        return ss.str();
    }

    std::vector<line> TokeniseStringToLines(const std::string &str)
    {
        std::vector<line> lines;
        std::string temp;

        for (char c : str)
        {
            if (c == '\n')
            {
                lines.push_back(temp);
                temp = "";
                continue;
            }
            temp.push_back(c);
        }

        return lines;
    }
}
struct CppFunction
{
    std::string Rtype;
    std::string Name;
    std::string Args;
    std::string ClassName;
    bool IsConst;
};
struct CppClass
{
    std::string Name;
    CppClass(const std::string &_classname) { Name = _classname; }
    CppClass() {}
};

class CMI
{
private:
    CppClass m_class;
    std::vector<CppFunction> m_functions;
    std::vector<line> m_lines;
    std::bitset<16> addop;

public:
    CMI(const std::vector<line> &_lines, std::bitset<16> &_addop)
    {
        addop = _addop;
        m_lines = _lines;
        ParseLines();
    }
    std::vector<CppFunction> GetParsedFunctions() const
    {
        return m_functions;
    }
    auto GetClassName() { return m_class.Name; }

    void GenerateCMI(const std::vector<std::string> &FormattedFunc, std::string DestFile, std::string HeaderFile = USE_CLASS_NAME)
    {
        std::ofstream write;
        write.open(DestFile, std::ofstream::out | std::ofstream::trunc);
        std::string include = "#include";
        char quote = '"';
        if (HeaderFile == USE_CLASS_NAME)
        {
            HeaderFile = m_class.Name;
        }
        else
        {
            HeaderFile = HeaderFile.substr(0, HeaderFile.size() - 2);
        }

        const std::string doth = ".h";
        
        if (!addop[NO_INCLUDE])
        {
            write << include << quote << HeaderFile << doth << quote << "\n\n";
        }

        for (auto function : FormattedFunc)
        {
            write << function
                  << "\n{\n\n}\n";
        }
        write.close();
    }

private:
    void ParseLines()
    {
        //check if class is present in file
        bool ClassPresent = false;
        std::string CurrentClass;

        for (line &l : m_lines)
        {
            std::stringstream ss;
            if ((l.find("class") != std::string::npos) || (addop[IMPLE_STRUCT] && (l.find("struct") != std::string::npos)))
            {
                ClassPresent = true;
                std::string KeywordClass, ClassName;

                ss << l;
                ss >> KeywordClass >> ClassName;

                m_class = ClassName;
                CurrentClass = ClassName;
            }
            else
            {
                if (l.find('#') == std::string::npos)
                {
                    const std::regex reg(FUNCTION_SIGNATURE_REG, std::regex::ECMAScript);
                    std::smatch match;
                    if (std::regex_search(l, match, reg) == true)
                    {
                        std::string type, name, args;
                        bool IsConst = false;

                        type = match[1];
                        name = match[2];
                        args = match[3];

                        IsConst = match[4] == "const" ? true : false;

                        m_functions.push_back({type, name, args, CurrentClass, IsConst});
                    }
                }
            }
        }
        if (!ClassPresent)
        {
            std::cout << "class not present in file\n";
            exit(1);
        }
    }
};
