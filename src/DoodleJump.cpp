#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<Windows.h>
#include <ctime>
#include <string>
#include "DoodleHomePage.cpp";

using namespace std;
using namespace sf;

struct Platform {
    float x, y;
};

bool pixelPerfectCollision(const Sprite& sprite1, const Sprite& sprite2) {
    // Get the bounding boxes of the sprites
    FloatRect intersection;
    if (!sprite1.getGlobalBounds().intersects(sprite2.getGlobalBounds(), intersection)) {
        return false;
    }

    // Get the texture data for both sprites
    const Image& image1 = sprite1.getTexture()->copyToImage();
    const Image& image2 = sprite2.getTexture()->copyToImage();

    // Calculate the positions of the intersection area in the texture space
    Vector2f position1 = sprite1.getPosition();
    Vector2f position2 = sprite2.getPosition();

    for (int i = 0; i < intersection.width; i++) {
        for (int j = 0; j < intersection.height; j++) {
            int x1 = static_cast<int>(intersection.left - position1.x + i);
            int y1 = static_cast<int>(intersection.top - position1.y + j);
            int x2 = static_cast<int>(intersection.left - position2.x + i);
            int y2 = static_cast<int>(intersection.top - position2.y + j);

            if (image1.getPixel(x1, y1).a != 0 && image2.getPixel(x2, y2).a != 0) {
                return true;
            }
        }
    }
    return false;
}


