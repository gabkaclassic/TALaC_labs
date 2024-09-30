#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>
#include <algorithm>

using namespace std;

// Структура для хранения токенов
struct Token {
    string type;    // Тип токена
    string value;   // Значение токена
    int line;       // Строка в которой находится токен
};

// Возможные типы токенов
enum TokenType {
    TOKEN_INT, TOKEN_BOOL, TOKEN_VOID,
    TOKEN_MAIN, TOKEN_FOR, TOKEN_IF, TOKEN_RETURN,
    TOKEN_IDENTIFIER, TOKEN_NUMBER,
    TOKEN_ASSIGN, TOKEN_SEMICOLON, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_RELOP, TOKEN_EOF, TOKEN_UNKNOWN
};

// Лексический анализатор
class Lexer {
private:
    ifstream file;
    int line;
    char currentChar;

public:
    Lexer(const string& filename) : line(1) {
        file.open(filename);
        if (!file.is_open()) {
            cerr << "Ошибка при открытии файла: " << filename << endl;
            exit(1);
        }
        currentChar = file.get();
    }

    // Проверка конца файла
    bool isEOF() {
        return file.eof();
    }

    // Пропуск пробелов и комментариев
    void skipWhitespace() {
        while (isspace(currentChar)) {
            if (currentChar == '\n') line++;
            currentChar = file.get();
        }
    }

    // Возвращает следующий токен
    Token nextToken() {
        skipWhitespace();

        // Если конец файла
        if (isEOF()) {
            return {"EOF", "", line};
        }

        // Идентификатор или ключевое слово
        if (isalpha(currentChar) || currentChar == '_') {
            string identifier;
            while (isalnum(currentChar) || currentChar == '_') {
                identifier += currentChar;
                currentChar = file.get();
            }

            if (identifier == "int") return {"TYPE", "int", line};
            if (identifier == "bool") return {"TYPE", "bool", line};
            if (identifier == "void") return {"TYPE", "void", line};
            if (identifier == "main") return {"MAIN", "main", line};
            if (identifier == "for") return {"FOR", "for", line};
            if (identifier == "if") return {"IF", "if", line};
            if (identifier == "return") return {"RETURN", "return", line};
            
           if (identifier == "true" || identifier == "false") return {"BOOL", identifier, line};


            return {"IDENTIFIER", identifier, line};
        }

        // Число
        if (isdigit(currentChar)) {
            string number;
            while (isdigit(currentChar)) {
                number += currentChar;
                currentChar = file.get();
            }
            return {"NUMBER", number, line};
        }

        // Операторы
        if (currentChar == '=') {
            currentChar = file.get();
            if (currentChar == '=') {
                currentChar = file.get();
                return {"RELOP", "==", line};
            }
            return {"ASSIGN", "=", line};
        }
        if (currentChar == '<') {
            currentChar = file.get();
            return {"RELOP", "<", line};
        }
        if (currentChar == '>') {
            currentChar = file.get();
            return {"RELOP", ">", line};
        }
        if (currentChar == '!') {
            currentChar = file.get();
            if (currentChar == '=') {
                currentChar = file.get();
                return {"RELOP", "!=", line};
            }
        }

        // Разделители
        if (currentChar == ';') {
            currentChar = file.get();
            return {"SEMICOLON", ";", line};
        }
        if (currentChar == '{') {
            currentChar = file.get();
            return {"LBRACE", "{", line};
        }
        if (currentChar == '}') {
            currentChar = file.get();
            return {"RBRACE", "}", line};
        }
        if (currentChar == '(') {
            currentChar = file.get();
            return {"LPAREN", "(", line};
        }
        if (currentChar == ')') {
            currentChar = file.get();
            return {"RPAREN", ")", line};
        }

        // Неизвестный символ
        string unknown(1, currentChar);
        currentChar = file.get();
        return {"UNKNOWN", unknown, line};
    }
};

// Синтаксический анализатор
class Parser {
private:
    Lexer lexer;
    Token currentToken;
    int errorCount;
    unordered_map<string, string> symbolTable;
    string function_type;

public:
    Parser(const string& filename) : lexer(filename), errorCount(0) {
        currentToken = lexer.nextToken();
    }

