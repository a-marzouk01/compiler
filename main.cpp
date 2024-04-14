#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum tokens {
    RETURN,
    INT_LIT,
    SEMI,
    INVALID,
    EQUAL,
    STRING,
    LET
};

class Token {
    public:
        tokens type;
        int value;
        std::string val;

        Token(const tokens& n) : type(n), value(0), val("") {}
        Token(const tokens& n, int a) : type(n), value(a), val("") {}
        Token(const tokens& n, std::string s) : type(n), value(0), val(s) {}
        Token(const tokens& n, int a, std::string s) : type(n), value(a), val(s) {}
};

std::vector<Token> getTokens(std::string str) {
    std::vector<Token> token_list;
    std::string buf;

    for(int i = 0; i < str.length(); i++) {
        char c = str[i];
        if(isalpha(c)) {
            buf += c;
            while(isalnum(str[++i])) {
                buf += str[i];
            }
            if(buf == "return") {
                token_list.push_back({ RETURN });
            } else if( buf == "let") {
                token_list.push_back({ LET });
            } else {
                token_list.push_back({ STRING, buf });
            }
            i--;
            std::cout << "buf: " << buf << " i: " << i << std::endl;
            buf = "";
        } else if(isdigit(c)) {
            buf += c;
            while(isdigit(str[++i])) {
                buf += str[i];
            }
            token_list.push_back({ INT_LIT, std::stoi(buf) });
            buf = "";
            i--;
        } else if(c == ';') {
            buf += c;
            token_list.push_back({ SEMI });
            buf = "";
        } else if(c == '=') {
            buf += c;
            token_list.push_back({ EQUAL });
            buf = "";
        } else if(isspace(c)) {
            continue;
        } else {
            token_list.push_back({ INVALID });
            std::cerr << "FOUND AN INVALID, at i = " << i << std::endl;
            break;
        }
    }

    return token_list;
}

std::string code(std::vector<Token> t) {
    std::string code;
    for(int i = 0; i < t.size(); i++) {
        std::cout << i << " " << t.size() << std::endl;
        if(t[i].type == RETURN){
            if(i + 2 < t.size()) {
                if(t[i + 1].type == INT_LIT && t[i + 2].type == SEMI) {
                    code += "    mov rax, 60\n    mov rdi, ";
                    code += std::to_string(t[i + 1].value);
                    code += "\n    syscall";
                    i+=2;
                } else if (t[i + 1].type == STRING && t[i + 2].type == SEMI) {
                    code += "    mov rax, 60\n    mov edi, dword [";
                    code += t[i + 1].val;
                    code += "]\n    syscall";
                    i+=2;
                }
            } else {
                std::cerr << "return used withut an error return value" << std::endl;
                break;
            }
        }
    }
    return code;
}


std::string variables(std::vector<Token> t) {
    std::string code;
    for(int i = 0; i < t.size(); i++) {
    if(t[i].type == LET) {
            if(i + 4 < t.size()) {
                if(t[i+1].type == STRING && t[i+2].type == EQUAL && t[i+3].type == INT_LIT && t[i+4].type == SEMI) {
                    //std::cout << i << " " << t[i].type << " " << t[i].value << std::endl;
                    code += (t[i+1].val + " dd " + std::to_string(t[i+3].value));
                    i += 4;
                }
            }
        }
    }
    return code;
}

std::string usingTokens(std::vector<Token> t) {
    std::string asmCode;
    asmCode = "section .data\n";
    asmCode += variables(t);
    asmCode += "\nsection .text\n";
    asmCode += "global _start\n\n";
    asmCode += "_start:\n";
    asmCode += code(t);

    return asmCode;
}

int main(int argc, char* argv[]) {
    std::cout << argv[1] << std::endl;

    std::ifstream file(argv[1]);
    std::string code((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));;

    std::vector<Token> found = getTokens(code);
    for(int i = 0; i < found.size(); i++) {
          std::cout << "type: " << found[i].type << " value: " << found[i].value << " val: " << found[i].val << std::endl;
    }

    {
        std::ofstream asmFile("./asm/main1.asm");
        asmFile << usingTokens(found);
    }

    return 0;
}
