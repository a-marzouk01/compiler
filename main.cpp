#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum tokens {
    RETURN,
    INT_LIT,
    SEMI,
    INVALID
};

class Token {
    public:
        tokens type;
        int value;

        Token(const tokens& n) : type(n), value(0) {}
        Token(const tokens& n, int a) : type(n), value(a) {}
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
            }
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

std::string usingTokens(std::vector<Token> t) {
    std::string asmCode;
    asmCode = "global _start\n\n_start:\n";

    for(int i = 0; i < t.size(); i++) {
        if(t[i].type == RETURN){
            if(i + 2 < t.size()) {
                if(t[i + 1].type == INT_LIT && t[i + 2].type == SEMI) {
                    std::cout << i << " " << t[i].type << " " << t[i].value << std::endl;
                    asmCode += "    mov rax, 60\n    mov rdi, ";
                    asmCode += std::to_string(t[i + 1].value);
                    asmCode += "\n    syscall";
                }
            }
        }
    }

    return asmCode;
}

int main(int argc, char* argv[]) {
    std::cout << argv[1] << std::endl;

    std::ifstream file(argv[1]);
    std::string code((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));;

    std::vector<Token> found = getTokens(code);
    //for(int i = 0; i < found.size(). i++) {
      //  std::cout << "type: " << found[i].type << " value: " <
    //}

    {
        std::ofstream asmFile("./asm/main.asm");
        asmFile << usingTokens(found);
    }

    return 0;
}
