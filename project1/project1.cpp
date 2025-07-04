#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include <stack>
#include <vector>
#include <random>
#include <iostream>
using namespace std;
using namespace sf;
#define CELL_WIDTH 20

class Cell {
public:
    int x = 0, y = 0;
    int pos = 0;
    float size = CELL_WIDTH;
    float thickness = 2.f;
    bool walls[4] = { true, true, true, true };  // top,right,bottom,left//
    bool visited = false;
    bool isActive = false;

    Cell() {}
    Cell(int _x, int _y, int _pos) : x(_x), y(_y), pos(_pos) {}

    void draw(sf::RenderWindow& window) {
        RectangleShape rect;

        if (isActive) {
            rect.setFillColor(sf::Color(247, 23, 53));
            rect.setSize({ size, size });
            rect.setPosition(x, y);
            window.draw(rect);
        }

        rect.setFillColor(sf::Color(223, 243, 228));

        if (walls[0]) {
            rect.setSize({ size, thickness });
            rect.setPosition(x, y);
            window.draw(rect);
        }
        if (walls[1]) {
            rect.setSize({ thickness, size });
            rect.setPosition(x + size, y);
            window.draw(rect);
        }
        if (walls[2]) {
            rect.setSize({ size + thickness, thickness });
            rect.setPosition(x, y + size);
            window.draw(rect);
        }
        if (walls[3]) {
            rect.setSize({ thickness, size });
            rect.setPosition(x, y);
            window.draw(rect);
        }
    }
};

class Maze {
private:
    vector<Cell> cells;
    int size;

    void reset() {
        for (auto& cell : cells) {
            for (int i = 0; i < 4; i++) cell.walls[i] = true;
            cell.visited = false;
            cell.isActive = false;
        }
    }

    void removeWalls(Cell& current, Cell& neighbor) {
        int diff = current.pos - neighbor.pos;

        if (diff == size) {
            current.walls[0] = false; neighbor.walls[2] = false;
        }
        else if (diff == -1) {
            current.walls[1] = false; neighbor.walls[3] = false;
        }
        else if (diff == -size) {
            current.walls[2] = false; neighbor.walls[0] = false;
        }
        else if (diff == 1) {
            current.walls[3] = false; neighbor.walls[1] = false;
        }
    }

public:
    Maze(int _size) : size(_size) {
        cells.resize(size * size);
        for (int row = 0, k = 0; row < size; row++) {
            for (int col = 0; col < size; col++, k++) {
                int x = 30 + col * CELL_WIDTH;
                int y = 30 + row * CELL_WIDTH;
                cells[k] = Cell(x, y, k);
            }
        }
        generateMaze();
    }

    void generateMaze() {
        reset();
        stack<Cell*> stack;
        cells[0].visited = true;
        stack.push(&cells[0]);

        while (!stack.empty()) {
            Cell* current = stack.top();
            stack.pop();

            int pos = current->pos;
            std::vector<int> neighbors;

            if (pos % size != 0 && !cells[pos - 1].visited) neighbors.push_back(pos - 1);
            if ((pos + 1) % size != 0 && pos + 1 < size * size && !cells[pos + 1].visited) neighbors.push_back(pos + 1);
            if (pos + size < size * size && !cells[pos + size].visited) neighbors.push_back(pos + size);
            if (pos - size >= 0 && !cells[pos - size].visited) neighbors.push_back(pos - size);

            if (!neighbors.empty()) {
                stack.push(current);
                random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dist(0, neighbors.size() - 1);
                int nextPos = neighbors[dist(gen)];

                Cell* neighbor = &cells[nextPos];

                removeWalls(*current, *neighbor);
                neighbor->visited = true;
                stack.push(neighbor);
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& cell : cells) cell.draw(window);
    }

    Cell& getCell(int index) { return cells[index]; }

    int getSize() const { return size; }
};

class MazeAnimation {
private:
    RectangleShape miniMaze;
    vector<sf::RectangleShape> walls;
    CircleShape player;
    Vector2f position;
    Vector2f velocity;
    float rotation;
    float rotationSpeed;

public:
    MazeAnimation(float x, float y) {
        position = sf::Vector2f(x, y);

        // Create mini maze//
        miniMaze.setSize(sf::Vector2f(120, 120));
        miniMaze.setFillColor(sf::Color(20, 20, 40));
        miniMaze.setOutlineThickness(3);
        miniMaze.setOutlineColor(sf::Color(100, 100, 200));
        miniMaze.setOrigin(60, 60);

        // Create some walls inside the mini maze
        for (int i = 0; i < 6; i++) {
            sf::RectangleShape wall;
            wall.setFillColor(sf::Color(223, 243, 228));

            if (i % 2 == 0) {
                // Horizontal walls
                wall.setSize(sf::Vector2f(40, 4));
                wall.setPosition(-20 + (i / 2) * 10, -30 + (i / 2) * 15);
            }
            else {
                // Vertical walls
                wall.setSize(sf::Vector2f(4, 40));
                wall.setPosition(-30 + ((i - 1) / 2) * 20, -20 + ((i - 1) / 2) * 15);
            }
            walls.push_back(wall);
        }

        // Create player dot
        player.setRadius(5);
        player.setFillColor(sf::Color(247, 23, 53));
        player.setOrigin(5, 5);
        player.setPosition(0, 0);

        // Initialize animation properties
        velocity = sf::Vector2f(1.0f, 1.5f);
        rotation = 0.0f;
        rotationSpeed = 0.2f;
    }

