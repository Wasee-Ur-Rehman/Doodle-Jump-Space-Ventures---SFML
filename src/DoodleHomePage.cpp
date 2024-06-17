#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<SFML/Audio.hpp>
using namespace std;
using namespace sf;

class HomePage
{
private:
	Font font;
	Text start;
	Text exit;
	Music click;
	Texture texture;
	Sprite front;
public:
	HomePage()
	{
		
		start.setString("Play Game");
		start.setCharacterSize(55);
		start.setPosition(340,425);
		
		exit.setString("Quit");
		exit.setCharacterSize(55);
		exit.setPosition(395, 580);
		click.openFromFile("click.wav");
		texture.loadFromFile("images/HomePage1.png");
		front.setTexture(texture);

	}
	int drawHomePage()
	{
		// make a new window
		RenderWindow window(VideoMode(880, 880), "DoodleJump!");
		if (!font.loadFromFile("Fonts/RobotoCondensed-Light.ttf")) {
			std::cout << "Error loading font" << std::endl;
			return -1;
		}
		start.setFont(font);
		exit.setFont(font);
		start.setFillColor(Color::Black);
		exit.setFillColor(Color::Black);

		while (window.isOpen())
		{
			// check all the window's events that were triggered since the last iteration of the loop
			Event event;
			while (window.pollEvent(event))
			{
				// "close requested" event: we close the window
				if (event.type == Event::Closed)
					window.close();

				// Hover Effects
				if (event.type == sf::Event::MouseMoved)
				{
					if (start.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
					{
						start.setFillColor(sf::Color::Blue);
					}
					else
					{
						start.setFillColor(sf::Color::Black);
					}
					if (exit.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
					{
						exit.setFillColor(sf::Color::Red);
					}
					else
					{
						exit.setFillColor(sf::Color::Black);
					}
				}
				// Click Events
				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						if (start.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
						{

							cout << "Play  button clicked" << endl;
							click.play();
							sleep(seconds(1));
							// play as guest
							window.close();
						}
						else if (exit.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
						{
							window.close();
							click.play();
							sleep(seconds(1));
							return -1;
						}
					}
				}

			}

			window.clear();
			window.draw(front);
			window.draw(start);
			window.draw(exit);
			window.display();
		}

	}

	int drawScoreBoard(int score)
	{
		// Load textures and fonts
		Texture replayTexture, logoutTexture;
		replayTexture.loadFromFile("images/replay.png");
		logoutTexture.loadFromFile("images/logout.png");

		Texture background;
		background.loadFromFile("images/a.jpg");

		Sprite ScoreBack;
		ScoreBack.setTexture(background);
		font.loadFromFile("fonts/Roboto-Light.ttf");

		// Create sprites for the icons
		Sprite replaySprite(replayTexture);
		Sprite logoutSprite(logoutTexture);

		// Set sprite positions
		replaySprite.setPosition(200, 150);
		logoutSprite.setPosition(200, 250);

		// Create text objects
		Text scoreText, replayText, exitText;

		// Score Text
		scoreText.setFont(font);
		scoreText.setString("Score: " + to_string(score));
		scoreText.setCharacterSize(65);
		scoreText.setPosition(160, 40);
		scoreText.setFillColor(Color::Black);

		// Replay Text
		replayText.setFont(font);
		replayText.setString("Replay");
		replayText.setCharacterSize(34);
		replayText.setPosition(270, 160);
		replayText.setFillColor(Color::Black);

		// Exit Text
		exitText.setFont(font);
		exitText.setString("Exit");
		exitText.setCharacterSize(34);
		exitText.setPosition(275, 260);
		exitText.setFillColor(Color::Black);

		// Create a window
		RenderWindow window(VideoMode(600, 450), "Scoreboard");

		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();

				// Hover Effects
				if (event.type == sf::Event::MouseMoved)
				{
					if (replaySprite.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
					{
						replayText.setFillColor(Color::Magenta);
					}
					else
					{
						replayText.setFillColor(Color::Black);
					}
					if (logoutSprite.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
					{
						exitText.setFillColor(Color::Red);
					}
					else
					{
						exitText.setFillColor(Color::Black);
					}
				}

				// Click Events
				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						if (replaySprite.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
						{
							cout << "Replay button clicked" << endl;
							// Handle replay action
							click.play();
							sleep(seconds(1));
							return 1;
							window.close(); // Close the window for replay (or reset game state)
						}
						else if (logoutSprite.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
						{
							cout << "Exit button clicked" << endl;
							// Handle exit action
							click.play();
							sleep(seconds(1));

							window.close();
							return -1;
						}
					}
				}
			}

			window.clear();

			window.draw(ScoreBack);
			window.draw(scoreText);
			window.draw(replaySprite);
			window.draw(replayText);
			window.draw(logoutSprite);
			window.draw(exitText);
			window.display();
		}
	}
};
