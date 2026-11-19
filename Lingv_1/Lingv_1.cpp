#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <stack>
#include <cctype>

bool exist(char from, std::string to)
{
    if (to.find(from) != std::string::npos)
        return true;
    else
        return false;
}
bool exist(std::string from, std::vector<std::string> to)
{
    for (auto i : to)
        if (from == i)
            return true;
    return false;
}

enum lettertype
{
    operator_,
    words,
    syntaxis,
    numbers,
    newstring,
    none_letter
};

bool CheckForReserved(std::string letter)
{
    std::vector<std::string> reserved{ "TYPE", "if" };
    for (auto it : reserved)
    {
        if (it == letter)
            return true;
    }
    return false;

}
//std::map<name, std::pair<id, type>>
std::string add(int& count, std::map<std::string, std::pair<int, int>>& a, std::string key, int type, std::string last_string)
{
    if (key != "")
    {
        if (a.find(key) == a.end())
        {
            a[key] = std::make_pair(count, type);
            count++;

        }
        if (type == numbers)
        {
            last_string = last_string + std::to_string(a["<const>"].first) + " ";
        }
        if (type == words)
        {
            if (!CheckForReserved(key))
                last_string = last_string + std::to_string(a["<ident>"].first) + " ";
        }
        last_string = last_string + std::to_string(a[key].first);
        last_string = last_string + ' ';
    }
    return last_string;
}
int getPriority(const std::string& letter)
{
    if (exist(letter, std::vector<std::string>{"=", "TYPE", "if"}))
        return 1;
    if (exist(letter, std::vector<std::string>{","}))
        return 2;
    if (exist(letter, std::vector<std::string>{"*"}))
        return 3;
    if (exist(letter, std::vector<std::string>{"."}))
        return 4;
    return 0; // не оператор
}


void task1(std::string* result_code, std::map<std::string, std::pair<int, int>>* result_alp, std::string name = "example.txt")
{
    std::ifstream file(name); // открываем файл для чтения

    std::string ignore = " \n";
    std::string numers = "1234567890";
    std::string operators = "*-+/=<>!~&|";
    std::string letter = "qwertyuiopasdfghjklzxcvbnm";
    std::string LETTER = "QWERTYUIOPASDFGHJKLZXCVBNM";

    if (!file.is_open()) {
        std::cout << "Не удалось открыть файл!" << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl; // вывод каждой строки
        lines.push_back(line + "\n");
    }

    std::map<std::string, std::pair<int, int>> alphavit;
    int count = 0;

    add(count, alphavit, "<const>", none_letter, "");
    add(count, alphavit, "<ident>", none_letter, "");

    lettertype current_l = none_letter;

    std::string code;

    for (auto l : lines)
    {
        std::string lecsem = "";

        for (auto i : l)
        {
            if (exist(i, ignore))
            {
                if ((current_l != none_letter) && (i == '\n'))
                {
                    code = add(count, alphavit, lecsem, current_l, code);
                }
                continue;
            }


            if (exist(i, letter) || exist(i, LETTER))
            {

                if ((current_l != words) && (current_l != none_letter))
                {                    
                    code = add(count, alphavit, lecsem, current_l, code);
                    lecsem = "";
                }
                lecsem = lecsem + i;
                current_l = words;

            }
            else if (exist(i, numers))
            {
                if ((current_l != numbers) && (current_l != none_letter))
                {
                    code = add(count, alphavit, lecsem, current_l, code);
                    lecsem = "";
                }

                lecsem = lecsem + i;
                current_l = numbers;
            }
            else if (exist(i, operators))
            {
                if ((current_l != operator_) && (current_l != none_letter))
                {
                    code = add(count, alphavit, lecsem, current_l, code);
                    lecsem = "";
                }
                lecsem = lecsem + i;
                current_l = operator_;
            }
            else
            {
                if ((current_l != syntaxis) && (current_l != none_letter))
                {
                    code = add(count, alphavit, lecsem, current_l, code);
                    lecsem = "";
                }
                lecsem = lecsem + i;
                current_l = syntaxis;
            }
        }
        code = code + "\n";
    }

    for (const auto& kv : alphavit) {
        std::cout << kv.first << "  : " << kv.second.first << "\n";
    }

    std::cout << code;
    *result_code = code;
    *result_alp = alphavit;
    file.close();
}
class AlphavitLecsem
{
private:
    std::map<std::string, std::pair<int, int>> alphavit_str_to_id;
    std::map<int, std::pair<std::string, int>> alphavit_id_to_str;

public:
    AlphavitLecsem(std::map<std::string, std::pair<int, int>> alphavit)
    {
        alphavit_str_to_id = alphavit;

        for (const auto& item : alphavit_str_to_id) {
            const std::string& str = item.first;
            int id = item.second.first;
            int additional_param = item.second.second;

            alphavit_id_to_str[id] = std::make_pair(str, additional_param);
        }
    }

    const std::pair<int, int> operator[](const std::string& str) const 
    {
        try 
        {
            return alphavit_str_to_id.at(str);
        }
        catch (const std::out_of_range& e) 
        {
            return std::pair<int, int>(0, 0);
        }
    }

