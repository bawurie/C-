#include "ExpressionParser.h"

const std::string charSet[] = { "(", ")", "%", "+", "-", "*", "/", "^", "," };

const double pi = 3.1415927;

const double e = 2.71828182846;

template<class BidirIt>

BidirIt Prev(BidirIt it,

typename std::iterator_traits<BidirIt>::difference_type n = 1)

{

std::advance(it, -n);

return it;

}

int Modulo(int num, int div)

{

int mod = num % div;

return ( num >= 0 || mod == 0 ) ? mod : div + mod;

}

unsigned int OpArgCount( const std::string& s )

{

unsigned int val = 1;

if ( s == "*" || s == "/" || s == "%" ||

s == "+" || s == "-" || s == "=" ||

s == "^" || s == "POW" )

{

val = 2;

}

else if ( s == "!" )

{

val = 1;

}

return val;

}

// Return operator precedence

int OpPrecedence(const std::string& s)

{

int precedence = 1;

if ( s == "!" )

precedence = 4;

else if ( s == "*" || s == "/" || s == "%" )

precedence = 3;

else if ( s == "+" || s == "-" )

precedence = 2;

else if ( s == "=" )

precedence = 1;

return precedence;

}

// Return true if left associative; false otherwise

bool OpLeftAssoc( const std::string& s )

{

bool opLeftAssoc = false;

if ( s == "*" || s == "/" ||

s == "%" || s == "+" || s == "-" )

{

opLeftAssoc = true;

}

else if ( s == "=" || s == "!" )

{

opLeftAssoc = false;

}

return opLeftAssoc;

}

// Is token an operator

bool IsOperator( const std::string& s )

{

return s == "+" || s == "-" ||

s == "/" || s == "*" ||

s == "!" || s == "%" ||

s == "=";

}

// Is token a function argument separator eg comma

bool IsComma( const std::string& s )

{

return s == ",";

}

// Convert string into all uppercase

std::string UpperCase( std::string input )

{

for ( std::string::iterator it = input.begin();

it != input.end();

++it )

*it = toupper(*it);

return input;

}

// Is token PI

bool IsPi( const std::string& s )

{

if ( UpperCase( s ) == "PI" )

return true;

return false;

}

// Is token Euler's constant

bool IsE( const std::string& s )

{

if ( UpperCase( s ) == "E" )

{

return true;

}

return false;

}

// Is the token a function

bool IsFunction( const std::string& s )

{

std::string str = UpperCase( s );

bool isFunction = false;

if ( str.find( "^" ) != std::string::npos ||

str.find( "SIN" ) != std::string::npos ||

str.find( "COS" ) != std::string::npos ||

str.find( "TAN" ) != std::string::npos ||

str.find( "LN" ) != std::string::npos ||

str.find( "LOG" ) != std::string::npos ||

str.find( "EXP" ) != std::string::npos ||

str.find( "POW" ) != std::string::npos ||

str.find( "SQRT" ) != std::string::npos )

{

isFunction = true;

}

return isFunction;

}

// Is the number a float

bool IsFloat( const std::string& s )

{

std::istringstream iss( s );

float f;

iss >> std::noskipws >> f;

return iss.eof() && !iss.fail();

}

// Is the string a number

bool IsNumber( const std::string& s )

{

std::string::const_iterator it = s.begin();

while ( it != s.end() && std::isdigit(*it, std::locale() ) )

{

++it;

}

return !s.empty() && it == s.end();

}

ExpressionParser::ExpressionParser( const std::string& input )

{

m_strInput = input;

}

// Determine if matching number of left and right parentheses

bool ExpressionParser::MatchingParetheses()

{

const size_t nLeft = std::count( m_strInput.begin(),

m_strInput.end(), '(');

const size_t nRight = std::count( m_strInput.begin(),

m_strInput.end(), ')');

return nLeft == nRight && !m_strInput.empty();

}

// Split selected text into delimited vector array of strings

void ExpressionParser::Tokenize( std::list<std::string>& tokens,

const std::string& delimiter )

