#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include <filesystem>
#include <sstream>
#include <iomanip>

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// ---------- Console ----------

void enableConsoleColors()
{
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_PROCESSED_OUTPUT;
    SetConsoleMode(hConsole, mode);
}

void clearScreen()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, ' ', cells, {0, 0}, &written);
    SetConsoleCursorPosition(hConsole, {0, 0});
}

void setColor(WORD c)
{
    SetConsoleTextAttribute(hConsole, c);
}

void resetColor()
{
    setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

// ---------- Intro ----------

void intro(string &path, bool &readOnly)
{
    clearScreen();

    setColor(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    system("cls");
    std::cout << "\n\n\n";
    std::cout << "                     __ __| ____|\\ \\  /__ __| ____| ___ \\         \n";
    std::cout << "                        |   __|   \\  /    |   __|   |   |           \n";
    std::cout << "                        |   |        \\    |   |     |   |           \n";
    std::cout << "                       _|  _____| _/\\_\\  _|  _____ |____/          \n\n\n";

    std::cout << "\n                       MINIMAL EDITOR v0.2\n";
    std::cout << "                Fast - Keyboard Driven - Minimal\n\n";
    resetColor();

    cout << "----------------------------------------\n";
    cout << "[1] Open existing file\n";
    cout << "[2] New file\n";
    cout << "[3] Read-only mode\n";
    cout << "----------------------------------------\n\n";

    cout << "Commands:\n";
    cout << "  :        command mode\n";
    cout << "  e <n>    edit line n\n";
    cout << "  w        save\n";
    cout << "  wq       save & quit\n";
    cout << "  q        quit\n\n";

    cout << "Choice: ";
    int choice;
    cin >> choice;
    cin.ignore();

    cout << "Path: ";
    getline(cin, path);

    if (choice == 3)
        readOnly = true;
}

// ---------- Editor ----------

string editLine(const string &initial)
{
    string text = initial;
    int cursor = text.size();

    while (true)
    {
        clearScreen();
        cout << text << "\n";
        cout << string(cursor, ' ') << "^\n";

        int key = _getch();

        if (key == 13)
            break; // ENTER
        if (key == 27)
            return initial; // ESC cancel

        if (key == 8 && cursor > 0)
        {
            text.erase(cursor - 1, 1);
            cursor--;
        }
        else if (key == 0 || key == 224)
        {
            key = _getch();
            if (key == 75 && cursor > 0)
                cursor--;
            else if (key == 77 && cursor < text.size())
                cursor++;
            else if (key == 71)
                cursor = 0;
            else if (key == 79)
                cursor = text.size();
        }
        else if (key >= 32 && key <= 126)
        {
            text.insert(cursor, 1, (char)key);
            cursor++;
        }
    }
    return text;
}

// ---------- Main ----------

enum Mode
{
    INSERT,
    COMMAND,
    EDIT
};

int main()
{
    ios::sync_with_stdio(false);
    enableConsoleColors();

    string path;
    bool readOnly = false;

    intro(path, readOnly);

    vector<string> lines;
    Mode mode = INSERT;
    bool running = true;
    int editIndex = -1;

    if (filesystem::exists(path))
    {
        ifstream in(path);
        string l;
        while (getline(in, l))
            lines.push_back(l);
    }

    clearScreen();

    while (running)
    {
        if (mode == INSERT)
        {
            cout << setw(3) << lines.size() + 1 << "> ";
            string input;
            getline(cin, input);

            if (input == ":")
                mode = COMMAND;
            else if (!readOnly)
                lines.push_back(input);
            else
                cout << "(read-only)\n";
        }
        else if (mode == COMMAND)
        {
            cout << "~ ";
            string cmd;
            getline(cin, cmd);

            istringstream iss(cmd);
            string c;
            iss >> c;

            if (c == "q")
                running = false;

            else if ((c == "w" || c == "wq") && !readOnly)
            {
                ofstream out(path + ".tmp");
                for (auto &l : lines)
                    out << l << "\n";
                out.close();
                filesystem::rename(path + ".tmp", path);

                if (c == "wq")
                    running = false;
            }
            else if (c == "e" && !readOnly)
            {
                int n;
                iss >> n;
                if (n > 0 && n <= lines.size())
                {
                    editIndex = n - 1;
                    mode = EDIT;
                    continue;
                }
            }
            else
                cout << "Invalid / read-only command\n";

            mode = INSERT;
        }
        else if (mode == EDIT)
        {
            lines[editIndex] = editLine(lines[editIndex]);
            mode = INSERT;
        }
    }

    resetColor();
    clearScreen();
    cout << "---- END ----\n";
    return 0;
}
