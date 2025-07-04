
Note: **Make sure you setup of Visual studio 2022 not Visual Studio Code**

🧩 How to Install and Set Up SFML for This Project
✅ Prerequisites
A C++ compiler (e.g. g++, Visual Studio)

SFML 2.5+

(Optional) CMake for easier building


🧩 How to Install and Set Up SFML for This Project
✅ Prerequisites
A C++ compiler (e.g. g++, Visual Studio)

SFML 2.5+

(Optional) CMake for easier building


C:\SFML\
├── include\   → SFML headers
├── lib\       → .a or .dll files

🔹 Step 3: Compile With g++
Use the following compile command:

g++ src/main.cpp -o MazeGame -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system

 Step 4: Copy DLL Files
From C:\SFML\bin, copy all required .dll files (like sfml-graphics-2.dll, etc.) to your project folder (where the .exe is).

❓ Common SFML Issues
Problem	Fix
Missing DLL error	Copy SFML .dll files to your .exe location
SFML not found	Double-check include/lib paths and installation
Undefined references	Make sure you’re linking: -lsfml-graphics -lsfml-window -lsfml-system