    const std::pair<std::string, int> operator[](int id) const
    {
        try
        {
            return alphavit_id_to_str.at(id);
        }
        catch (const std::out_of_range& e)
        {
            return std::pair<std::string, int>("", 0);
        }
    }
};

// Правоассоциативные операторы
bool isRightAssoc(const std::string& op)
{
    return (op == "=" || op == "TYPE" || op == "if");
}

void task2(std::string result_code,
    std::map<std::string, std::pair<int, int>> result_alp)
{
    AlphavitLecsem alph(result_alp);

    int idIdent = result_alp["<ident>"].first;
    int idConst = result_alp["<const>"].first;

    int idOpen = -1, idClose = -1;
    for (auto& kv : result_alp)
    {
        if (kv.first == "(") idOpen = kv.second.first;
        if (kv.first == ")") idClose = kv.second.first;
    }

    // ---- 1. Разбор result_code на лексемы (с парами 1 N / 0 N) ----
    struct Lex { bool isOperandPair; int head; int tail; bool newline; };
    std::vector<Lex> lexs;

    std::vector<int> raw;
    std::vector<bool> nl;
    {
        std::string cur;
        for (char c : result_code)
        {
            if (std::isdigit((unsigned char)c)) cur += c;
            else
            {
                if (!cur.empty()) { raw.push_back(std::stoi(cur)); nl.push_back(c == '\n'); cur.clear(); }
                else if (c == '\n' && !raw.empty()) nl.back() = true;
            }
        }
        if (!cur.empty()) { raw.push_back(std::stoi(cur)); nl.push_back(true); }
    }

    for (size_t i = 0; i < raw.size(); ++i)
    {
        int id = raw[i];
        if ((id == idIdent || id == idConst) && i + 1 < raw.size())
        {
            lexs.push_back({ true, id, raw[i + 1], nl[i + 1] });
            ++i;
        }
        else
            lexs.push_back({ false, id, 0, nl[i] });
    }

    // ---- 2. Помощники ----
    auto idToStr = [&](int id) -> std::string { return alph[id].first; };
    auto idPriority = [&](int id) -> int { return getPriority(idToStr(id)); };
    auto idType = [&](int id) -> int { return alph[id].second; };

    auto isOperator = [&](int id) -> bool {
        return idPriority(id) != 0 || idType(id) == operator_;
        };
    auto isPrefixMarker = [&](int id) -> bool {
        std::string s = idToStr(id);
        return s == "if" || s == "TYPE";
        };

    // ---- 3. Сортировочная станция ----
    std::vector<Lex> output;
    std::stack<int> opStack;

    auto pushOp = [&](int op) {
        while (!opStack.empty() && opStack.top() != idOpen)
        {
            int top = opStack.top();
            int pTop = idPriority(top);
            int pCur = idPriority(op);
            if (pTop >= pCur) { output.push_back({ false, top, 0, false }); opStack.pop(); }
            else break;
        }
        opStack.push(op);
        };

    for (auto& L : lexs)
    {
        if (L.isOperandPair)
            output.push_back(L);
        else
        {
            int id = L.head;
            if (id == idOpen)
                opStack.push(id);
            else if (id == idClose)
            {
                while (!opStack.empty() && opStack.top() != idOpen)
                {
                    output.push_back({ false, opStack.top(), 0, false }); opStack.pop();
                }
                if (!opStack.empty()) opStack.pop();
            }
            else if (isPrefixMarker(id))
                output.push_back(L);         // if / TYPE — сразу в выход
            else if (isOperator(id))
                pushOp(id);
            else
                output.push_back(L);
        }

        if (L.newline)
        {
            while (!opStack.empty() && opStack.top() != idOpen)
            {
                output.push_back({ false, opStack.top(), 0, false }); opStack.pop();
            }
        }
    }
    while (!opStack.empty())
    {
        output.push_back({ false, opStack.top(), 0, false }); opStack.pop();
    }

    // ---- 4. Печать ----
    std::cout << "\n===== POSTFIX (POLIZ) =====\n";
    for (auto& L : output)
    {
        if (L.isOperandPair)
            std::cout << idToStr(L.head) << " " << L.tail << "  ";
        else
            std::cout << idToStr(L.head) << "(" << L.head << ")  ";
    }
    std::cout << "\n===========================\n";
}
std::string getNextWord(std::string source) 
{
    if (source.empty()) {
        return "";
    }

    std::istringstream stream(source);
    std::string word;

    if (stream >> word) {
        std::streampos pos = stream.tellg();
        if (pos != -1) {
            source = source.substr(static_cast<size_t>(pos));
        }
        else {
            source.clear();
        }
        return word;
    }

    return "";
}
std::string popNextWord(std::string& source) {
    if (source.empty()) {
        return "";
    }

    std::istringstream stream(source);
    std::string word;

    // Считываем первое слово
    if (stream >> word) {
        // Находим позицию, где закончилось это слово в исходной строке
        // tellg() вернет позицию после прочитанного слова и пробелов
        std::streampos pos = stream.tellg();

        if (pos != -1) {
            // Обрезаем строку, оставляя только остаток
            source = source.substr(static_cast<size_t>(pos));
        }
        else {
            // Если достигли конца, очищаем строку полностью
            source.clear();
        }

        return word;
    }

    return "";
}
int get_num(std::string& source)
{
    std::string numStr = getNextWord(source);

    if (!numStr.empty()) {
        int number = std::stoi(numStr);
        return number;
    }
    else
        return -1;
}
int pop_num(std::string& source)
{
    std::string numStr = popNextWord(source);

    if (!numStr.empty()) {
        int number = std::stoi(numStr);
        return number;
    }
    else
        return -1;
}
class expression {
public:
    expression(expression* parent, std::string name)
    {
        this->parent = parent;
        count = -1;
    }
    expression() : expression(nullptr, "PROGRAM")
    {
        
    }
    expression& append(int lecsem)
    {
        stack.push_back(lecsem);
        return *this;
    }
    expression& add(int lecsem)
    {
        child.push_back(new expression (this, ""));
        stack.push_back(count);
        count--;


        return *child[child.size()-1];
    }
    expression& close(int lecsem = 0)
    {
        return *parent;
    }
    int end()
    {
        return stack[stack.size() - 1];
    }
private:
    std::string name;
    std::vector<int> stack;
    std::vector<expression*> child;
    expression* parent;
    int count;
};
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}
int recursion(char TABLE[16][16], std::string& result_code, expression& current, int prev_lecsem, int deep, AlphavitLecsem result_alp)
{
    int lecsem = get_num(result_code);

    for (int lecsem = get_num(result_code); lecsem >= 0; lecsem = get_num(result_code))
    {
        if (TABLE[prev_lecsem][lecsem] == '<')
        {
            pop_num(result_code);
            if ((lecsem == 0) || (lecsem == 1))
                pop_num(result_code);
            current = current.add(lecsem);
            /*if (lecsem <= 13)
                std::cout << std::string("  ", deep + 1) << result_alp[lecsem].first << std::endl;*/
            recursion(TABLE, result_code, current, lecsem, deep + 1, result_alp);
            continue;

        }
        if (TABLE[prev_lecsem][lecsem] == '=')
        {
            pop_num(result_code);
            if ((lecsem == 0) || (lecsem == 1))
                pop_num(result_code);
            current = current.append(lecsem);
            /*if (lecsem <= 13)
                std::cout << std::string("  ", deep) << result_alp[lecsem].first << std::endl;*/
        }
        if (TABLE[prev_lecsem][lecsem] == '>')
        {
            current = current.close();
            return prev_lecsem;
        }
        prev_lecsem = lecsem;
    }
}
void task3(std::string result_code,
    std::map<std::string, std::pair<int, int>> result_alp)
{
    char TABLE[16][16] = {
        //       <c>  <i>  MAX   =    0    if   (    X    .   GT    )    T    *    ,    \n
        /* 0 */ {' ', ' ', '*', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ', ' ', '>'}, // <const>
        /* 1 */ {'<', ' ', '*', '=', '*', ' ', ' ', '*', '=', '*', '>', ' ', ' ', ' ', '>'}, // <ident>
        /* - */ {'*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'}, // MAX
        /* 3 */ {'<', '<', '*', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ', ' ', '>'}, // =
        /* - */ {'*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'}, // 0
        /* 5 */ {' ', ' ', '*', ' ', '*', ' ', '=', '*', ' ', '*', ' ', ' ', ' ', ' ', '>'}, // if
        /* 6 */ {' ', '<', '*', ' ', '*', ' ', ' ', '*', ' ', '*', '=', ' ', ' ', ' ', '>'}, // (
        /* - */ {'*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'}, // X
        /* 8 */ {' ', '=', '*', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ', ' ', '>'}, // .
        /* - */ {'*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'}, // GT
        /* 10*/ {' ', '<', '*', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ', ' ', '>'}, // )
        /* 11*/ {' ', ' ', '*', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', '<', ' ', '>'}, // TYPE
        /* 12*/ {' ', ' ', '*', ' ', '*', ' ', ' ', '*', ' ', '*', '>', ' ', ' ', '=', '>'}, // * 
        /* 13*/ {' ', '<', '*', ' ', '*', ' ', ' ', '*', ' ', '*', ' ', ' ', ' ', ' ', '>'}, // ,
        /* 14*/ {' ', '<', '*', ' ', '*', '<', ' ', '*', ' ', '*', ' ', '<', ' ', ' ', '='}  // \n
    };
    replaceAll(result_code, "\n", "14 ");
    expression start;
    AlphavitLecsem alph(result_alp);

    recursion(TABLE, result_code, start, 14, 0, alph);





 
}
int main()
{
    std::string result_code;
    std::map<std::string, std::pair<int, int>> result_alp;
    task1(&result_code, &result_alp);
    task2(result_code, result_alp);
    task3(result_code, result_alp);

}

