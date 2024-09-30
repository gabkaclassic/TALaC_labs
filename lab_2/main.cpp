#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <sstream>
#include <unordered_set>
#include <algorithm>

using namespace std;

// Класс для представления состояния автомата
class State {
public:
    string name;
    bool isFinal;

    // Конструктор по умолчанию
    State() : name(""), isFinal(false) {}

    // Конструктор состояния
    State(const string& name, bool isFinal) : name(name), isFinal(isFinal) {}

    // Сравнение состояний по имени
    bool operator==(const State& other) const {
        return name == other.name;
    }

    // Оператор для использования State в контейнерах std::set и std::map
    bool operator<(const State& other) const {
        return name < other.name;
    }
};

// Класс для представления перехода между состояниями автомата
class Transition {
public:
    State from;
    char symbol;
    State to;

    // Конструктор перехода
    Transition(const State& from, char symbol, const State& to) : from(from), symbol(symbol), to(to) {}

    
    bool operator<(const Transition& other) const {
        // Сначала сравниваем по полю from (по состоянию, из которого исходит переход)
        if (from < other.from) return true;
        if (other.from < from) return false;

        // Затем сравниваем по символу перехода
        if (symbol < other.symbol) return true;
        if (other.symbol < symbol) return false;

        // И наконец, сравниваем по полю to (по состоянию, в которое осуществляется переход)
        return to < other.to;
    }
};

// Класс для представления конечного автомата
class FiniteAutomaton {
public:
    set<State> states;  // множество состояний
    set<Transition> transitions;  // множество переходов
    State initialState;  // начальное состояние
    set<State> finalStates;  // множество конечных состояний

    // Конструктор автомата
    FiniteAutomaton() : initialState("q0", false) {}

    // Добавление состояния
    void addState(const State& state) {
        states.insert(state);
    }

    // Добавление перехода
    void addTransition(const Transition& transition) {
        transitions.insert(transition);
    }

    // Установка начального состояния
    void setInitialState(const State& state) {
        initialState = state;
    }

    // Добавление конечного состояния
    void addFinalState(const State& state) {
        finalStates.insert(state);
    }

    // Проверка, является ли автомат детерминированным
    bool isDeterministic() const {
        map<State, map<char, State>> transitionMap;
        for (const Transition& transition : transitions) {
            if (transitionMap[transition.from].count(transition.symbol)) {
                return false;  // если существует несколько переходов с одинаковым символом, автомат не детерминирован
            }
            transitionMap[transition.from][transition.symbol] = transition.to;
        }
        return true;
    }

    // Функция для детерминизации недетерминированного автомата
    FiniteAutomaton determinize() const {
        FiniteAutomaton deterministicFA;
        map<set<State>, State> newStates;
        queue<set<State>> queue;
        set<State> initialSet = {initialState};
        queue.push(initialSet);
        newStates[initialSet] = State(getStateName(initialSet), isFinal(initialSet));
        deterministicFA.addState(newStates[initialSet]);
        deterministicFA.setInitialState(newStates[initialSet]);

        // Основной алгоритм детерминизации
        while (!queue.empty()) {
            set<State> currentSet = queue.front();
            queue.pop();
            State currentState = newStates[currentSet];

            map<char, set<State>> transitionsMap;
            for (const State& state : currentSet) {
                for (const Transition& transition : transitions) {
                    if (transition.from == state) {
                        transitionsMap[transition.symbol].insert(transition.to);
                    }
                }
            }

            // Обработка каждого перехода
            for (const auto& entry : transitionsMap) {
                char symbol = entry.first;
                set<State> nextSet = entry.second;
                if (!newStates.count(nextSet)) {
                    State newState = State(getStateName(nextSet), isFinal(nextSet));
                    newStates[nextSet] = newState;
                    deterministicFA.addState(newState);
                    if (isFinal(nextSet)) {
                        deterministicFA.addFinalState(newState);
                    }
                    queue.push(nextSet);
                }
                deterministicFA.addTransition(Transition(currentState, symbol, newStates[nextSet]));
            }
        }

        return deterministicFA;
    }

    // Функция для анализа строки с использованием автомата
    bool analyzeString(const string& input) const {
        State currentState = initialState;
        for (char c : input) {
            bool foundTransition = false;
            for (const Transition& transition : transitions) {
                if (transition.from == currentState && transition.symbol == c) {
                    currentState = transition.to;
                    foundTransition = true;
                    break;
                }
            }
            if (!foundTransition) {
                cout << "Нет перехода для символа: " << c << " из состояния: " << currentState.name << endl;
                return false;
            }
        }
        return finalStates.count(currentState) > 0;
    }

private:
    // Получение имени составного состояния
    string getStateName(const set<State>& states) const {
        stringstream ss;
        ss << "q{";
        for (const State& state : states) {
            ss << state.name << ",";
        }
        string result = ss.str();
        result.pop_back();  // удаляем последнюю запятую
        result += "}";
        return result;
    }

    // Проверка, является ли одно из состояний конечным
    bool isFinal(const set<State>& states) const {
        for (const State& state : states) {
            if (finalStates.count(state)) {
                return true;
            }
        }
        return false;
    }
};
// Функция для удаления пробелов в начале и конце строки
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return ""; // Строка состоит только из пробелов
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

// Основная функция программы
int main() {
    FiniteAutomaton fa;
    cout << "Введите имя файла с автоматом: ";
    string file;
    getline(cin, file);
    if (file.empty()) {
        file = "1";
    }

    // Чтение файла с автоматом
    ifstream fin(file + ".txt");
    if (!fin) {
        cerr << "Ошибка открытия файла!" << endl;
        return 1;
    }

    string line;
    while (getline(fin, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line.length() > 1 && line[0] == '/' && line[1] == '/') {
            continue;  // Пропускаем комментарии и пустые строки
        }

        int lastEquals = line.find_last_of('=');
        int firstComma = line.find(',');
        string fromStateName = line.substr(0, firstComma);
        char symbol = line[firstComma + 1];
        string toStateName = line.substr(lastEquals + 1);
        bool isFinal = toStateName[0] == 'f';

        State fromState(fromStateName, false);
        State toState(toStateName, isFinal);

        fa.addState(fromState);
        fa.addState(toState);
        fa.addTransition(Transition(fromState, symbol, toState));

        if (isFinal) {
            fa.addFinalState(toState);
        }
    }

    // Проверка, является ли автомат детерминированным
    cout << "Автомат детерминирован? " << (fa.isDeterministic() ? "Да" : "Нет") << endl;

    // Если недетерминирован, выполняем детерминизацию
    if (!fa.isDeterministic()) {
        FiniteAutomaton deterministicFA = fa.determinize();
        cout << "Переходы детерминированного автомата:" << endl;
        for (const Transition& transition : deterministicFA.transitions) {
            cout << transition.from.name << "," << transition.symbol << "=" << transition.to.name << endl;
        }
        fa = deterministicFA;
    }

    // Анализ строки с использованием автомата
    cout << "Введите строку для анализа: ";
    string inputString;
    getline(cin, inputString);
    cout << "Может ли автомат разобрать строку \"" << inputString << "\"? "
         << (fa.analyzeString(inputString) ? "Да" : "Нет") << endl;

    return 0;
}
