#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <regex>

using namespace std;

// Структура, представляющая переход автомата
struct Transition 
{
	char state; // Состояние автомата
	string input; // Входной символ
	string stack; // Содержимое стека
	int index; // Индекс текущего перехода
	bool isTerminal; // Является ли переход терминальным

	Transition(char s, string p, string h, bool t) : state(s), input(p), stack(h), index(-1), isTerminal(t) { }
	Transition(char s, string p, string h) : state(s), input(p), stack(h), index(-1) { }
};

// Структура, представляющая аргументы перехода
struct TransitionArgs 
{
	char state; // Состояние
	char inputSymbol; // Входной символ
	char stackSymbol; // Символ в стеке

	TransitionArgs(char s, char p, char h) : state(s), inputSymbol(p), stackSymbol(h) { }
};

// Структура, представляющая значение в стеке
struct StackValue 
{
	char state;		// Состояние
	string content;	// Содержимое перехода

	StackValue(char s, string c) : state(s), content(c) { }
};

// Структура, представляющая команду автомата
struct Command 
{
	TransitionArgs args; // Аргументы перехода
	vector<StackValue> values; // Значения, связанные с командой

	Command(TransitionArgs f, vector<StackValue> v) : args(f), values(v) { }
};

// Класс для хранения и обработки автомата
class AutomatonStorage 
{
private:
	ifstream file; // Файл с грамматикой
	set<char> inputSymbols; // Терминальные символы
	set<char> nonTerminalSymbols; // Нетерминальные символы
	char initialState = '0', initialStackSymbol = '|', emptySymbol = '\0'; // Начальные значения
	vector<Command> commands; // Список команд автомата
	vector<Transition> transitionChain; // Цепочка переходов, работающая в процессе

public:
	AutomatonStorage(const char* filename) : file(filename) 
	{
		if (!file.is_open())
			throw runtime_error("Не удалось открыть файл для чтения\n");

		string tmpStr;
		int valueSize; // Размер значений в команде
		const regex exp("([[:upper:]])>([[:print:]]+)");
		smatch match; // Совпадение для хранения результатов разбора

		while (getline(file, tmpStr)) 
		{
			if (tmpStr.empty()) 
				continue;

			if (!regex_match(tmpStr, match, exp) || tmpStr.back() == '|' || tmpStr[2] == '|') 
			{
				throw runtime_error("Не удалось распознать синтаксис входного файла\n");
			}
			else 
			{
				nonTerminalSymbols.insert(match[1].str()[0]); // Добавляем нетерминальный символ
				commands.emplace_back(TransitionArgs(initialState, emptySymbol, match[1].str()[0]), vector<StackValue>());
				commands.back().values.push_back(StackValue(initialState, "")); // Инициализируем значения

				// Обработка входного символа
				for (char c : match[2].str())
				{
					if (c == '|')
					{
						if (commands.back().values.back().content.size() > 0)
							commands.back().values.push_back(StackValue(initialState, ""));
					}
					else
					{
						inputSymbols.insert(c); // Добавляем терминальный символ
						valueSize = commands.back().values.size();
						commands.back().values[valueSize - 1].content.push_back(c); // Добавляем символ в содержимое
					}
				}

				// Переворачиваем содержимое значений
				for (auto& value : commands.back().values)
					reverse(value.content.begin(), value.content.end());
			}
		}

		// Обработка символов в грамматике
		for (const auto& c : nonTerminalSymbols)
			inputSymbols.erase(c); // Убираем нетерминальные символы из терминальных

		// Добавляем команды для терминальных символов
		for (const auto& c : inputSymbols)
			commands.emplace_back(TransitionArgs(initialState, c, c), vector<StackValue>({ StackValue(initialState, "\0") }));

		// Добавляем команду для пустого символа
		commands.emplace_back(TransitionArgs(initialState, emptySymbol, initialStackSymbol), vector<StackValue>({ StackValue(initialState, "\0") }));
	}

	// Метод для отображения информации о грамматике
	void displayInfo() 
	{
		cout << "Входные алфавиты:\nP = {";
		for (const auto& c : inputSymbols)
			cout << c << ", ";
		cout << "\b\b}\n\n";
		cout << "Алфавит нетерминальных символов:\nZ = {";
		for (const auto& c : nonTerminalSymbols)
			cout << c << ", ";
		for (const auto& c : inputSymbols)
			cout << c << ", ";
		cout << "h0}\n\n";

		cout << "Список команд:\n";
		for (const auto& cmd : commands)
		{
			cout << "f(s" << cmd.args.state << ", ";
			if (cmd.args.inputSymbol == emptySymbol)
				cout << "lambda"; // Пустой символ
			else
				cout << cmd.args.inputSymbol;

			cout << ", ";
			if (cmd.args.stackSymbol == initialStackSymbol)
				cout << "h0"; // Начальный символ стека
			else
				cout << cmd.args.stackSymbol;

			cout << ") = {";
			for (const StackValue& v : cmd.values)
			{
				cout << "(s" << v.state << ", ";
				if (v.content[0] == emptySymbol)
					cout << "lambda"; // Пустой символ
				else
					cout << v.content;
				cout << "); ";
			}
			cout << "\b\b}\n";
		}
		cout << endl;
	}

