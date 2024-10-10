#ifndef TOKENIZATION_H
#define TOKENIZATION_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <iomanip>
#include <map>

using namespace std;


// Enum class to define different types of tokens
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    LITERAL,
    OPERATOR,
    SEPARATOR,
    UNKNOWN
};

// Struct to represent a token with its type and value
struct Token {
    TokenType type;
    string value;

    Token(TokenType t, const string& v)
        : type(t)
        , value(v)
    {
    }
};

// Class that implements the lexical analyzer
class LexicalAnalyzer {
private:
    string input;
    size_t position;
    unordered_map<string, TokenType> keywords;
    string cleanedInput;


    // Function to initialize the keywords map
    void initKeywords()
    {
        keywords["int"] = TokenType::KEYWORD;
        keywords["float"] = TokenType::KEYWORD;
        keywords["if"] = TokenType::KEYWORD;
        keywords["else"] = TokenType::KEYWORD;
        keywords["while"] = TokenType::KEYWORD;
        keywords["return"] = TokenType::KEYWORD;
        keywords["string"] = TokenType::KEYWORD;
        keywords["do"] = TokenType::KEYWORD;
        keywords["void"] = TokenType::KEYWORD;
        keywords["cout"] = TokenType::KEYWORD;
        keywords["endl"] = TokenType::KEYWORD;
        keywords["for"] = TokenType::KEYWORD;
        keywords["#include"] = TokenType::KEYWORD;
        keywords["using"] = TokenType::KEYWORD;
        keywords["namespace"] = TokenType::KEYWORD;
        keywords["std"] = TokenType::KEYWORD;
        keywords["iostream"] = TokenType::KEYWORD;
        keywords["fstream"] = TokenType::KEYWORD;
        keywords["vector"] = TokenType::KEYWORD;
    }

    // Function to check if a character is whitespace
    bool isWhitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\n'
               || c == '\r';
    }

    // Function to check if a character is alphabetic
    bool isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z')
               || (c >= 'A' && c <= 'Z');
    }

    // Function to check if a character is a digit
    bool isDigit(char c) { return c >= '0' && c <= '9'; }

    // Function to check if a character is alphanumeric
    bool isAlphaNumeric(char c)
    {
        return isAlpha(c) || isDigit(c);
    }

    // Function to get the next word (identifier or keyword) from the input
    string getNextWord()
    {
        size_t start = position;
        while (position < input.length() && isAlphaNumeric(input[position]) && input[position] != '_') {
            position++;

        }
        // Check if we've reached a non-word character
        if (position < input.length() && !isAlphaNumeric(input[position])) {
            if(position == '_' && isAlphaNumeric(input[position+1])){
                position++;
            }
            string result = input.substr(start, position - start);
            position--;
            return result;
        }
        return ""; // Return empty string if no valid word found
    }

    // Function to get the next number (integer or float) from the input
    string getNextNumber()
    {
        size_t start = position;
        bool hasDecimal = false;
        while (position < input.length()
               && (isDigit(input[position])
                   || input[position] == '.')) {
            if (input[position] == '.') {
                if (hasDecimal)
                    break;
                hasDecimal = true;
            }
            position++;
        }
        return input.substr(start, position - start);
    }