{

// Insert whitepaces before and after each special characters

size_t size = sizeof( charSet ) / sizeof( std::string );



for ( int i = 0; i < static_cast<int>( size ); i++ ) {

std::string s = charSet[ i ];

ReplaceAll( m_strInput, s, " " + s + " " );

}

size_t next_pos = 0;

size_t init_pos = m_strInput.find_first_not_of( delimiter, next_pos );

while ( next_pos != std::string::npos &&

init_pos != std::string::npos ) {

next_pos = m_strInput.find( delimiter, init_pos );

std::string token = m_strInput.substr( init_pos, next_pos - init_pos );

tokens.push_back( token );

init_pos = m_strInput.find_first_not_of( delimiter, next_pos );

}

std::string firstToken = tokens.front();

if ( firstToken == "-" ) {

std::list<std::string>::iterator it = tokens.begin();

it++;

if ( it == tokens.end() ) return;

std::string nextToken = *( it );

if ( IsNumber( nextToken ) || IsFloat( nextToken ) ) {

tokens.pop_front();

tokens.front() = firstToken + nextToken;

}

else if ( nextToken == "(" || IsFunction( nextToken ) ) {

tokens.front() = firstToken + "1";

tokens.insert( it, "*" );

}

// minus minus is a plus

else if ( nextToken == "-" && firstToken == "-" ) {

std::list<std::string>::iterator nit = it;

std::advance ( nit, -1 );

tokens.erase( it );

tokens.erase( nit );

}

}

// Deal with minus sign after opening parenthesis or operator

typedef std::list<std::string>::iterator t_iter;

std::string prevToken = "";

for ( t_iter it = tokens.begin(); it != Prev( tokens.end() ); it++ ) {

std::string token = *it;

std::list<std::string>::iterator nit = it;

std::advance ( nit, 1 );

if ( nit == tokens.end() ) break;

std::string ntoken = *nit;

if ( token == "-" && prevToken == "(" ) {

if ( IsNumber( ntoken ) || IsFloat( ntoken ) ) {

tokens.erase( nit );

*it = "-" + ntoken;

token = *it;

}

}

else if ( token == "-" &&

( IsOperator( prevToken ) || prevToken == "^" || prevToken == "%" ) ) {

// Minus minus becomes a plus

if ( token == "-" && prevToken == "-" ) {

std::list<std::string>::iterator nit = it;

std::list<std::string>::iterator nnit = nit;

nnit++;

std::advance ( nit, -1 );

tokens.erase( it );

*nit = "+";

std::list<std::string>::iterator pnit = nit;

std::advance ( pnit, -1 );

if ( IsOperator( *pnit ) || *pnit == "(" )

tokens.erase( nit );

token = *nnit; it = nnit;

if ( it == Prev( tokens.end() ) ) break;

}

else if ( IsNumber( ntoken ) ||

IsFloat( ntoken ) ||

IsFunction( ntoken ) ) {

bool exit = false;

if ( nit == Prev( tokens.end() ) ) exit = true;

tokens.erase( nit );

*it = "-" + ntoken;

token = *it;

if ( exit ) break;

}

else if ( ntoken == "(" ) {

*it = "-1";

token = *it;

tokens.insert( nit, "*" );

}

}

prevToken = token;

}

// Deal with minus sign before opening parenthesis

prevToken = ""; t_iter prevIt;

for ( t_iter it = tokens.begin(); it != tokens.end(); it++ ) {

std::string token = *it;

if ( token == "(" && prevToken == "-" ) {

tokens.insert( it, "1" );

tokens.insert( it, "*" );

}

prevToken = token;

prevIt = it;

}

}

// Replace all instances of selected string with replacement string

void ExpressionParser::ReplaceAll( std::string& str, const std::string& from,

const std::string& to )

{

size_t start_pos = 0;

while( (start_pos = str.find(from, start_pos)) != std::string::npos)

{

str.replace(start_pos, from.length(), to);

start_pos += to.length();

}

}

// Deduce the numerical result from the RPN string passed to it

bool ExpressionParser::Evaluate(

const std::vector<std::string>& rpn,

std::string& result )

