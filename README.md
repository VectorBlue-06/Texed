# Texed 
A minimal terminal-based text editor written in C++ for Windows.  

##  Features
- Open, edit, and save text files  
- Simple keyboard-driven controls  
- Lightweight and distraction-free  
- Built using standard C++ 

##  Build
```bash
g++ main.cpp -o texted
```

## TEXED - Minimal Text Editor — v0.2

A lightweight, keyboard-driven console text editor for Windows.

### Features
- Menu-based startup:
  - Open existing file
  - Create new file
  - Read-only mode
- Command mode (`:`) with:
  - `e <n>` edit line
  - `w` save
  - `wq` save & quit
  - `q` quit
- Improved line editor:
  - Arrow keys + Home/End
  - ESC cancels edit safely
- Read-only mode enforcement (no edit/save)
- Centered intro screen with formatted commands
- Colored console UI (WinAPI)

### Fixes & Improvements
- Removed `system("cls")`, replaced with WinAPI screen clearing
- Safe file saving using temporary file swap
- Fixed cursor redraw and ESC deletion bug
- Cleaner command parsing and state handling

### Platform
- Windows only (WinAPI based)

### Planned
- Undo/redo
- Search
- Status bar
- Cross-platform support

