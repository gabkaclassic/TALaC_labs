#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

string getAnsiColor(int textcolor, int bgcolor) {
    stringstream ss;
    ss << "\033[" << (30 + textcolor % 8) << ";"; // Цвет текста (30-37)
    ss << (40 + bgcolor % 8) << "m";              // Цвет фона (40-47)
    return ss.str();
}

string resetAnsiColor() {
    return "\033[0m";  // Сброс цветов
}

int getIndent(const string& halign, int columnWidth, const string& text) {
    int textLength = text.length();
    if (columnWidth <= 0) {
        return 0; // Если ширина колонки некорректна, не добавляем отступы
    }
    
    if (halign == "center") {
        return max((columnWidth - textLength) / 2, 0); // Не допускаем отрицательных отступов
    } else if (halign == "right") {
        return max(columnWidth - textLength, 0); // Не допускаем отрицательных отступов
    }
    
    return 0; // По умолчанию - выравнивание влево
}

// Структуры для хранения данных
struct Column {
    string valign = "top";
    string halign = "left";
    int textcolor = 15;  // По умолчанию белый текст
    int bgcolor = 0;     // По умолчанию чёрный фон
    int width = 0;
    string text;
};

struct Row {
    string valign = "top";
    string halign = "left";
    int textcolor = 15;
    int bgcolor = 0;
    int height = 1;
    vector<Column> columns;
};

struct Block {
    int rows = 0;
    int columns = 1;
    vector<Row> rowsData;
};

int safeStoi(const string& str, int defaultValue = 0) {
    if (str.empty()) {
        return defaultValue;
    }
    try {
        return stoi(str);
    } catch (const invalid_argument&) {
        cout << "Error: " << str << endl;
        return defaultValue;
    }
}

// Парсинг атрибутов
map<string, string> parseAttributes(const string& tag) {
    map<string, string> attributes;
    istringstream iss(tag);
    string token;
    while (iss >> token) {
        auto pos = token.find('=');
        if (pos != string::npos) {
            auto key = token.substr(0, pos);
            auto value = token.substr(pos + 1);
            attributes[key] = value;
        }
    }
    return attributes;
}

// Вывод блока
void printBlock(const Block& block) {
    for (const auto& row : block.rowsData) {
        for (const auto& column : row.columns) {
            
            cout << getAnsiColor(column.textcolor, column.bgcolor);
            
            int indent = getIndent(column.halign, column.width, column.text);
            cout << string(indent, ' ') << column.text << "\n"; // Выводим текст с отступом
            
            cout << resetAnsiColor();
        }
    }
}

Block parseEmark(const string& filename) {
    ifstream file(filename);
    string line;
    Block block;
    Row currentRow;
    Column currentColumn;
    bool inBlock = false, inRow = false, inColumn = false;

    while (getline(file, line)) {
        if (line.find("<block") != string::npos) {
            cout << "BLOCK " << endl;
            auto attrs = parseAttributes(line);
            block.rows = safeStoi(attrs["rows"], 0);      // Количество строк
            block.columns = safeStoi(attrs["columns"], 1); // Количество столбцов
            inBlock = true;
        } else if (line.find("<row") != string::npos) {
            cout << "ROW " << endl;
            auto attrs = parseAttributes(line);
            currentRow.valign = attrs["valign"];
            currentRow.halign = attrs["halign"];
            currentRow.height = safeStoi(attrs["height"], 1);
            currentRow.textcolor = safeStoi(attrs["textcolor"], 15);
            currentRow.bgcolor = safeStoi(attrs["bgcolor"], 0);
            currentRow.height = safeStoi(attrs["height"], 1);
            inRow = true;
        } else if (line.find("<column") != string::npos) {
            auto attrs = parseAttributes(line);
            cout << "COLUMN " << endl;
            currentColumn.valign = attrs["valign"];
            currentColumn.halign = attrs["halign"];
            currentColumn.textcolor = safeStoi(attrs["textcolor"], 15); // Белый по умолчанию
            currentColumn.bgcolor = safeStoi(attrs["bgcolor"], 0);      // Чёрный по умолчанию
            currentColumn.width = safeStoi(attrs["width"], 80);         // Ширина столбца по умолчанию
            inColumn = true;
        } else if (line.find("</column>") != string::npos) {
            cout << "COLUMN CLOSE " << endl;
            currentRow.columns.push_back(currentColumn);
            block.rowsData.push_back(currentRow);
            if (inRow) {
                inRow = false;
            }
            currentColumn = Column(); // Сбросить текущий столбец
            currentRow = Row();       // Сбросить текущую строку
            inColumn = false;
        } else if (line.find("</row>") != string::npos) {
            cout << "ROW CLOSE " << endl;
            block.rowsData.push_back(currentRow);
            currentRow = Row(); // Сбросить текущую строку
            inRow = false;
        } else if (line.find("</block>") != string::npos) {
            cout << "BLOCK CLOSE " << endl;
            inBlock = false;
        } else if (inColumn) {
            currentColumn.text += line; // Добавить текст в текущий столбец
        } else if (inRow) {
            // Если находимся в строке, добавляем текст в текущий столбец
            if (currentRow.columns.empty()) {
                currentColumn = Column();
                currentColumn.bgcolor = currentRow.bgcolor;
                currentColumn.textcolor = currentRow.textcolor;
                currentColumn.valign = currentRow.valign;
                currentColumn.halign = currentRow.halign;
            }
            currentColumn.text += line; // Добавить текст в текущий столбец
            if (currentRow.columns.empty()) {
                currentRow.columns.push_back(currentColumn);
            }
        }
    }

    return block;
}

int main() {
    string filename = "1";
    cout << "Файл: ";
    getline(cin, filename);
    Block block = parseEmark(filename + ".txt");
    printBlock(block);
    return 0;
}
