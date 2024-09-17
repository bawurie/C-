#include "ExpressionParser.h"
#include <assert.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <cmath>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;
// Print iterators in a generic way

template<typename T, typename InputIterator>

void Print( const std::string& message,

const InputIterator& itbegin,

const InputIterator& itend,

const std::string& delimiter)
{
    std::cout << message << std::endl;
    std::copy(itbegin, itend,
    std::ostream_iterator<T>(std::cout, delimiter.c_str()));
    std::cout << std::endl;
}

double string_to_double( const std::string& s )
{
    std::istringstream i(s);
    double x;
    if (!(i >> x))
        return 0;
    return x;

}

class Calculator
{

    public:
        char assignmentOperator = '=';
        std::map<std::string, double> variables;
        int currentLine = 1;
        int currentLineCol = 1;
        string result = "";

    void defineVariable(string name, double value)
    {
        variables[name] = value;
    }

    double getVariable(string name)
    {
        //cout<<endl<<"-----------getVariable =>"<<name<<endl;
        map<std::string, double>::iterator it = variables.find(name);
        double value = 0;
        if(it != variables.end())
        {
           value = it->second;
        }
        else
        {
            //Throw error since variable doesn't exist
            return -1;
        }
        return value;
    }

    void printAllVariables()
    {
        cout<<endl<<"------ All variables converted to Double ;) ------"<<endl;
        map<string, double>::iterator it;

        for ( it = variables.begin(); it != variables.end(); it++ )
        {
            std::cout << it->first  // string (key)
                      << ':'
                      << it->second   // string's value
                      << std::endl ;
        }
    }

    double convertValue(string value, bool checkIfVariable)
    {
        std::string valueString(value);

        if (valueString.find("0x") == 0) //starts with 0x and is a hex value
        {
            //Hexi-decimal to integer
            //cout<<"hex";
            char * p;
            double n = strtold( value.c_str(), & p);
            if ( * p != 0 )
            {
                return 0;
            }
            else
            {
                return n;
            }
        }
        else if (valueString.find("b") == valueString.length()-1 && valueString.length() > 1) //ends with b and is a binary value
        {
            //cout<<"binary";
            //Remove the ending b char
            valueString = valueString.substr(0, valueString.size()-1);

            double _value = 0;
            int indexCounter = 0;
            for(int i=valueString.length()-1;i>=0;i--)
            {
                if(valueString[i]=='1')
                {
                    _value += pow(2, indexCounter);
                }
                indexCounter++;
            }

            return _value;
        }
        else if(checkIfVariable && getVariable(valueString) != -1)
        {
            //cout<<valueString<<' : '<<getVariable(valueString);
            return getVariable(valueString);
        }
        else if(is_number(valueString))
        {
            double n = strtod(value.c_str(), NULL);
            return n;
        }
        else
        {
            return -1;
        }
    }

