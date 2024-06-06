#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

enum TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    IDENTIFIER,
    STRING,
    NUMBER,

    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    EOF_TOKEN
};

bool hadError = false;

void reporter(int line, std::string where, std::string message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message
        << std::endl;
    hadError = true;
}

void error(int line, std::string message) { reporter(line, "", message); }

class Token {
private:
    TokenType type;
    std::string lexeme;
    void *literal;
    int line;

public:
    Token(TokenType type, std::string lexeme, void *literal, int line)
    : type(type), lexeme(lexeme), literal(literal), line(line) {}

    void print() {
        std::cout << "Token: " << "type: " << type << " lexme: " 
            << lexeme << " literal: " << literal << " line: " << line << std::endl;
    }
};

class Scanner {
private:
    std::string source;
    std::vector<Token> tokens;

    std::map<std::string, TokenType> keywords = {
        {"and", AND},
        {"class", CLASS},
        {"else", ELSE},
        {"false", FALSE},
        {"for", FOR},
        {"fun", FUN},
        {"if", IF},
        {"nil", NIL},
        {"or", OR},
        {"print", PRINT},
        {"return", RETURN},
        {"super", SUPER},
        {"this", THIS},
        {"true", TRUE},
        {"var", VAR},
        {"while", WHILE}
    };

    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd() { return current >= source.length(); }

    char advance() { return source[current++]; }

    void addToken(TokenType type) { 
        addToken(type, nullptr);
    }

    void addToken(TokenType type, void *literal) {
        std::string text = source.substr(start, current - start);
        tokens.push_back(Token(type, text, literal, line));
    }

    bool match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected) return false;

        current++;
        return true;
    }

    char peek() {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    char peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    } 

    TokenType get(const std::string &text) {
        auto it = keywords.find(text);
        if (it != keywords.end()) {
            return it->second;
        } else {
            return IDENTIFIER;
        }
    }

    void string() {
        while (peek() != '"' && !isAtEnd()) {
            if(peek() == '\n') line++;
            advance();
        }

        if (isAtEnd()) {
            error(line, "Unterminated string");
            return;
        }

        advance();

        std::string val = source.substr(start + 1, current - 2);
        addToken(STRING, &val);
    }

    void digit() {
        while(isdigit(peek())) advance();

        if(peek() == '.' && isdigit(peekNext())) {
            advance();
            while (isdigit(peek())) { advance(); }
        }

        double num = std::stod(source.substr(start, current - 1));
        addToken(NUMBER, &num);
    }

    void identifier() {
        while (isalnum(peek())) advance();

        std::string text = source.substr(start, current-1);
        TokenType type = get(text);

        addToken(type);
    }

    void scanToken() {
        char c = advance();
        switch (c) {
            case '(':
                addToken(LEFT_PAREN);
                break;
            case ')':
                addToken(RIGHT_PAREN);
                break;
            case '{':
                addToken(LEFT_BRACE);
                break;
            case '}':
                addToken(RIGHT_BRACE);
                break;
            case ',':
                addToken(COMMA);
                break;
            case '.':
                addToken(DOT);
                break;
            case '-':
                addToken(MINUS);
                break;
            case '+':
                addToken(PLUS);
                break;
            case ';':
                addToken(SEMICOLON);
                break;
            case '*':
                addToken(STAR);
                break;

            case '!':
                addToken(match('=') ? BANG_EQUAL : BANG);
                break;
            case '=':
                addToken(match('=') ? EQUAL_EQUAL : EQUAL);
                break;
            case '<':
                addToken(match('=') ? LESS_EQUAL : LESS);
                break;
            case '>':
                addToken(match('=') ? GREATER_EQUAL : GREATER);
                break;

            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    addToken(SLASH);
                }
                break;

            case '"' : string(); break;

            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                line++;
                break;
            default:
                if(isdigit(c)){
                    digit();
                    break;
                } else if (isalpha(c)) {
                    identifier();
                } else {
                    error(line, "Unexpected character.");
                    break;
                }
        }
    }

public:
    Scanner(std::string source) : source(source) {}

    std::vector<Token> scanTokens() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }

        tokens.push_back(Token(EOF_TOKEN, "", nullptr, line));
        return tokens;
    }
};

void run(const std::string source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    for (Token token : tokens) {
        token.print();
    }
}

void runFile(const char *filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        exit(74);
    }

    std::vector<char> buffer{std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()};

    run(std::string(buffer.begin(), buffer.end()));

    if (hadError)
        exit(65);
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: zc" << " [filename]" << std::endl;
        exit(64);
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        std::cout << "No file specified" << std::endl;
    }

    return 0;
}