    // Получение следующего токена
    void advance() {
        currentToken = lexer.nextToken();
    }

    // Ошибка
    void error(const string& message) {
        cerr << "Ошибка в строке " << currentToken.line << ": " << message << " (текущий токен: " << currentToken.value << ")" << endl;
        errorCount++;
        panicMode();
    }
    
    void error(const string& message, initializer_list<string> expected) {
        cerr << "Ошибка в строке " << currentToken.line << ": " << message << " (текущий токен: " << currentToken.value << ")" << endl;
        errorCount++;
        panicMode(expected);
    }

    void panicMode() {
        cout << "PANIC " << currentToken.type << endl;
        advance();
        cout << "PANIC 2 " << currentToken.type << endl;
    }
    
    void panicMode(initializer_list<string> expected) {
        cout << "EXPECTED PANIC ";
        for (const auto& type : expected) {
            cout << type << " ";
        }
        cout << endl;
    
    
        while (find(expected.begin(), expected.end(), currentToken.type) == expected.end() && currentToken.type != "EOF") {
            advance();
        }
    
        cout << "EXPECTED PANIC OUT 1 " << currentToken.type << endl;
    }
    // Проверка и ожидание токена
    void expect(const string& expectedType) {
        cout << "EXPECT " << expectedType << " " << currentToken.type << endl;
        if (currentToken.type == expectedType) {
            advance();
        } else {
            error("Ожидался " + expectedType, {expectedType});
        }
    }

    // <program> ::= <type> 'main' '(' ')' '{' <statement> '}'
    void program() {
        type(true);
        expect("MAIN");
        expect("LPAREN");
        expect("RPAREN");
        expect("LBRACE");
        statement();
        expect("RBRACE");
    }

    // <type> ::= 'int' | 'bool' | 'void'
    void type(bool function) {
        if (currentToken.type == "TYPE") {
            if(function) {
                function_type = currentToken.value;
            }
            advance();
        } else {
            error("Ожидался тип данных (int, bool или void)");
        }
    }

    // <statement> ::= <declaration> ';' | '{' <statement> '}' | <for> <statement> | <if> <statement> | <return>
    void statement() {
        if (currentToken.type == "LBRACE") {
            advance();  // Пропускаем '{'
            while (currentToken.type != "RBRACE" && currentToken.type != "EOF") {
                statement();  // Рекурсивно обрабатываем другие операторы внутри блока
            }
            expect("RBRACE");
        } else if (currentToken.type == "FOR") {
            advance();
            forStatement();
            statement();
        } else if (currentToken.type == "IF") {
            advance();
            ifStatement();
            statement();
        } else if (currentToken.type == "RETURN") {
            advance();
            bool success = returnStatement();
            if(!success) {
                advance();    
            }
            
        } else if(currentToken.type == "RBRACE") {
            advance();
            
        } else {
            cout << "DECLARATION " << currentToken.type << endl;
            declaration();
            expect("SEMICOLON");
        }
    }
    
    // Метод для проверки оператора return
    bool returnStatement() {
        string returnType = function_type;  // Получаем тип текущей функции (например, хранить его в контексте функции)
        
        if (currentToken.type == "NUMBER") {
            if (returnType != "int") {
                error("Несоответствие типов: ожидается " + returnType + ", но возвращено число.");
                return false;
            }
            advance();
        } else if (currentToken.type == "BOOL") {
            if (returnType != "bool") {
                error("Несоответствие типов: ожидается " + returnType + ", но возвращено булевое значение.");
                
                return false;
            }
            advance();
        } else if (currentToken.type == "IDENTIFIER") {
            string varName = currentToken.value;
            if (symbolTable.find(varName) == symbolTable.end()) {
                error("Переменная " + varName + " не объявлена.", {"SEMICOLON"});
                return false;
            } else if (symbolTable[varName] != returnType) {
                error("Несоответствие типов: ожидается " + returnType + ", но возвращена переменная типа " + symbolTable[varName] + ".");
                return false;
            }
            advance();
        } else {
            error("Некорректное возвращаемое значение.");
            return false;
        }
        expect("SEMICOLON");  // После return ожидается ';'
    
        return true;
    }


