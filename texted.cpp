#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <conio.h>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// ---------- Console ----------

void clearScreen()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, ' ', cells, {0, 0}, &written);
    SetConsoleCursorPosition(hConsole, {0, 0});
}

// ---------- Intro ----------
// THE SCREEN WHICH SHOWS UP AT EVERY STARTUP
void intro_text()
{
    clearScreen();

    system("cls");
    std::cout << "\n\n";
    std::cout << "                  __ __| ____|\\ \\  /__ __| ____| ___ \\         \n";
    std::cout << "                     |   __|   \\  /    |   __|   |   |           \n";
    std::cout << "                     |   |        \\    |   |     |   |           \n";
    std::cout << "                    _|  _____| _/\\_\\  _|  _____ |____/          \n\n\n";

    std::cout << "\n                       MINIMAL EDITOR v0.2\n";
    std::cout << "                Fast - Keyboard Driven - Minimal\n\n";
    std::cout << "                        by VectorBlue-06\n\n";

    cout << "-----------------------------------------\n";
    cout << "           [1] Open existing file\n";
    cout << "           [2] New file\n";
    cout << "           [3] Read-only mode\n";
    cout << "-----------------------------------------\n\n";

    cout << "Commands:\n\n";
    cout << "  :           command mode\n";
    cout << "  e<n>        edit line n (e.g., e5 or e 5)\n";
    cout << "  d<n>        delete line n (e.g., d5 or d 5)\n";
    cout << "  d<n>,<m>    delete lines n,m (e.g., d1,3,5)\n";
    cout << "  d<n> to <m> delete lines n to m (e.g., d1 to 5 including 5)\n";
    cout << "  s           save\n";
    cout << "  sq          save & quit\n";
    cout << "  q           quit\n\n";
}

void intro_logic(string &path, bool &readOnly)
{

    while (true)
    {

        intro_text();

        cout << "Choice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            cout << "Path: ";
            getline(cin, path);

            // Open existing file
            if (!filesystem::exists(path))
            {
                cout << "\nWARNING: File does not exist or incorrect file name!\n";
                cout << "Please try again.\n\n";
                continue;
            }
            readOnly = false;
            break;
        }
        else if (choice == 2)
        {
            // New file
            cout << "New File Name: ";
            getline(cin, path);
            readOnly = false;
            break;
        }
        else if (choice == 3)
        {
            // Read-only mode
            cout << "Path: ";
            getline(cin, path);
            if (!filesystem::exists(path))
            {
                cout << "\nWARNING: File does not exist or incorrect file name!\n";
                cout << "Cannot open in read-only mode.\n\n";
                continue;
            }
            readOnly = true;
            break;
        }
        else
        {
            cout << "\nInvalid choice!\n\n";
        }
    }
}

// ---------- Editor ----------
// SMALL FUNCTION USED FOR EDITING THE TEXT WITH : e COMMAND