    void update() {
        // Update position
        position += velocity;

        // Bounce off screen edges
        if (position.x < 200 || position.x > 1080) {
            velocity.x = -velocity.x;
        }

        if (position.y < 250 || position.y > 550) {
            velocity.y = -velocity.y;
        }

        // Rotate the maze
        rotation += rotationSpeed;

        // Move player inside the maze in a circular pattern
        float playerRadius = 30;
        float playerX = cos(rotation * 2) * playerRadius;
        float playerY = sin(rotation * 2) * playerRadius;
        player.setPosition(playerX, playerY);
    }

    void draw(sf::RenderWindow& window) {
        // Save current transform
        sf::Transform transform;
        transform.translate(position);
        transform.rotate(rotation);

        // Draw the maze
        window.draw(miniMaze, transform);

        // Draw the walls
        for (auto& wall : walls) {
            window.draw(wall, transform);
        }

        // Draw the player
        window.draw(player, transform);
    }
};

// Base Screen class
class Screen {
public:
    virtual ~Screen() = default;
    virtual int run() = 0;
};

class Lobby {
private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text title;
    sf::Text startText;
    sf::Text exitText;
    sf::RectangleShape startButton;
    sf::RectangleShape exitButton;
    bool startgame;
    sf::Texture lobbyimage;
    sf::Sprite lobbySprite;
    sf::SoundBuffer buffer;
    sf::Sound sound;

    // Hover state tracking
    bool startHovered;
    bool exitHovered;

    // Animation
    MazeAnimation mazeAnimation1;
    MazeAnimation mazeAnimation2;
    MazeAnimation mazeAnimation3;

public:
    Lobby(sf::RenderWindow& window) :
        window(window),
        startgame(false),
        startHovered(false),
        exitHovered(false),
        mazeAnimation1(200, 350),
        mazeAnimation2(window.getSize().x - 200, 250),
        mazeAnimation3(window.getSize().x / 2, 550)
    {
        if (!font.loadFromFile("Data/Roboto.ttf")) {
            cout << "Error loading font" << std::endl;
        }
        if (!lobbyimage.loadFromFile("Data/lobby.jpg")) {
            cout << "Error loading image" << std::endl;
        }
        if (!buffer.loadFromFile("Data/sound.wav")) {
            cout << "Error loading sound" << std::endl;
        }

        sound.setBuffer(buffer);
        sound.setLoop(true);
        sound.play();

        title.setFont(font);
        title.setString("Maze Game");
        title.setCharacterSize(80);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 100);

        // Start button setup
        startText.setFont(font);
        startText.setString("Press Enter to Start");
        startText.setCharacterSize(30);
        startText.setFillColor(sf::Color::White);

        // Exit button setup
        exitText.setFont(font);
        exitText.setString("Press Escape to Exit");
        exitText.setCharacterSize(30);
        exitText.setFillColor(sf::Color::White);

        // Create button rectangles
        const float padding = 20.0f;
        startButton.setSize({ startText.getGlobalBounds().width + padding * 2, startText.getGlobalBounds().height + padding * 2 });
        startButton.setFillColor(sf::Color(50, 50, 100));
        startButton.setOutlineThickness(2);
        startButton.setOutlineColor(sf::Color(100, 100, 200));