public:

    // Constructor for LexicalAnalyzer
    LexicalAnalyzer(const string& source)
        : input(source)
        , position(0)
    {
        initKeywords();
    }

    // Function to tokenize the input string
    vector<Token> tokenize()
    {
        vector<Token> tokens;
        bool inMultiLineComment = false;

        while (position < input.length()) {
            char currentChar = input[position];

            // Skip whitespace
            if (isWhitespace(currentChar)) {
                position++;
                continue;
            }

            // Check for preprocessor directives
            if (currentChar == '#'){
                string directive = getNextWord();
                    tokens.emplace_back(TokenType::KEYWORD, directive);
                    cleanedInput += directive;
            }

            // Check for multi-line comment start
            if (currentChar == '/' && input[position+1] == '*') {
                inMultiLineComment = true;
                position += 2;
            }
            // Check for single-line comment start
            else if (currentChar == '/' && input[position+1] == '/') {
                // Skip until end of line
                while (position < input.length() && input[position] != '\n') {
                    position++;
                }
                continue;
            }

            // Check for multi-line comment end
            if (inMultiLineComment && currentChar == '*' && input[position+1] == '/') {
                inMultiLineComment = false;
                position += 2;
            }

            // Process non-comment characters
            if (!inMultiLineComment) {
                // Identify keywords or identifiers
                if (isAlpha(currentChar)) {
                    string word = getNextWord();
                    if (keywords.find(word) != keywords.end()) {
                        tokens.emplace_back(TokenType::KEYWORD,
                                            word);
                    }
                    else {
                        tokens.emplace_back(
                            TokenType::IDENTIFIER, word);
                    }
                    cleanedInput += word;
                }
                else if (isAlpha(currentChar) || currentChar == '_') {
                    string word;
                    while (position < input.length() && (isAlphaNumeric(input[position]) || input[position] == '_')) {
                        word += input[position];
                        position++;
                    }
                    tokens.emplace_back(TokenType::IDENTIFIER, word);
                    cleanedInput += word;
                }
                // Identify integer or float literals
                else if (isDigit(currentChar)) {
                    string number = getNextNumber();
                    if (number.find('.') != string::npos) {
                        tokens.emplace_back(
                            TokenType::LITERAL, number);
                    }
                    else {
                        tokens.emplace_back(
                            TokenType::LITERAL, number);
                    }
                    cleanedInput += number;
                }
                // Check for left shift operator
                else if (currentChar == '<' && input[position+1] == '<') {
                    tokens.emplace_back(TokenType::OPERATOR, "<<");
                    cleanedInput += "<<";
                    position += 2;
                }
                // Check for right shift operator
                else if (currentChar == '>' && input[position+1] == '>') {
                    tokens.emplace_back(TokenType::OPERATOR, ">>");
                    cleanedInput += ">>";
                    position += 2;
                }
                // Identify operators
                else if (currentChar == '+'
                        || currentChar == '-'
                        || currentChar == '*'
                        || currentChar == '='
                        || currentChar == '<'
                        || currentChar == '>'
                        || currentChar == '^'
                        || currentChar == '/') {
                    tokens.emplace_back(TokenType::OPERATOR,
                                        string(1, currentChar));
                    cleanedInput += currentChar;
                    position++;
                }

                // Identify separators
                else if (currentChar == '('
                        || currentChar == ')'
                        || currentChar == '{'
                        || currentChar == '}'
                        || currentChar == ','
                        || currentChar == ';') {
                    tokens.emplace_back(TokenType::SEPARATOR,
                                        string(1, currentChar));
                    cleanedInput += currentChar;
                    position++;
                }
                // Identify String Literals
                else if(currentChar == '"'){
                    string literalString;
                    position++;

                    bool inEscapedQuote = false;
                    while(position < input.length()) {
                        if (input[position] == '"' && !inEscapedQuote) {
                            // End of string literal
                            position++; // Move past the closing quote
                            break;
                        } else if (input[position] == '\\') {
                            inEscapedQuote = true;
                            position++; // Move past the backslash
                        } else {
                            literalString += input[position++];
                            inEscapedQuote = false;
                        }
                    }
                    
                    if (!literalString.empty()) {
                        tokens.emplace_back(TokenType::LITERAL, literalString);
                    }
                    cleanedInput += '"' + literalString + '"';
                }
                // Handle unknown characters
                else {
                    tokens.emplace_back(TokenType::UNKNOWN,
                                        string(1, currentChar));
                    cleanedInput += currentChar;
                    position++;
                }
                position++;
            }
        }

        return tokens;
    }

        // New method to display cleaned-up text
    void printCleanedInput() {
        cout << "Cleaned-up Input:\n" << cleanedInput << "\n\n";
    }
};

// Function to convert TokenType to string for printing
string getTokenTypeName(TokenType type)
{
    switch (type) {
    case TokenType::KEYWORD:
        return "KEYWORD";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::LITERAL:
        return "LITERAL";
    case TokenType::OPERATOR:
        return "OPERATOR";
    case TokenType::SEPARATOR:
        return "SEPARATOR";
    case TokenType::UNKNOWN:
        return "UNKNOWN";
    default:
        return "UNDEFINED";
    }
}

// Function to print all tokens
void printTokens(const vector<Token>& tokens)
{
    for (const auto& token : tokens) {
        cout << "Type: " << getTokenTypeName(token.type)
             << ", Value: " << token.value << endl;
    }
}

void printUniqueTokens(const vector<Token>& tokens) {
    // Create a map to group tokens by category
    map<TokenType, set<string>> tokenCategories;

    // Group tokens by category
    for (const auto& token : tokens) {
        tokenCategories[token.type].insert(token.value);
    }
    // Print header
    cout << left << setw(15) << "Category"
          << left << setw(15) << "Tokens" << endl;

    // Print separator
    cout << string(35, '-') << endl;

    // Print each category
    for (const auto& pair : tokenCategories) {
        const TokenType& type = pair.first;
        const set<string>& values = pair.second;

        cout << left << setw(15) << getTokenTypeName(type)
              << left << setw(2);

        // Print tokens for this category
        for (const auto& value : values) {
            cout << value << "   ";
        }
        cout << endl;
    }
}

// Function to read from file
void tokenizeFile(const string& filename){

    ifstream inFile(filename);  // Open text file
    if(!inFile){    // If text file can't be opened, return error message
        cerr << "Error: File could not be opened." << endl;
        return;
    }

    string fileContent((istreambuf_iterator<char>(inFile)), {}); // Read contents of opened text file into string    
    inFile.close(); // Close text file

    LexicalAnalyzer textFile(fileContent);

    // Tokenize the file content
    vector<Token> tokens = textFile.tokenize();

    // Print modified file
    textFile.printCleanedInput();

    // Print all identified tokens
    cout << endl;
    printUniqueTokens(tokens);
    cout << endl;

    return;
}

#endif