int runGame()
{
    HomePage H1;

    srand(time(nullptr));

    RenderWindow window(VideoMode(800, 600), "Doodle Jump!");
    window.setFramerateLimit(70);

    // Load fireball textures
    vector<Texture> fireballTextures;
    for (int i = 0; i < 26; ++i) {
        Texture texture;
        string filename = "fire/efecto_fuego_" + to_string(i) + ".png";
        if (!texture.loadFromFile(filename)) {
            cout << "Error loading " << filename << endl;
            return -1;
        }
        fireballTextures.push_back(texture);
    }

    // Load other textures
    Texture doodleTextureLeft;
    if (!doodleTextureLeft.loadFromFile("images/SpdoodleLeft.png")) {
        cout << "Error loading doodle.png" << endl;
        return -1;
    }

    Texture doodleTextureRight;
    if (!doodleTextureRight.loadFromFile("images/SpdoodleRight.png")) {
        cout << "Error loading doodle.png" << endl;
        return -1;
    }

    Texture platformTexture;
    if (!platformTexture.loadFromFile("images/platform.png")) {
        cout << "Error loading platform.png" << endl;
        return -1;
    }

    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/SpaceBackground.png")) {
        cout << "Error loading background2.jpg" << endl;
        return -1;
    }

    Texture pauseIconTexture, playIconTexture;
    if (!pauseIconTexture.loadFromFile("images/pause.png") || !playIconTexture.loadFromFile("images/play.png")) {
        cout << "Error loading icon textures" << endl;
        return -1;
    }

    // Create sprite objects for pause and play icons
    Sprite pauseIcon, playIcon;
    pauseIcon.setTexture(pauseIconTexture);
    playIcon.setTexture(playIconTexture);

    // Set initial positions for the icons
    pauseIcon.setPosition(600, 23); // Example position (top-left corner)
    playIcon.setPosition(600, 23);  // Example position (top-left corner)

    // Set the pause icon visible and play icon invisible initially
    bool isPauseVisible = true;
    bool isPlayVisible = false;

    // Set up sprites
    Sprite doodle;
    doodle.setTexture(doodleTextureLeft);

    Sprite background;
    background.setTexture(backgroundTexture);

    // Scaling Background to fit window size
    Vector2u windowSize = window.getSize();
    Vector2u textureSize = backgroundTexture.getSize();
    background.setScale(
        float(windowSize.x) / textureSize.x,
        float(windowSize.y) / textureSize.y
    );

    // Set up platforms
    vector<Platform> platforms(9);
    float platformSpacing = 60.0f;
    for (int i = 0; i < platforms.size(); i++) {
        platforms[i].x = rand() % (window.getSize().x - platformTexture.getSize().x);
        platforms[i].y = i * platformSpacing;
    }

    // Ensure the first platform is at a specific position
    platforms[0].x = (window.getSize().x - platformTexture.getSize().x) / 2;
    platforms[0].y = window.getSize().y - 100;
    // Set up sounds
    SoundBuffer jumpBuffer;
    if (!jumpBuffer.loadFromFile("sounds/jump.wav")) {
        cout << "Error loading jump.wav" << endl;
        return -1;
    }
    Sound jumpSound;
    jumpSound.setBuffer(jumpBuffer);

    SoundBuffer loseBuffer;
    if (!loseBuffer.loadFromFile("sounds/lose.wav")) {
        cout << "Error loading lose.wav" << endl;
        return -1;
    }
    Sound loseSound;
    loseSound.setBuffer(loseBuffer);

    SoundBuffer EmptyBoostBuffer;
    if (!EmptyBoostBuffer.loadFromFile("sounds/EmptyBoost.wav")) {
        cout << "Error loading lose.wav" << endl;
        return -1;
    }
    Sound EmptyBoostSound;
    EmptyBoostSound.setBuffer(EmptyBoostBuffer);



    SoundBuffer boostBuffer;
    if (!boostBuffer.loadFromFile("sounds/boost.wav")) {
        cout << "Error loading boost.wav" << endl;
        return -1;
    }
    Sound boostSound;
    boostSound.setBuffer(boostBuffer);

    // Set up game variables
    float doodleX = 400;
    float doodleY = 300;
    float moveSpeed = 5.0f;
    float gravity = 0.5f;
    float jumpSpeed = 0;
    int score = 0;
    bool lost = false;
    short boostCount = 5;
    bool spacePressed = false;

    // Set up fireball
    Sprite fireball;
    fireball.setTexture(fireballTextures[0]);
    fireball.setPosition(400, 0);
    int frame = 0;
    Clock fireballClock;

    // Set up text
    Font font;
    if (!font.loadFromFile("Fonts/2157-font.otf")) {
        cout << "Error loading font" << endl;
        return -1;
    }
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(32);
    scoreText.setFillColor(Color::White);

    Text BoostText;
    BoostText.setFont(font);
    BoostText.setCharacterSize(32);
    BoostText.setFillColor(Color::White);
    BoostText.setPosition(110, 30);

    Clock gameClock;
    bool fireballVisible = false;
    Clock pauseClock; // Clock to track the time when the user remains at the bottom
    bool gamePaused = false; // Flag to indicate if the game is paused

    // Main game loop
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Space && boostCount > 0) {
                    spacePressed = true;
                }
                if (event.key.code == Keyboard::Space && boostCount == 0) {
                    EmptyBoostSound.play();
                }
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::P) {
                // Toggle the game pause state
                gamePaused = !gamePaused;

                // Toggle the visibility of pause and play icons
                isPauseVisible = !gamePaused;
                isPlayVisible = gamePaused;
            }

        }

        if (!gamePaused)
        {
            if (Keyboard::isKeyPressed(Keyboard::Left)) {
                doodleX -= moveSpeed;
                doodle.setTexture(doodleTextureLeft);
            }
            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                doodleX += moveSpeed;
                doodle.setTexture(doodleTextureRight);
            }

            if (spacePressed) {
                jumpSpeed = -15; // Apply boost
                boostCount--; // Decrease the boost count
                spacePressed = false; // Reset the flag
                boostSound.play();
            }

            doodleY += jumpSpeed;
            jumpSpeed += gravity;

            // Ensuring that doodle donot get out of the window
            if (doodleX < 0) doodleX = 0;
            if (doodleX > window.getSize().x - doodle.getGlobalBounds().width)
                doodleX = window.getSize().x - doodle.getGlobalBounds().width;
            if (doodleY >= window.getSize().y - doodle.getGlobalBounds().height) {
                doodleY = window.getSize().y - doodle.getGlobalBounds().height;
            }

            // To Add an effect that doodle is moving Upward
            if (doodleY < 200)
            {
                float offset = 200.f - doodleY;
                doodleY = 200.f;

                for (auto& platform : platforms)
                {
                    platform.y += offset;

                    if (platform.y > window.getSize().y) {
                        platform.x = rand() % (window.getSize().x - platformTexture.getSize().x);
                        platform.y = 0;
                    }
                }
                score += static_cast<int>(offset);
            }

            // Checking collision of doodle's bottom with platform Top 
            FloatRect doodleBound = doodle.getGlobalBounds();
            for (auto& platform : platforms) {
                Sprite plat(platformTexture);
                plat.setPosition(platform.x, platform.y);
                FloatRect platformBounds = plat.getGlobalBounds();

                if (doodleBound.intersects(platformBounds)) {
                    float doodleBottom = doodleBound.top + doodleBound.height;
                    float platformTop = platformBounds.top;
                    if (doodleBottom >= platformTop && doodleBottom <= platformTop + 15.f && jumpSpeed > 0) {
                        jumpSpeed = -15;
                        jumpSound.play();
                    }
                }
            }

            if (!fireballVisible)
            {// Calculate the maximum x position for the fireball
                float maxFireballX = window.getSize().x - fireball.getGlobalBounds().width;

                // Set the fireball position randomly within the window's width
                fireball.setPosition(rand() % static_cast<int>(maxFireballX) + 20, 0);

                fireballVisible = true;
                gameClock.restart();
            }

            // Update fireball animation


            if (fireballVisible)
            {
                fireball.move(0, 3); // Adjust speed as needed

                // Reset fireball position when it reaches the bottom
                if (fireball.getPosition().y > window.getSize().y) {
                    fireballVisible = false;
                }
            }

            if (boostCount == 0)
            {
                BoostText.setFillColor(Color::Red);
            }

            // Check if the doodle is at the bottom
            if (doodleY >= window.getSize().y - doodle.getGlobalBounds().height)
            {
                loseSound.play();
                gamePaused = true;

                if (H1.drawScoreBoard(score) == -1)
                {
                    return 0;
                }
                else
                {

                    runGame();
                }
            }

            // Check collision with fireball using pixel-perfect collision
            if (pixelPerfectCollision(doodle, fireball)) {
                cout << "Collision with fireball!" << endl;
                loseSound.play();
                gamePaused = true;
                if (H1.drawScoreBoard(score) == -1)
                {
                    return 0;
                }
                else
                {
                    runGame();
                }
            }
            // Update score text
            scoreText.setString("Score: " + to_string(score));
            FloatRect textRect = scoreText.getLocalBounds();
            scoreText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            scoreText.setPosition(window.getSize().x / 2.0f, 50);

            // Update doodle position
            doodle.setPosition(doodleX, doodleY);
            BoostText.setString("Boosts: " + to_string(boostCount));
        }

        Time fireballElapsed = fireballClock.restart();
        frame += fireballElapsed.asMilliseconds() / 16; // Adjust animation speed
        if (frame >= fireballTextures.size())
            frame = 0;

        fireball.setTexture(fireballTextures[frame]);


        window.clear();
        window.draw(background);

        // Draw platforms
        for (int i = 0; i < 9; i++)
        {
            Sprite plat;
            plat.setTexture(platformTexture);
            plat.setPosition(platforms[i].x, platforms[i].y);
            window.draw(plat);
        }

        if (isPauseVisible) {
            window.draw(pauseIcon);
        }
        else if (isPlayVisible) {
            window.draw(playIcon);
        }

        // Draw doodle, fireball, and score text
        window.draw(doodle);
        window.draw(fireball);
        window.draw(scoreText);
        window.draw(BoostText);

        window.display();

    }

    return 0;
}

int main()
{
    HomePage H2;
    if (H2.drawHomePage() == -1)
    {
        return 0;
    }
    runGame();
}