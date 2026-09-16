#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

bool exist(char from, std::string to)
{
    if (to.find(from) != std::string::npos)
        return true;
    else
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
std::string add(int& count, std::map<std::string, std::pair<int, int>>& a, std::string key, int type, std::string last_string)
{
    if (key != "")
    {
        if (a.find(key) == a.end())
        {
            a[key] = std::make_pair(count, type);
            count++;

        }
        last_string = last_string + std::to_string(a[key].first);
        last_string = last_string + ' ';
    }
    return last_string;
}




int main()
{

    std::ifstream file("example.txt"); // открываем файл для чтения

    std::string ignore = " \n";
    std::string numers = "1234567890";
    std::string operators = "*-+/=<>!~&|";
    std::string letter = "qwertyuiopasdfghjklzxcvbnm";
    std::string LETTER = "QWERTYUIOPASDFGHJKLZXCVBNM";

    if (!file.is_open()) {
        std::cout << "Не удалось открыть файл!" << std::endl;
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl; // вывод каждой строки
        lines.push_back(line+"\n");
    }

    std::map<std::string, std::pair<int, int>> alphavit;
    int count = 1;

    lettertype current_l = none_letter;


    std::string code;


    for (auto l : lines)
    {
        std::string lecsem = "";

        for (auto i : l)
        {
            if (exist(i, ignore))
            {    
                if ((current_l != none_letter)&&(i == '\n'))
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






    file.close(); // закрытие файла (необязательно, вызовется автоматически)
    return 0;
}