        exitButton.setSize({ exitText.getGlobalBounds().width + padding * 2, exitText.getGlobalBounds().height + padding * 2 });
        exitButton.setFillColor(sf::Color(100, 50, 50));
        exitButton.setOutlineThickness(2);
        exitButton.setOutlineColor(sf::Color(200, 100, 100));

        // Position the buttons
        startButton.setPosition(window.getSize().x / 2 - startButton.getSize().x / 2, 300);
        exitButton.setPosition(window.getSize().x / 2 - exitButton.getSize().x / 2, 400);

        // Position the text in the middle of the buttons
        startText.setPosition(
            startButton.getPosition().x + (startButton.getSize().x - startText.getGlobalBounds().width) / 2,
            startButton.getPosition().y + (startButton.getSize().y - startText.getGlobalBounds().height) / 2 - 15
        );

        exitText.setPosition(
            exitButton.getPosition().x + (exitButton.getSize().x - exitText.getGlobalBounds().width) / 2,
            exitButton.getPosition().y + (exitButton.getSize().y - exitText.getGlobalBounds().height) / 2 - 15
        );
    }

    bool run() {
        while (window.isOpen()) {
            handlevent();
            update();
            render();
            if (startgame) return true;
        }
        return false;
    }

    void update() {
        // Update animations
        mazeAnimation1.update();
        mazeAnimation2.update();
        mazeAnimation3.update();
    }

    void handlevent() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) startgame = true;
                else if (event.key.code == sf::Keyboard::Escape) window.close();
            }

            // Handle mouse movement for hover effects
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
                startHovered = startButton.getGlobalBounds().contains(mousePos);
                exitHovered = exitButton.getGlobalBounds().contains(mousePos);
            }

            // Handle mouse clicks on buttons
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (startButton.getGlobalBounds().contains(mousePos)) startgame = true;
                if (exitButton.getGlobalBounds().contains(mousePos)) window.close();
            }
        }

        // Update button colors based on hover status
        startButton.setFillColor(startHovered ? sf::Color(80, 80, 180) : sf::Color(50, 50, 100));
        exitButton.setFillColor(exitHovered ? sf::Color(180, 80, 80) : sf::Color(100, 50, 50));
    }

    void render() {
        window.clear(sf::Color(13, 2, 33));

        // Draw animated mazes
        mazeAnimation1.draw(window);
        mazeAnimation2.draw(window);
        mazeAnimation3.draw(window);

        window.draw(title);
        window.draw(startButton);
        window.draw(exitButton);
        window.draw(startText);
        window.draw(exitText);
        window.draw(lobbySprite);
        window.display();
    }
};

class LevelSelector : public Screen {
private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text title, easyText, mediumText, hardText;
    sf::RectangleShape easyButton, mediumButton, hardButton;
    bool easyHovered, mediumHovered, hardHovered;
    MazeAnimation mazeAnimation1;
    MazeAnimation mazeAnimation2;
    MazeAnimation mazeAnimation3;
    int selectedLevel;

public:
    LevelSelector(sf::RenderWindow& window) :
        window(window),
        easyHovered(false),
        mediumHovered(false),
        hardHovered(false),
        selectedLevel(0),
        mazeAnimation1(200, 350),
        mazeAnimation2(window.getSize().x - 200, 250),
        mazeAnimation3(window.getSize().x / 2, 550)
    {
        font.loadFromFile("Data/Roboto.ttf");

        title.setFont(font);
        title.setString("Select Difficulty");
        title.setCharacterSize(70);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 100);

