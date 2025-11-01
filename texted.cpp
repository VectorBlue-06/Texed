#include <iostream>
#include <fstream> //ofstream
#include <vector>  //all temp lines stored
#include <windows.h>
#include <conio.h>
#include <filesystem> //for filesystem::exists
#include <iomanip>    //for setw()-> for alignment of bullets

std::string editLine(const std::string &initial = "")
{
    std::string text = initial;
    int cursor = text.size();

    std::cout << "\r" << text;

    while (true)
    {
        int key = _getch();

        // ENTER key → finish editing
        if (key == 13)
            break;

        // ESC key → cancel
        else if (key == 27)
            return "";

        // BACKSPACE
        else if (key == 8)
        {
            if (cursor > 0)
            {
                text.erase(cursor - 1, 1);
                cursor--;
            }
        }

        // Arrow keys
        else if (key == 0 || key == 224)
        {
            key = _getch(); // get second code
            if (key == 75 && cursor > 0)
                cursor--; // Left
            else if (key == 77 && cursor < (int)text.size())
                cursor++; // Right
        }

        // Printable characters
        else if (key >= 32 && key <= 126)
        {
            text.insert(cursor, 1, (char)key);
            cursor++;
        }

        // redraw
        std::cout << "\r" << std::string(80, ' ') << "\r"; // clear line
        std::cout << text;
        std::cout << "\r" << text.substr(0, cursor); // move cursor
    }

    std::cout << "\n";
    return text;
}

// TODO-done- files not saving cause os using stream i and o at same time
// TODO-fixed- aligned bullets
// FIX e without number crashes the program
// FIX-fixed- edit mode still showing editing line after pressing enter

int main()
{

    system("cls");

    bool should_run = true;
    bool command_mode = false;
    bool edit_mode = false;
    bool does_file_exists = false;

    std::string path;
    std::cout << "path:";
    std::cin >> path;
    std::cin.ignore();

    system("cls");

    std::vector<std::string> lines;
    int line_count = 1;
    int line_count_load = 1; // number of lines from loaded file
    int line_editing;

    std::ifstream in_myfile(path);
    std::string temp;

    // the output starts from here
    system("CLS");
    std::cout << "\n   ------------START----------------" << std::endl;

    if (std::filesystem::exists(path))
    {
        std::string temp_load;

        while (std::getline(in_myfile, temp_load))
        {
            lines.push_back(temp_load);
            std::cout << std::setw(3) << line_count_load << "> "; // bullets for in_myfile
            std::cout << temp_load << std::endl;
            line_count_load++;
        }
        line_count = line_count_load;
    }

    while (should_run)
    {
        if (command_mode == false)
        {
            std::cout << std::setw(3) << line_count++ << "> ";

            std::getline(std::cin, temp);

            lines.push_back(temp);

            if (temp == ":")
                command_mode = true;
        }

        if (command_mode == true)
        {
            std::cout << "\r~";
            std::string cmd;
            std::cin >> cmd;

            if (cmd == "q")
            {
                should_run = false;
                break;
            }
            if (cmd == "n")
                command_mode = false;
            if (cmd.find("e") != std::string::npos)
            {
                edit_mode = true;

                size_t pos = cmd.find('e');
                try
                {
                    std::string num_str = cmd.substr(pos + 1);
                    int num = std::stoi(num_str);
                    std::cout << "line no : " << num << "\n";
                    line_editing = num - 1;
                    throw 1;
                }
                catch (...)
                {
                }
                command_mode = false;
            }
            std::cin.ignore();
        }

        if (edit_mode == true)
        {
            // std::cout<<line_editing<<"< "<<lines[line_editing];
            lines[line_editing] = editLine(lines[line_editing]);
            edit_mode = false;
        }
    }

    in_myfile.close();
    std::ofstream out_myfile(path);
    for (auto &ln : lines)
    {
        out_myfile << ln << "\n";
    }
    out_myfile.close();

    std::cout << "\n------------ENDS----------------" << std::endl;
    int i;
    std::cin >> i;

    return 0;
}