{

typedef std::vector<std::string>::const_iterator rpn_iter;

std::stack<std::string> stack;

for ( rpn_iter it = rpn.begin(); it != rpn.end(); it++ )

{

std::string token = *it;

if( IsNumber( token ) || IsFloat( token ) ||

IsPi( token ) || IsE( token ) )

{

if ( IsPi( token ) )

{

std::stringstream s;

s << pi;

token = s.str();

}

else if ( IsE( token ) )

{

std::stringstream s;

s << e;

token = s.str();

}

stack.push( token );

}

else if( IsOperator( token ) || IsFunction( token ) )

{

double result = 0.0;

unsigned int nargs = OpArgCount( UpperCase( token ) );

bool isUnary = false;

unsigned int stackArgs = stack.size();

std::vector<double> args;

if( stackArgs < nargs)

{

// For dealing with unary '-' or unary '+'

if ( stackArgs == 1 &&

nargs == 2 && ( token == "+" || token == "-" ) )

{

std::string value = stack.top();

result = strtod( value.c_str(), NULL );

stack.pop(); isUnary = true;

}

else { // (Error) Insufficient values in the expression.

return false;

}

}

else

{

// Else, Pop the top n values from the stack.

while ( nargs > 0 )

{

std::string value = stack.top();

double d = strtod( value.c_str(), NULL );

args.push_back( d );

stack.pop(); nargs--;

}

}

// Evaluate the operator, with the values as arguments.

if ( IsOperator( token ) && !isUnary )

{

double d2 = args[ 0 ];

double d1 = args[ 1 ];

if ( token == "+" ) {

result = d1 + d2;

}

else if ( token == "-" ) {

result = d1 - d2;

}

else if ( token == "*" ) {

result = d1 * d2;

}

else if ( token == "/" ) {

result = d1 / d2;

}

else if ( token == "%" ) {

int i2 = (int) args[ 0 ];

int i1 = (int) args[ 1 ];

double iresult = Modulo( i1, i2 );

result = iresult;

}

}

else if ( IsFunction( token ) )

{

double d0 = args[ 0 ];

std::string capToken = UpperCase( token );

double mult = token.find( "-" ) != std::string::npos ? -1 : 1;

if ( capToken.find( "SIN" ) != std::string::npos ) {

result = sin( d0 );

}

else if ( capToken.find( "COS" ) != std::string::npos ) {

result = cos( d0 );

}

else if ( capToken.find( "TAN" ) != std::string::npos ) {

result = tan( d0 );

}

else if ( capToken.find( "LN" ) != std::string::npos ) {

result = log( d0 );

}

else if ( capToken.find( "LOG" ) != std::string::npos ) {

result = log10( d0 );

}

else if ( capToken.find( "EXP" ) != std::string::npos ) {

result = exp( d0 );

}

else if ( capToken.find( "^" ) != std::string::npos ) {

double d2 = args[ 0 ]; double d1 = args[ 1 ];

if ( d1 < 0 ) mult = -1.0;

result = pow( (double) d1, d2);

}

else if ( capToken.find( "POW" ) != std::string::npos ) {

double d2 = args[ 0 ]; double d1 = args[ 1 ];

result = pow( d1, d2);

}

else if ( capToken.find( "SQRT" ) != std::string::npos ) {

result = sqrt( d0 );

}

result *= mult;

}

// Push the returned results, if any, back onto the stack

if ( result == (long) result )

{

result = long( result );

}

std::stringstream s; s << result;

stack.push( s.str() );

}

}

if ( stack.size() == 1 )

{

result = stack.top();

double res = strtod( result.c_str(), NULL );

if ( res == (long) res )

{

long lres = (long) res;

std::stringstream s;

s << lres;

result = s.str();

}

return true;

}

return false; // (Error) The user input has too many values

}

// Convert infix expression format into reverse Polish notation

bool ExpressionParser::InfixToRPN( std::vector<std::string>& inputs )

{

typedef std::list<std::string>::const_iterator tok_iter;

std::list<std::string> infix;

std::stack<std::string> stack;

std::queue<std::string> outputQueue;

Tokenize( infix, " " );

bool success = true;

for ( tok_iter it = infix.begin(); it != infix.end(); it++ )

{

std::string token = *it;

if ( IsNumber( token ) ||

IsFloat( token ) ||

IsPi( token ) ||

IsE( token ) )

{

outputQueue.push( token );

}

else if ( IsFunction( token ) )

{

stack.push( token );

}

else if ( IsComma( token ) )

{

std::string stackToken = stack.top();

while ( stackToken != "(" )

{

outputQueue.push( stackToken );

stack.pop(); stackToken = stack.top();

}

if ( stackToken == "(" )

{

success = true;

}

else

{

success = false;

}

}

else if ( IsOperator( token ) )

{

while( !stack.empty() &&

IsOperator( stack.top() ) &&

( ( OpLeftAssoc( token ) && OpPrecedence( token ) == OpPrecedence( stack.top() ) ) ||

( OpPrecedence( token ) < OpPrecedence( stack.top() ) ) ) ) {

std::string stackToken = stack.top();

stack.pop(); outputQueue.push( stackToken );

}

stack.push( token );

}

else if ( token == "(" )

{

stack.push(token);

}

else if ( token == ")" )

{

while ( !stack.empty() && stack.top() != "(" )

{

outputQueue.push( stack.top() );

stack.pop();

}

if ( !stack.empty() )

{

std::string stackToken = stack.top();

if ( stackToken != "(" )

success = false;

}

else

{

return false;

}

stack.pop();

if ( !stack.empty() )

{

std::string stackToken = stack.top();

if ( IsFunction( stackToken ) )

{

outputQueue.push( stackToken );

stack.pop();

}

}

}

}

while ( !stack.empty() )

{

outputQueue.push( stack.top() );

stack.pop();

}

while ( !outputQueue.empty() ) {

std::string token = outputQueue.front();

inputs.push_back( token );

outputQueue.pop();

}

return success;

}
