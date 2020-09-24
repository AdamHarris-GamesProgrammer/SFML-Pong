#include <SFML/Graphics.hpp>
#include <memory>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 720

void SetPaddlePosition(sf::RectangleShape& paddle, sf::Vector2f pos) {
	paddle.setPosition(pos);
}

class Paddle {
public:
	Paddle() = default;
	Paddle(sf::RenderWindow* window, float xPos, sf::Color color, sf::Vector2f size) : mWindow(window), color(color), position(xPos, SCREEN_HEIGHT / 2)
	{
		paddleSprite.setSize(size);
		paddleSprite.setPosition(position);
		paddleSprite.setFillColor(color);
	}
	~Paddle() {

	}

	void Draw() {
		mWindow->draw(paddleSprite);
	}

	void Update() {

	}

private:
	sf::RectangleShape paddleSprite;
	sf::Vector2f position;
	sf::Color color;

	sf::RenderWindow* mWindow;

};


int main()
{
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32), "SFML Pong");
	
	Paddle leftPaddle(window, 15, sf::Color::Red, sf::Vector2f(20,100));
	Paddle rightPaddle(window, SCREEN_WIDTH - 35, sf::Color::Green, sf::Vector2f(20,100));

	while (window->isOpen())
	{
		sf::Event event;
		while (window->pollEvent(event))
		{


			if (event.type == sf::Event::Closed)
				window->close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				//position.y--;
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				//position.y++;
			}
		}


		window->clear();

		leftPaddle.Draw();
		rightPaddle.Draw();


		window->display();
	}

	return 0;
}