        // Setup text and buttons
        setupOption(easyText, easyButton, "EASY LEVEL", 200, sf::Color(0, 100, 0));
        setupOption(mediumText, mediumButton, "MEDIUM LEVEL", 290, sf::Color(100, 100, 0));
        setupOption(hardText, hardButton, "HARD LEVEL", 380, sf::Color(100, 0, 0));
    }

    void setupOption(sf::Text& text, sf::RectangleShape& button, const std::string& str, float y, sf::Color baseColor) {
        // Set up text
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);

        // Create button with padding
        const float padding = 20.0f;
        text.setPosition(0, 0); // Temporary position to get bounds
        sf::FloatRect textBounds = text.getGlobalBounds();

        button.setSize({ textBounds.width + padding * 2, textBounds.height + padding * 2 });
        button.setFillColor(baseColor);
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(baseColor.r + 50, baseColor.g + 50, baseColor.b + 50));
        button.setPosition(window.getSize().x / 2 - button.getSize().x / 2, y);

        // Position text in the middle of button
        text.setPosition(
            button.getPosition().x + (button.getSize().x - textBounds.width) / 2,
            button.getPosition().y + (button.getSize().y - textBounds.height) / 2 - 15
        );
    }

    int run() override {
        selectedLevel = 0;

        while (window.isOpen() && selectedLevel == 0) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return -1;
                }

                // Check for mouse hovering over buttons
                if (event.type == sf::Event::MouseMoved) {
                    sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
                    easyHovered = easyButton.getGlobalBounds().contains(mousePos);
                    mediumHovered = mediumButton.getGlobalBounds().contains(mousePos);
                    hardHovered = hardButton.getGlobalBounds().contains(mousePos);

                    // Update button colors based on hover state
                    easyButton.setFillColor(easyHovered ? sf::Color(0, 150, 0) : sf::Color(0, 100, 0));
                    mediumButton.setFillColor(mediumHovered ? sf::Color(150, 150, 0) : sf::Color(100, 100, 0));
                    hardButton.setFillColor(hardHovered ? sf::Color(150, 0, 0) : sf::Color(100, 0, 0));
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (easyButton.getGlobalBounds().contains(mousePos)) {
                        selectedLevel = 15;
                    }
                    else if (mediumButton.getGlobalBounds().contains(mousePos)) {
                        selectedLevel = 25;
                    }
                    else if (hardButton.getGlobalBounds().contains(mousePos)) {
                        selectedLevel = 35;
                    }
                }

                // Keyboard shortcuts
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::E) {
                        selectedLevel = 15;
                    }
                    else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::M) {
                        selectedLevel = 25;
                    }
                    else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::H) {
                        selectedLevel = 35;
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        return -1; // Exit to main menu
                    }
                }
            }

            // Update animations
            mazeAnimation1.update();
            mazeAnimation2.update();
            mazeAnimation3.update();

            // Render
            window.clear(sf::Color(13, 2, 33));
            mazeAnimation1.draw(window);
            mazeAnimation2.draw(window);
            mazeAnimation3.draw(window);

            window.draw(title);
            window.draw(easyButton);
            window.draw(mediumButton);
            window.draw(hardButton);
            window.draw(easyText);
            window.draw(mediumText);
            window.draw(hardText);
            window.display();
        }

        return selectedLevel;
    }
};

class GameOverScreen : public Screen {
private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text title, playAgainText, exitText, changeDifficultyText;
    sf::RectangleShape playAgainButton, exitButton, changeDifficultyButton;
    bool playAgainHovered, exitHovered, changeDifficultyHovered;
    MazeAnimation mazeAnimation1, mazeAnimation2, mazeAnimation3;
    int result;

public:
    GameOverScreen(sf::RenderWindow& window) :
        window(window),
        playAgainHovered(false),
        exitHovered(false),
        changeDifficultyHovered(false),
        result(0),
        mazeAnimation1(200, 350),
        mazeAnimation2(window.getSize().x - 200, 250),
        mazeAnimation3(window.getSize().x / 2, 550)
    {
        font.loadFromFile("Data/Roboto.ttf");

        title.setFont(font);
        title.setString("Game Over");
        title.setCharacterSize(60);
        title.setFillColor(sf::Color::Red);
        title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 100);

