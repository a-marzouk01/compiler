#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using std::ostringstream;

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
void reporter(int line, std::string loc, std::string message) {
    std::cerr << "[line " << line << "] Error " <<  loc << ": " << message;
    hadError = true;
}

void error(int line, std::string message) {
    reporter(line, "", message);
}

class Token {
private:
	const TokenType type;
	const std::string lexeme;
	const void* literal;
	const int line;

public:
    Token(TokenType type, const std::string& lexeme, const void* literal, int line) 
        : type(type), lexeme(lexeme), literal(literal), line(line) {}
    
    std::string tostring() {
        std::ostringstream oss;
        oss << static_cast<int>(type) << " " << lexeme << " "; 
        if (literal != nullptr) {
            oss << *reinterpret_cast<const std::string *>(literal); 
        }
        return oss.str();
    }
};

class Scanner {
private:
    std::string source;
    std::vector<Token> tokens;

    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd() {
        return current >= source.length();
    }

    char advance() {
        return source[current++];
    }

    void addToken(TokenType type) {
        std::string text = source.substr(start, current - start);
        tokens.push_back(Token(type, text, nullptr, line));
    }

    void addToken(TokenType type, const void* literal) {
        std::string text = source.substr(start, current);
        tokens.push_back(Token(type, text, literal, line));
    }

    void scanToken() {
        char c = advance();
        switch (c) {
            case '(': addToken(LEFT_PAREN); break;
            case ')': addToken(RIGHT_PAREN); break;
            case '{': addToken(LEFT_BRACE); break;
            case '}': addToken(RIGHT_BRACE); break;
            case ',': addToken(COMMA); break;
            case '.': addToken(DOT); break;
            case '-': addToken(MINUS); break;
            case '+': addToken(PLUS); break;
            case ';': addToken(SEMICOLON); break;
            case '*': addToken(STAR); break; 
            default: error(line, "Unexpected character");
        }
    }
public:
    Scanner(std::string str) : source(str) {}


    std::vector<Token> scanTokens() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }

        tokens.push_back(Token(EOF_TOKEN, "", nullptr, line));
        return tokens;
    }
};


void run(std::string source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();
}

std::vector<char> readAllBytes(const std::string &path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
        std::cerr << "failed to open the file at " << path << std::endl;
        return {};
  }

  std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
  return buffer;
}

void runFile(std::string path) {
    std::vector<char> bytes = readAllBytes(path);
    if (bytes.empty()) { std::cerr << "provided file is empty" << std::endl; }

    std::string sourceCode(bytes.begin(), bytes.end());
    // run(sourceCode);
    if (hadError) std::exit(65);
}

void Prompt() {
    for(;;) {
        std::cout << '>';
        std::string line;
        if(!std::getline(std::cin, line)) {
            break;
        }
        run(line);
        hadError = false;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::cerr << "Usage: zc [filePath]" << std::endl;
        return 64;
    } else if (argc == 1) {
        runFile(argv[0]);
    } else {
        Prompt();
    }

    return 0;
}