string editLine(const string &initial)
{
    string text = initial;
    int cursor = text.size();
    cout << "> " << "\r" << text;
    cout.flush();

    while (true)
    {
        int key = _getch();

        // ENTER → finish editing
        if (key == 13)
            break;

        // ESC → cancel edit
        else if (key == 27)
            return initial;

        // BACKSPACE
        else if (key == 8 && cursor > 0)
        {
            text.erase(cursor - 1, 1);
            cursor--;
        }

        // Arrow keys
        else if (key == 0 || key == 224)
        {
            key = _getch();
            if (key == 75 && cursor > 0)
                cursor--; // Left
            else if (key == 77 && cursor < (int)text.size())
                cursor++; // Right
            else if (key == 71)
                cursor = 0; // Home
            else if (key == 79)
                cursor = text.size(); // End
        }

        // Printable characters
        else if (key >= 32 && key <= 126)
        {
            text.insert(cursor, 1, (char)key);
            cursor++;
        }

        // Redraw line
        cout << "\r" << string(80, ' ') << "\r";
        cout << text;
        cout << "\r" << text.substr(0, cursor);
        cout.flush();
    }

    cout << "\n";
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

    string path;
    bool readOnly = false;

    intro_logic(path, readOnly);

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

    // Display file name at the first line
    cout << path << "\n";
    cout << "-------------------\n";

    // Display existing lines
    for (int i = 0; i < lines.size(); i++)
        cout << setw(3) << i + 1 << "> " << lines[i] << "\n";

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

            else if ((c == "s" || c == "sq") && !readOnly)
            {
                ofstream out(path + ".tmp");
                for (auto &l : lines)
                    out << l << "\n";
                out.close();
                filesystem::rename(path + ".tmp", path);

                if (c == "sq")
                    running = false;
            }
            else if ((c == "e" || c[0] == 'e') && !readOnly)
            {
                // Handle e3 or e 3 format
                int n = 0;
                if (c.size() > 1)
                    n = stoi(c.substr(1)); // e3 format
                else
                    iss >> n; // e 3 format

                if (n > 0 && n <= lines.size())
                {
                    editIndex = n - 1;
                    mode = EDIT;
                    continue;
                }
                else
                    cout << "Invalid line number\n";
            }
            else if ((c == "d" || c[0] == 'd') && !readOnly)
            {
                // Handle delete command: d3, d 3, d1,3,5, d1 to 5 (spaces optional)
                vector<int> linesToDelete;
                string delCmd = cmd;

                // Remove 'd' from the start and trim spaces
                size_t pos = delCmd.find('d');
                if (pos != string::npos)
                    delCmd = delCmd.substr(pos + 1);

                // Trim leading spaces
                delCmd.erase(0, delCmd.find_first_not_of(' '));

                auto removeSpaces = [](string value)
                {
                    value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());
                    return value;
                };

                string compact = removeSpaces(delCmd);
                size_t toPos = compact.find("to");

                if (toPos != string::npos)
                {
                    // Handle range: d1to5, d1 to 5, d 1 to5, etc.
                    try
                    {
                        int start = stoi(compact.substr(0, toPos));
                        int end = stoi(compact.substr(toPos + 2));
                        if (start > 0 && end > 0 && start <= lines.size() && end <= lines.size() && start <= end)
                        {
                            for (int i = start; i <= end; i++)
                                linesToDelete.push_back(i - 1);
                        }
                        else
                            cout << "Invalid range\n";
                    }
                    catch (...)
                    {
                        cout << "Invalid delete syntax\n";
                    }
                }
                else if (delCmd.find(',') != string::npos)
                {
                    // Handle comma-separated: d1,3,5
                    stringstream ss(delCmd);
                    string token;
                    try
                    {
                        while (getline(ss, token, ','))
                        {
                            // Trim spaces from token
                            token.erase(0, token.find_first_not_of(' '));
                            token.erase(token.find_last_not_of(' ') + 1);
                            int num = stoi(token);
                            if (num > 0 && num <= lines.size())
                                linesToDelete.push_back(num - 1);
                        }
                    }
                    catch (...)
                    {
                        cout << "Invalid delete syntax\n";
                    }
                }
                else
                {
                    // Handle single line: d3 or d 3
                    try
                    {
                        int num = stoi(delCmd);
                        if (num > 0 && num <= lines.size())
                            linesToDelete.push_back(num - 1);
                        else
                            cout << "Line number out of range\n";
                    }
                    catch (...)
                    {
                        cout << "Invalid line number\n";
                    }
                }

                // Delete lines in reverse order to avoid index shifting
                if (!linesToDelete.empty())
                {
                    sort(linesToDelete.begin(), linesToDelete.end(), greater<int>());
                    for (int idx : linesToDelete)
                        lines.erase(lines.begin() + idx);

                    clearScreen();
                    cout << path << "\n";
                    cout << "-------------------\n";
                    for (int i = 0; i < lines.size(); i++)
                        cout << setw(3) << i + 1 << "> " << lines[i] << "\n";
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
            // Redraw all lines after editing
            clearScreen();
            cout << path << "\n";
            cout << "-------------------\n";
            for (int i = 0; i < lines.size(); i++)
                cout << setw(3) << i + 1 << "> " << lines[i] << "\n";
        }
    }

    clearScreen();
    cout << "---- END ----\n";
    return 0;
}