    void ReplaceStringInPlace(std::string& subject, const std::string& search,
                              const std::string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
             subject.replace(pos, search.length(), replace);
             pos += replace.length();
        }
    }

    void readFile()
    {
        string line;
        ifstream out("inputFileName.txt");
        while(getline(out, line))
        {
            parseLine(line);
            currentLine++;
        }
        out.close();

        writeFile();

    }

    void writeFile(){
        std::ofstream out("outputFileName.txt");
        out<<"output";
        out<<result;
        out.close();
    }

    void parseLine(string line)
    {
        if (line.find(assignmentOperator) != string::npos)
        {
            //Declaration
            parseDeclaration(line);
        }
        else
        {
            //Expression
            parseExpression(line);
        }
    }

    void parseDeclaration(string line)
    {
        //Read from left to right
        //Put characters in a temporary variable
        //When encounter =, the previous one is variable name
        //Skip '='
        //Next token is value, convert value to meaningful value using convertValue();
        //At this point you have both variable name and its value
        //Declare the variable. Done

        currentLineCol = 0;
        std::string varibleName = "";
        double varibleValue;
        std::string temp = "";

        //std::string str = ???;
        for(std::string::size_type i = 0; i < line.size(); ++i)
        {
            char token = line[i];
            //cout<<token;

            if(token == ' ')
            {
                continue;
            }
            else if(token == assignmentOperator)
            {
                if(temp == "")
                {
                    //Error, line cannot start with =
                }
                else
                {
                    varibleName = temp;
                    //reset temp
                    temp = "";
                }
            }
            else
            {
                temp += token;
                //cout;
            }
            currentLineCol++;
        }
        //cout<<temp<<endl;
        varibleValue = convertValue(temp, true);
        if(varibleValue == -1)
        {
            //Invalid value found
            std::stringstream sstm;
            sstm << "unable to evaluate value [" << temp << "] line #" << currentLine << "," << currentLineCol;
            throw std::invalid_argument( sstm.str() );
            return;

        }
        //cout<<varibleName<<"=>"<<varibleValue<<endl;
        defineVariable(varibleName, varibleValue);
    }

    void parseExpression(string expression)
    {
        if(expression == "" || expression == " " || expression == "----") return;
        std::string str_result;

        ReplaceHexOperandValues(expression);
        ReplaceBinaryOperandValues(expression);
        ReplaceVariableOperands(expression);
        WrapPowerOf(expression);
        //Expression handling
        ExpressionParser parser( expression );
        if(!parser.MatchingParetheses())
        {
            str_result = "Error: mismatched parentheses or empty input";
        }

        std::vector<std::string> RPN;

        if ( parser.InfixToRPN( RPN ) )
        {
            //Print<std::string, std::vector<std::string>::const_iterator>("RPN tokens: ", RPN.begin(), RPN.end(),  " " );
            if ( parser.Evaluate( RPN, str_result ) )
            {
                //str_result = str_result;
                //std::cout << std::endl;
                //double result = string_to_double( str_result );

                // Compare expected vs actual to within a precision: 1

                //assert( fabs(result-expected_result) < 0.001 );
            }
        }
        else
        {
            //std::cout << "Error: mismatched parentheses" << std::endl;
            str_result = "Error: mismatched parentheses";
        }

        result = result + str_result  + "\n----\n";;
        //expression + "\n" +
        //std::cout<<expression<<std::endl;
        //cout<<str_result;
        //return str_result;
    }

    void ReplaceVariableOperands(std::string& expression)
    {
        map<string, double>::iterator it;
        bool return1 = false;
        for ( it = variables.begin(); it != variables.end(); it++ )
        {
            std::ostringstream os;
            os << it->second;
            std::string value = os.str();

            std::ostringstream os1;
            os1 << it->first;
            std::string name = os1.str();

            size_t pos = expression.find('/', 0);
            if(pos == std::string::npos)
            {
                pos = expression.find('*', 0);
                if(pos == std::string::npos)
                {
                    pos = expression.find('+', 0);
                    if(pos == std::string::npos)
                    {
                        pos = expression.find('-', 0);
                        if(pos == std::string::npos)
                        {
                            pos = expression.find('^', 0);
                            if(pos == std::string::npos)
                            {
                                pos = expression.find('sin', 0);
                                if(pos == std::string::npos)
                                {
                                    pos = expression.find('cos', 0);
                                    if(pos == std::string::npos)
                                    {
                                        ReplaceStringInPlace(expression, name, value);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if(return1) return;

            size_t pos1 = expression.find(name, 0);
            //cout<<name<<" : "<<pos1<<endl;
            if(pos1 == 0)
            {
                ReplaceStringInPlace(expression, name, value);
                continue;
            }

            ReplaceStringInPlace(expression, ' ' + name, ' '+value); ReplaceStringInPlace(expression, ' '+name + ' ', value+ ' ' );
            ReplaceStringInPlace(expression, '/' + name, '/'+value); ReplaceStringInPlace(expression, name + '/', value+ '/' );
            ReplaceStringInPlace(expression, '*' + name, '*'+value); ReplaceStringInPlace(expression, name + '*', value+ '*');
            ReplaceStringInPlace(expression, '+' + name, '+'+value); ReplaceStringInPlace(expression, name + '+', value+ '+');
            ReplaceStringInPlace(expression, '-' + name, '-'+value); ReplaceStringInPlace(expression, name + '-', value+ '-');
            ReplaceStringInPlace(expression, '*' + name, '*'+value); ReplaceStringInPlace(expression, name + '*', value+ '*');
            ReplaceStringInPlace(expression, '^' + name, '^'+value); ReplaceStringInPlace(expression, name + '^', value+ '^');
            ReplaceStringInPlace(expression, '(' + name, '('+value); ReplaceStringInPlace(expression, name + '(', value+ '(');
            ReplaceStringInPlace(expression, ')' + name, ')'+value); ReplaceStringInPlace(expression, name + ')', value+ ')');
        }
    }
    void WrapPowerOf(std::string& expression)
    {
        std::string sub = "^"; // str is string to search, sub is the substring to search for
        bool leftadded = false;
        //1. Find starting positions of "0x"
        std::vector<std::string> powers; // holds hex operands
        size_t pos = expression.find(sub, 0);
        while(pos != std::string::npos)
        {
            //std::cout<<pos<<std::endl;
            //positions.push_back(pos);
           for(int j = pos-1; j >= 0; j--)
           {
                //std::cout<<j<<std::endl;
                int index = static_cast<int>(j);
                char current = expression[index];
                if(current == '+' || current == '-' || current == '*' || current == '(' || current == ')' || current == '^')
                {
                    expression.insert(j+1, 1, '(');
                    leftadded = true;
                    break;
                }
            }

            if(leftadded)
                pos = expression.find(sub,pos+2);
            else
                pos = expression.find(sub,pos+1);
        }
        bool rightadded = false;
        pos = expression.find(sub, 0);
        while(pos != std::string::npos)
        {
            //std::cout<<pos<<std::endl;
            std::string binary = "";

            for(std::string::size_type j = pos+1; j < expression.size(); j++)
            {
                int index = static_cast<int>(j);
                //std::cout<<"INDEX::"<<j<<std::endl;
                char current = expression[index];
                if(current == '+' || current == '-' || current == '/' || current == '*' || current == '(' || current == ')' || current == '^')
                {
                    expression.insert(j, 1, ')');
                    rightadded = true;
                    break;
                }
            }

            pos = expression.find(sub,pos+1);
        }
        if(!rightadded && leftadded)
        {
            expression.insert(expression.size(), 1, ')');
        }

    }
    void ReplaceHexOperandValues(std::string& expression)
    {
        std::string sub = "0x"; // str is string to search, sub is the substring to search for

        //1. Find starting positions of "0x"
        std::vector<std::string> hexcadecimals; // holds hex operands
        size_t pos = expression.find(sub, 0);
        while(pos != std::string::npos)
        {
            //std::cout<<pos<<std::endl;
            //positions.push_back(pos);
            std::string hex = "";

            for(std::string::size_type j = pos; j < expression.size(); j++) {
                char current = expression[j];
                if(current == ' ' || current == '+' || current == '-' || current == '/' || current == '*' || current == '(' || current == ')' || current == '^')
                {
                    break;
                }
                else
                {
                    hex = hex + current;
                }
            }
            hexcadecimals.push_back(hex);
            hex = "";
            pos = expression.find(sub,pos+1);
        }

        //1. For all starting positions parse forward until operator|parenthesis|end is encountered
        for (int i = 0; i < hexcadecimals.size(); i++)
        {
            std::string hex = hexcadecimals[i];
            //std::cout<<binary<<std::endl;
            double value = convertValue(hex, false);

            std::ostringstream os;
            os << value;
            std::string str = os.str();

            ReplaceStringInPlace(expression, hex, str);
        }
    }

    void ReplaceBinaryOperandValues(std::string& expression)
    {
        std::string sub = "0b";

        //1. Find starting positions of "0b"
        std::vector<std::string> binaries; // holds binary operands
        size_t pos = expression.find(sub, 0);
        while(pos != std::string::npos)
        {
            //std::cout<<pos<<std::endl;
            std::string binary = "";

            for(int j = pos+1; j >= 0; --j) {
                int index = static_cast<int>(j);
                //std::cout<<"INDEX::"<<j<<std::endl;
                char current = expression[index];
                if(current == ' ' || current == '+' || current == '-' || current == '/' || current == '*' || current == '(' || current == ')' || current == '^')
                {
                    break;
                }
                else
                {
                    if(binary != "")
                        binary = std::string(1, current) + binary;
                    else
                        binary = std::string(1, current);
                }
            }
            //std::cout<<binary<<std::endl;
            binaries.push_back(binary);
            binary = "";
            pos = expression.find(sub,pos+1);
        }

        //1. For all starting positions parse forward until operator|parenthesis|end is encountered
        for (int i = 0; i < binaries.size(); i++){
            std::string binary = binaries[i];
            //std::cout<<binary<<std::endl;
            double value = convertValue(binary, false);

            std::ostringstream os;
            os << value;
            std::string str = os.str();

            ReplaceStringInPlace(expression, binary, str);
        }

        sub = "1b";

        //1. Find starting positions of "0b"
        pos = expression.find(sub, 0);
        while(pos != std::string::npos)
        {
            //std::cout<<pos<<std::endl;
            std::string binary = "";

            for(int j = pos+1; j >= 0; --j) {
                int index = static_cast<int>(j);
                char current = expression[index];
                if(current == ' ' || current == '+' || current == '-' || current == '/' || current == '*' || current == '(' || current == ')' || current == '^')
                {
                    break;
                }
                else
                {
                    if(binary != "")
                        binary = std::string(1, current) + binary;
                    else
                        binary = std::string(1, current);
                }
            }
            //std::cout<<binary<<std::endl;
            binaries.push_back(binary);
            binary = "";
            pos = expression.find(sub,pos+1);
        }

        //1. For all starting positions parse forward until operator|parenthesis|end is encountered
        for (int i = 0; i < binaries.size(); i++){
            std::string binary = binaries[i];
            //std::cout<<binary<<std::endl;
            double value = convertValue(binary, false);

            std::ostringstream os;
            os << value;
            std::string str = os.str();

            ReplaceStringInPlace(expression, binary, str);
        }
    }

    bool is_number(const std::string& s)
    {
        long double ld;
        return((std::istringstream(s) >> ld >> std::ws).eof());
    }
};

int main(int argc, char** argv)
{
    Calculator cal;
    cal.readFile();
    //cal.printAllVariables();
    return 0;
}