        setupOption(playAgainText, playAgainButton, "Play Again", 200, sf::Color(0, 100, 0));
        setupOption(changeDifficultyText, changeDifficultyButton, "Change Difficulty", 280, sf::Color(50, 50, 100));
        setupOption(exitText, exitButton, "Exit", 360, sf::Color(100, 50, 50));
    }

    void setupOption(sf::Text& text, sf::RectangleShape& button, const std::string& str, float y, sf::Color baseColor) {
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);

        const float padding = 20.0f;
        text.setPosition(0, 0);
        sf::FloatRect textBounds = text.getGlobalBounds();

        button.setSize({ textBounds.width + padding * 2, textBounds.height + padding * 2 });
        button.setFillColor(baseColor);
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(baseColor.r + 50, baseColor.g + 50, baseColor.b + 50));
        button.setPosition(window.getSize().x / 2 - button.getSize().x / 2, y);

        text.setPosition(
            button.getPosition().x + (button.getSize().x - textBounds.width) / 2,
            button.getPosition().y + (button.getSize().y - textBounds.height) / 2 - 15
        );
    }

    int run() override {
        result = 0;

        while (window.isOpen() && result == 0) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return -1;
                }

                if (event.type == sf::Event::MouseMoved) {
                    sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
                    playAgainHovered = playAgainButton.getGlobalBounds().contains(mousePos);
                    exitHovered = exitButton.getGlobalBounds().contains(mousePos);
                    changeDifficultyHovered = changeDifficultyButton.getGlobalBounds().contains(mousePos);

                    playAgainButton.setFillColor(playAgainHovered ? sf::Color(0, 150, 0) : sf::Color(0, 100, 0));
                    exitButton.setFillColor(exitHovered ? sf::Color(150, 50, 50) : sf::Color(100, 50, 50));
                    changeDifficultyButton.setFillColor(changeDifficultyHovered ? sf::Color(80, 80, 180) : sf::Color(50, 50, 100));
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (playAgainButton.getGlobalBounds().contains(mousePos)) {
                        result = 1; // Play Again
                    }
                    else if (changeDifficultyButton.getGlobalBounds().contains(mousePos)) {
                        result = 2; // Change Difficulty
                    }
                    else if (exitButton.getGlobalBounds().contains(mousePos)) {
                        result = -1; // Exit
                    }
                }

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R) {
                        result = 1; // Play Again
                    }
                    else if (event.key.code == sf::Keyboard::D) {
                        result = 2; // Change Difficulty
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        result = -1; // Exit
                    }
                }
            }

            mazeAnimation1.update();
            mazeAnimation2.update();
            mazeAnimation3.update();

            window.clear(sf::Color(13, 2, 33));
            mazeAnimation1.draw(window);
            mazeAnimation2.draw(window);
            mazeAnimation3.draw(window);

            window.draw(title);
            window.draw(playAgainButton);
            window.draw(changeDifficultyButton);
            window.draw(exitButton);
            window.draw(playAgainText);
            window.draw(changeDifficultyText);
            window.draw(exitText);
            window.display();
        }

        return result;
    }
};

class CongratulationsScreen : public Screen {
private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text title, playAgainText, exitText, changeDifficultyText;
    sf::RectangleShape playAgainButton, exitButton, changeDifficultyButton;
    bool playAgainHovered, exitHovered, changeDifficultyHovered;
    MazeAnimation maze1, maze2, maze3;
    int result;

public:
    CongratulationsScreen(sf::RenderWindow& window) :
        window(window),
        playAgainHovered(false),
        exitHovered(false),
        changeDifficultyHovered(false),
        result(0),
        maze1(200, 350),
        maze2(window.getSize().x - 200, 250),
        maze3(window.getSize().x / 2, 550)
    {
        font.loadFromFile("Data/Roboto.ttf");

        title.setFont(font);
        title.setString("Congratulations!");
        title.setCharacterSize(60);
        title.setFillColor(sf::Color::Green);
        title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 100);