	// Метод для отображения цепочки переходов
	void displayTransitionChain() 
	{
		cout << "\nЦепочка переходов: \n";
		for (const auto& transition : transitionChain)
			cout << "(s" << transition.state << ", " << ((transition.input.empty()) ? "lambda" : transition.input) << ", h0" << transition.stack << ") | ";
		cout << "(s0, lambda, lambda)" << endl; // Конечный переход
	}

	// Метод для добавления перехода в цепочку
	bool pushTransition()
	{
		int chainSize = transitionChain.size(); // Размер цепочки
		int commandSize, j, i;

		// Перебор команд
		for (i = 0; i < commands.size(); i++) {
			commandSize = transitionChain[chainSize - 1].stack.size(); // Размер стека
			// Проверка условий для перехода
			if (!transitionChain.empty() && transitionChain.back().input.size() != 0 && transitionChain.back().stack.size() != 0 
                && transitionChain[chainSize - 1].state == commands[i].args.state 
                && (transitionChain[chainSize - 1].input[0] == commands[i].args.inputSymbol || emptySymbol == commands[i].args.inputSymbol) 
                && transitionChain[chainSize - 1].stack[commandSize - 1] == commands[i].args.stackSymbol)
			{
				for (j = 0; j < commands[i].values.size(); j++)
				{
					if (commands[i].args.inputSymbol == emptySymbol)
					{
						// Добавляем переход без изменения входа
						transitionChain.push_back(Transition(commands[i].values[j].state, transitionChain[chainSize - 1].input, string(transitionChain[chainSize - 1].stack)));
					}
					else
					{
						// Добавляем переход с изменением входа
						transitionChain.push_back(Transition(commands[i].values[j].state, transitionChain[chainSize - 1].input, string(transitionChain[chainSize - 1].stack)));
						reverse(transitionChain[chainSize].input.begin(), transitionChain[chainSize].input.end()); // Переворачиваем вход
						transitionChain[chainSize].input.pop_back(); // Удаляем последний символ
						reverse(transitionChain[chainSize].input.begin(), transitionChain[chainSize].input.end()); // Снова переворачиваем
					}

					// Обновляем стек
					transitionChain[chainSize].stack.pop_back();
					transitionChain[chainSize].stack += commands[i].values[j].content;

					// Проверка на допустимость перехода
					if (transitionChain[chainSize].input.size() < transitionChain[chainSize].stack.size())
					{
						transitionChain.pop_back(); // Удаляем последний переход
						transitionChain.pop_back(); // Удаляем переход
						return false; // Переход недопустим
					}
					else
					{
						// Рекурсивная проверка переходов
						if ((transitionChain.back().input.empty() && transitionChain.back().stack.empty()) || pushTransition())
							return true; // Переход допустим
					}
				}
			}
		}
		// Если не найден ни один подходящий переход
		if (i == commands.size())
		{
			transitionChain.pop_back(); // Удаляем последний переход
			return false; // Переход недопустим
		}
	}

	// Метод для проверки входной строки
	bool checkInputLine(const string& str)
	{
		// Инициализация перехода с входной строкой
		if (commands[0].values.size() == 1)
			transitionChain.push_back(Transition(initialState, str, string(""), false));
		else
			transitionChain.push_back(Transition(initialState, str, string(""), true));

		transitionChain[0].stack.push_back(commands[0].args.stackSymbol); // Начальное состояние стека

		// Запуск проверки переходов
		bool result = pushTransition();
		if (result)
		{
			cout << "Валидная строка\n"; // Если строка валидна
			displayTransitionChain(); // Отображаем цепочку переходов
		}
		else {
			cout << "Невалидная строка\n"; // Если строка не валидна
		}
		transitionChain.clear(); // Очищаем цепочку переходов
		return result;
	}

	~AutomatonStorage() 
	{ 
		file.close(); 
	}
};

// Главная функция программы
int main(int argc, char* argv[]) 
{
	setlocale(LC_ALL, "Russian");
	string inputLine;
	try {
	    string file = "1";
	    cout << "Грамматика: ";
	    getline(cin, file);
		AutomatonStorage storage(("grammar" + file + ".txt").c_str()); // Создаем экземпляр автомата
		storage.displayInfo(); // Отображаем информацию о грамматике

		while (true)
		{
			cout << "Введите строку: \n";
			getline(cin, inputLine);
			storage.checkInputLine(inputLine); // Проверяем строку
			cout << endl;
		}
	}
	catch (const exception& err) {
		cerr << err.what() << endl;
	}
	return 0;
}
