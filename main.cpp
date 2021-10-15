#include <iostream>
#include "cmi.h"
#include <string.h>

/* arguement defines*/
enum class OPTIONS
{
    MODE_HEADER_FILE_NAME,
    MODE_IMPLEMENTATON_FILE_NAME,
    MODE_HELP,
    MODE_INVALID_ARG,
    MODE_NO_VERBOSE,
    MODE_NO_INCLUDE_GUARD,
    MODE_NO_INCLUDE,
    MODE_IMPLE_STRUCT
};

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

class application
{
private:
    int m_argc;
    char **m_argv;
    std::string HeaderP, ImpleP;
    std::bitset<16> m_AdditionalOptions;

public:
    static void Start(int argc, char **argv)
    {
        application app(argc, argv);
        app.getInput();
        app.WriteToFile();
    }

private:
    application(int argc, char **argv) : m_argc(argc), m_argv(argv) {}
    void getInput()
    {
        using namespace std;
        if (m_argc == 1)
        {
            FAILED_EXIT();
        }
        else
        {
            for (int i = 1; i < m_argc; i++)
            {
                char *Curr = m_argv[i];
                if (strlen(Curr) > 512 || m_argc > 32)
                {
                    FAILED_EXIT();
                }
                if (!IsArg(Curr))
                {
                    continue;
                }
                std::string SwitchMode;
                std::string Val;

                if (m_argv[i + 1] == nullptr)
                {
                    Val = " ";
                }
                else
                {
                    Val = m_argv[i + 1];
                }
                SwitchMode = (Curr + 1);
                auto code = ProcessArguements(SwitchMode, Val);
                switch (code)
                {
                case OPTIONS::MODE_HEADER_FILE_NAME:
                    HeaderP = Val;
                    break;
                case OPTIONS::MODE_IMPLEMENTATON_FILE_NAME:
                    ImpleP = Val;
                    break;
                case OPTIONS::MODE_NO_INCLUDE_GUARD:
                    m_AdditionalOptions[NO_INCLUDE_GUARD] = 1;
                    break;
                case OPTIONS::MODE_NO_VERBOSE:
                    m_AdditionalOptions[NO_VERB] = 1;
                    break;
                case OPTIONS::MODE_NO_INCLUDE:
                    m_AdditionalOptions[NO_INCLUDE] = 1;
                    break;
                case OPTIONS::MODE_HELP:
                    cout << HELPERMSG;
                    SUCCESS_EXIT();
                    break;
                case OPTIONS::MODE_IMPLE_STRUCT:
                    m_AdditionalOptions[IMPLE_STRUCT] = 1;
                    break;
                default:
                    FAILED_EXIT();
                    break;
                }
            }
        }
    }
    void WriteToFile()
    {
        auto file = utils::ReadFile(HeaderP);
        auto lines = utils::TokeniseStringToLines(file);

        std::vector<std::string> FormattedFunc;
        CMI cmi(lines, m_AdditionalOptions);

        auto pfs = cmi.GetParsedFunctions();
        if (!m_AdditionalOptions[NO_VERB])
        {
            std::cout << "Detected functions:\n";

            int i = 1;

            for (auto &function : pfs)
            {
                std::stringstream ss;

                ss << function.Rtype << " "
                   << function.ClassName << "::" << function.Name << "(" << function.Args << ")"
                   << (function.IsConst ? " const" : "");
                std::cout << i << ". " << ss.str() << '\n';

                FormattedFunc.push_back(ss.str());

                i++;
            }
        }
        cmi.GenerateCMI(FormattedFunc, ImpleP, HeaderP);
    }
    bool IsArg(char *str)
    {
        if (str[0] == '-')
            return true;
        else
            return false;
    }

    OPTIONS ProcessArguements(const std::string &mode, const std::string &value)
    {
        if (mode == "help")
        {
            return OPTIONS::MODE_HELP;
        }

        if (value[0] == '-')
        {
            FAILED_EXIT();
        }
        if (mode == "header")
        {
            return OPTIONS::MODE_HEADER_FILE_NAME;
        }
        if (mode == "imple")
        {
            return OPTIONS::MODE_IMPLEMENTATON_FILE_NAME;
        }
        if (mode == "novb")
        {
            return OPTIONS::MODE_NO_VERBOSE;
        }
        if (mode == "noguard")
        {
            return OPTIONS::MODE_NO_INCLUDE_GUARD;
        }
        if (mode == "noinclude")
        {
            return OPTIONS::MODE_NO_INCLUDE;
        }
        if (mode == "implestruct")
        {
            return OPTIONS::MODE_IMPLE_STRUCT;
        }
        else
        {
            return OPTIONS::MODE_INVALID_ARG;
        }
    }
};
int main(int argc, char **argv) { application::Start(argc, argv); }
