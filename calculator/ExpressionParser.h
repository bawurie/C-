#include <algorithm>

#include <string>

#include <vector>

#include <list>

#include <iostream>

#include <fstream>

#include <iterator>

#include <queue>

#include <stack>

#include <sstream>

#include <locale>

#include <stdlib.h>

#include <math.h>

class ExpressionParser

{

public:

ExpressionParser( const std::string& input );

bool MatchingParetheses();

bool Evaluate( const std::vector<std::string>& rpn, std::string& result );

bool InfixToRPN( std::vector<std::string>& inputs );

private:

void ReplaceAll( std::string& str,

const std::string& from,

const std::string& to );

void Tokenize( std::list<std::string>& tokens,

const std::string& delimiter );

private:

std::string m_strInput;

};