        setupOption(playAgainText, playAgainButton, "Play Again", 200, sf::Color(0, 100, 0));
        setupOption(changeDifficultyText, changeDifficultyButton, "Change Difficulty", 280, sf::Color(50, 50, 100));
        setupOption(exitText, exitButton, "Exit", 360, sf::Color(100, 50, 50));
    }

    void setupOption(sf::Text& text, sf::RectangleShape& button, const std::string& str, float y, sf::Color baseColor) {
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);

        const float padding = 20.0f;
        text.setPosition(0, 0);
        sf::FloatRect textBounds = text.getGlobalBounds();

        button.setSize({ textBounds.width + padding * 2, textBounds.height + padding * 2 });
        button.setFillColor(baseColor);
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(baseColor.r + 50, baseColor.g + 50, baseColor.b + 50));
        button.setPosition(window.getSize().x / 2 - button.getSize().x / 2, y);

        text.setPosition(
            button.getPosition().x + (button.getSize().x - textBounds.width) / 2,
            button.getPosition().y + (button.getSize().y - textBounds.height) / 2 - 15
        );
    }

    int run() override {
        result = 0;

        while (window.isOpen() && result == 0) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return -1;
                }

                if (event.type == sf::Event::MouseMoved) {
                    sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
                    playAgainHovered = playAgainButton.getGlobalBounds().contains(mousePos);
                    exitHovered = exitButton.getGlobalBounds().contains(mousePos);
                    changeDifficultyHovered = changeDifficultyButton.getGlobalBounds().contains(mousePos);

                    playAgainButton.setFillColor(playAgainHovered ? sf::Color(0, 150, 0) : sf::Color(0, 100, 0));
                    exitButton.setFillColor(exitHovered ? sf::Color(150, 50, 50) : sf::Color(100, 50, 50));
                    changeDifficultyButton.setFillColor(changeDifficultyHovered ? sf::Color(80, 80, 180) : sf::Color(50, 50, 100));
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (playAgainButton.getGlobalBounds().contains(mousePos)) {
                        result = 1; // Play Again
                    }
                    else if (changeDifficultyButton.getGlobalBounds().contains(mousePos)) {
                        result = 2; // Change Difficulty
                    }
                    else if (exitButton.getGlobalBounds().contains(mousePos)) {
                        result = -1; // Exit
                    }
                }

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R) {
                        result = 1; // Play Again
                    }
                    else if (event.key.code == sf::Keyboard::D) {
                        result = 2; // Change Difficulty
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        result = -1; // Exit
                    }
                }
            }

            maze1.update();
            maze2.update();
            maze3.update();

            window.clear(sf::Color(13, 2, 33));
            maze1.draw(window);
            maze2.draw(window);
            maze3.draw(window);

            window.draw(title);
            window.draw(playAgainButton);
            window.draw(changeDifficultyButton);
            window.draw(exitButton);
            window.draw(playAgainText);
            window.draw(changeDifficultyText);
            window.draw(exitText);
            window.display();
        }

        return result;
    }

    };


    class Game {
    public:
        bool difficultyChanged = false;
    private:
        sf::Clock clock;
        sf::Font font;
        sf::Text timerText, bestTimeText, countdownText, gameOverText;

        sf::RenderWindow& window;
        Maze maze;
        int currentPos = 0;
        int bestTime = -1;
        int countdownSeconds;
        bool showingGameOver = false;
        sf::Clock freezeClock;
        int mazesize;



        sf::RectangleShape currentHighlight, goalHighlight;

    public:
        Game(sf::RenderWindow& win, int mazeSize) : window(win), maze(mazeSize), mazesize(mazeSize) {
            font.loadFromFile("Data/Roboto.ttf");




            setupText(timerText, 30, 5, sf::Color::White);
            setupText(bestTimeText, 150, 5, sf::Color::Yellow);
            setupText(countdownText, 300, 5, sf::Color::Red);

            gameOverText.setFont(font);
            gameOverText.setString("Game Over");
            gameOverText.setCharacterSize(60);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setStyle(sf::Text::Bold);
            gameOverText.setPosition(200, 200);

            maze.getCell(currentPos).isActive = true;

            currentHighlight.setSize({ CELL_WIDTH, CELL_WIDTH });
            currentHighlight.setFillColor(sf::Color(166, 207, 213));

            goalHighlight.setSize({ CELL_WIDTH, CELL_WIDTH });
            goalHighlight.setFillColor(sf::Color(0, 128, 0));
            if (this->mazesize == 15) {
                countdownSeconds = 5;
            }
            else if (this->mazesize == 25) {
                countdownSeconds = 30;
            }
            else {
                countdownSeconds = 120;
            }
        }

        void setupText(sf::Text& text, float x, float y, sf::Color color) {
            text.setFont(font);
            text.setCharacterSize(20);
            text.setFillColor(color);
            text.setPosition(x, y);
        }

        void run() {
            while (window.isOpen() && !difficultyChanged) {  // Add !difficultyChanged condition
                handleEvents();
                update();
                render();
            }
        }

    private:
        void handleEvents() {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();
                if (event.type == sf::Event::KeyPressed && !showingGameOver) handleMovement(event.key.code);
            }
        }

        void handleMovement(sf::Keyboard::Key key) {
            Cell& current = maze.getCell(currentPos);
            int size = maze.getSize();

            int next = -1;
            if ((key == sf::Keyboard::Left || key == sf::Keyboard::A) && !current.walls[3]) next = currentPos - 1;
            else if ((key == sf::Keyboard::Right || key == sf::Keyboard::D) && !current.walls[1]) next = currentPos + 1;
            else if ((key == sf::Keyboard::Up || key == sf::Keyboard::W) && currentPos >= size && !current.walls[0]) next = currentPos - size;
            else if ((key == sf::Keyboard::Down || key == sf::Keyboard::S) && currentPos + size < size * size && !current.walls[2]) next = currentPos + size;

            if (next >= 0 && next < size * size) {
                current.isActive = false;
                currentPos = next;
                maze.getCell(currentPos).isActive = true;
            }

            if (key == sf::Keyboard::Escape) {
                GameOverScreen gameOverScreen(window);
                int result = gameOverScreen.run();
                if (result == 1) restartGame(); // Play Again
                else if (result == 2) {
                    difficultyChanged = true;
                    return; // This will exit the function, and the main run() loop will exit due to !difficultyChanged
                }
                else if (result == -1) window.close(); // Exit
            }

        }

        void update() {
            if (showingGameOver) {
                GameOverScreen gameOverScreen(window);
                int result = gameOverScreen.run();
                if (result == 1) restartGame(); // Play Again
                else if (result == 2) { difficultyChanged = true; return; } // Change Difficulty
                else if (result == -1) window.close(); // Exit
                showingGameOver = false;
                return;
            }


            if (currentPos == maze.getSize() * maze.getSize() - 1) {
                int elapsed = static_cast<int>(clock.getElapsedTime().asSeconds());
                if (bestTime == -1 || elapsed < bestTime) bestTime = elapsed;

                // Show Congratulations screen
                CongratulationsScreen congratsScreen(window);
                int result = congratsScreen.run();
                if (result == 1) restartGame(); // Play Again
                else if (result == 2) { difficultyChanged = true; return; } // Change Difficulty
                else if (result == -1) window.close(); // Exit
            }

            // Calculate remaining time
            int elapsed = static_cast<int>(clock.getElapsedTime().asSeconds());
            int remaining = countdownSeconds - elapsed;

            if (remaining < 0 && !showingGameOver) {
                showingGameOver = true;
                freezeClock.restart();


            }

            // Update timer and countdown text
            timerText.setString("Time: " + std::to_string(elapsed) + "s");
            if (bestTime != -1)
                bestTimeText.setString("Best: " + std::to_string(bestTime) + "s");
            else
                bestTimeText.setString("Best: --");
            countdownText.setString("Time Left: " + std::to_string(std::max(0, remaining)) + "s");
        }

        void render() {
            window.clear(sf::Color(13, 2, 33));
            maze.draw(window);

            currentHighlight.setPosition(maze.getCell(currentPos).x, maze.getCell(currentPos).y);
            window.draw(currentHighlight);

            goalHighlight.setPosition(maze.getCell(maze.getSize() * maze.getSize() - 1).x,
                maze.getCell(maze.getSize() * maze.getSize() - 1).y);
            window.draw(goalHighlight);

            window.draw(timerText);
            window.draw(bestTimeText);
            window.draw(countdownText);
            if (showingGameOver) window.draw(gameOverText);

            window.display();
        }

        void restartGame() {
            maze.generateMaze();
            currentPos = 0;
            maze.getCell(currentPos).isActive = true;
            clock.restart();
            showingGameOver = false;
        }
    };

    int main() {
        sf::RenderWindow window(sf::VideoMode(1000, 800), "Maze Game");
        window.setFramerateLimit(120);

        Lobby lobby(window);
        if (lobby.run()) {
            while (window.isOpen()) {
                Screen* screen = new LevelSelector(window);
                int size = screen->run();
                if (size > 0) {
                    Game game(window, size);
                    game.run();
                    if (game.difficultyChanged == true) {
                        delete screen; // Delete the LevelSelector screen
                        continue; // Restart the loop to show LevelSelector again
                    }
                    delete screen; // Delete the LevelSelector screen
                }
                else {
                    window.close(); // Exit if LevelSelector returns 0 or negative
                }
            }
        }
		return 0;

            

        }
        
        
    