    // <declaration> ::= <type> <identifier> <assign>
    void declaration() {
        string varType = currentToken.value;
        type(false);
        string varName = currentToken.value; 
        expect("IDENTIFIER");
        symbolTable[varName] = varType;
        expect("ASSIGN");
        assign(varName);
    }

    // <assign> ::= '=' (<identifier> | <number> | <bool>)
    void assign(const string& varName) {
        string varType = symbolTable[varName];  // Получаем тип переменной
    
        if (currentToken.type == "NUMBER") {
            if (varType != "int") {  // Если тип переменной не соответствует числовому значению
                error("Несоответствие типов: переменной " + varName + " (типа " + varType + ") присваивается число.");
                return;
            }
            advance();
        } else if (currentToken.type == "IDENTIFIER") {
            string assignedVar = currentToken.value;
    
            if (symbolTable.find(assignedVar) == symbolTable.end()) {
                error("Переменная " + assignedVar + " не объявлена.", {"SEMICOLON"});
            } else if (symbolTable[assignedVar] != varType) {
                error("Несоответствие типов: переменной " + varName + " (типа " + varType + ") присваивается значение переменной " + assignedVar + " (типа " + symbolTable[assignedVar] + ").");
                return;
            }
            advance();
        } else if (currentToken.type == "BOOL") {  // Добавляем проверку на булевые значения
            if (varType != "bool") {
                error("Несоответствие типов: переменной " + varName + " (типа " + varType + ") присваивается булевое значение.");
                return;
            }
            advance();
        } else {
            error("Ожидался идентификатор, число или булевое значение после '='", {"NUMBER", "IDENTIFIER", "BOOL"});
        }
    }


    // <for> ::= 'for' '(' <declaration> ';' <bool_expression> ';' ')'
    void forStatement() {
        expect("LPAREN");
        declaration();
        expect("SEMICOLON");
        boolExpression();
        expect("SEMICOLON");
        expect("RPAREN");
        statement();
    }

    // <bool_expression> ::= <identifier> <relop> <identifier> | <number> <relop> <identifier>
    void boolExpression() {
        string firstOperandType;
        
        if (currentToken.type == "IDENTIFIER") {
            string varName = currentToken.value;
    
            if (symbolTable.find(varName) == symbolTable.end()) {
                error("Переменная " + varName + " не объявлена.", {"SEMICOLON"});
            }
            firstOperandType = symbolTable[varName];
            advance();
        } else if (currentToken.type == "NUMBER") {
            firstOperandType = "int";
            advance();
        } else {
            error("Ожидалось выражение типа <bool_expression>", {"IDENTIFIER", "NUMBER"});
            return;
        }
    
        expect("RELOP");
        
        if (currentToken.type == "IDENTIFIER") {
            string secondVarName = currentToken.value;
    
            if (symbolTable.find(secondVarName) == symbolTable.end()) {
                error("Переменная " + secondVarName + " не объявлена.", {"SEMICOLON"});
                cout << "ASSIGN " << currentToken.type << endl;
            } else if (symbolTable[secondVarName] != firstOperandType) {
                error("Несоответствие типов в булевом выражении: " + firstOperandType + " и " + symbolTable[secondVarName]);
            }
        } else if(currentToken.type == "NUMBER") {
            string secondVarName = currentToken.value;
            advance();
            
        } else {
            error("Ожидался идентификатор после оператора отношения.");
        }
    }


    // <if> ::= 'if' '(' <bool_expression> ')'
    void ifStatement() {
        expect("LPAREN");
        boolExpression();
        expect("RPAREN");
    }

    // Запуск парсера
    void parse() {
        program();
        if (errorCount == 0) {
            cout << "Синтаксический анализ успешно завершен." << endl;
        } else {
            cout << "Обнаружено ошибок: " << errorCount << endl;
        }
    }
};

int main() {
    string filename = "6";
    // cout << "Файл: ";
    // getline(cin, filename);

    Parser parser((filename + ".txt").c_str());
    parser.parse();

    return 0;
}
