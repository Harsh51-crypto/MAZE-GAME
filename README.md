# 🧩 Maze Solver Game

Welcome to the **Maze Solver Game**, a fun and challenging puzzle game built with **C++ and SFML**, where logic meets adventure. Navigate mazes, beat the timer, and conquer each level with precision and speed!

---

## 🚀 Features

- 🔁 **Procedurally Generated Mazes** – A new challenge every time!
- 🧠 **Smart Maze Solving** – Put your logic to the test.
- ⏱️ **Countdown Timer** – Beat the clock before time runs out!
- 🎮 **Interactive UI** – Clean lobby, animated level selector, and game over screens.
- 🧱 **Object-Oriented Design** – Clean, modular, and extensible C++ code.
- 🎨 **Built with SFML** – Smooth graphics and event handling.

---

## 🎮 Gameplay

1. Start from the lobby and choose your level.
2. Use arrow keys to navigate through the maze.
3. Find the exit before time runs out.
4. Advance through levels and beat your own record!

---

## 📸 Screenshots

> *(Add images here)*  
> ![Lobby Screen](screenshots/lobby.png)  
> ![Maze Gameplay](screenshots/maze.png)  
> ![Game Over](screenshots/gameover.png)

---

## 🔧 Tech Stack

- **Language:** C++
- **Library:** [SFML](https://www.sfml-dev.org/)
- **Paradigm:** Object-Oriented Programming (OOP)

---

## 🛠️ How to Build and Run

### 🔗 Prerequisites

- C++ compiler (G++ recommended)
- SFML installed

### 🧪 Compilation (Linux/macOS/WSL)

```bash
g++ -o MazeGame main.cpp -lsfml-graphics -lsfml-window -lsfml-system
./MazeGame


💡 Windows
Use Code::Blocks or Visual Studio with SFML setup, or configure MinGW with the right SFML libraries

📁 Folder Structure
makefile
Copy
Edit
MazeSolverGame/
│
├── src/                # Source files (Player.cpp, Maze.cpp, Game.cpp...)
├── assets/             # Images, fonts, sounds
├── include/            # Header files
├── screenshots/        # Game screenshots
├── main.cpp            # Entry point
├── README.md           # You're here!
└── Makefile            # For Linux builds (optional)


 Future Improvements
🧭 Add hint or maze pathfinder mode

🧠 Implement AI that solves the maze

🌐 Add multiplayer or leaderboard support

📱 Port to mobile (SFML/SDL hybrid)

🙌 Credits
Developed by Mujataba Qureshi, Harsh Kumar, Faizan Nara  
Special thanks to the SFML community and all logic puzzle lovers!

🕹️ Let's Play!
“Every maze has an exit—
You just need to think like the way out exists.”

Ready to conquer the maze?

📜 License
This project is open-source and free to use under the MIT License.
