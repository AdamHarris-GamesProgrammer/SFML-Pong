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

	void SetKeys(sf::Keyboard::Key up, sf::Keyboard::Key down) {
		upKey = up;
		downKey = down;
	}

	void Draw() {
		mWindow->draw(paddleSprite);
	}

	void PollInput(float deltaTime, sf::Event* event) {
		if (sf::Keyboard::isKeyPressed(upKey)) {
			position.y--;
		}
		else if (sf::Keyboard::isKeyPressed(downKey)) {
			position.y++;
		}
	}

	void Update(float deltaTime, sf::Event* event) {
		PollInput(deltaTime, event);
		paddleSprite.setPosition(position);
	}

private:
	sf::RectangleShape paddleSprite;
	sf::Vector2f position;
	sf::Color color;

	sf::RenderWindow* mWindow;

	sf::Keyboard::Key upKey;
	sf::Keyboard::Key downKey;
};


int main()
{
	auto* window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML Pong");
	window->setVerticalSyncEnabled(true);
	window->setFramerateLimit(60);

	Paddle leftPaddle(window, 15, sf::Color::Red, sf::Vector2f(20,100));
	leftPaddle.SetKeys(sf::Keyboard::W, sf::Keyboard::S);

	Paddle rightPaddle(window, SCREEN_WIDTH - 35, sf::Color::Green, sf::Vector2f(20,100));
	rightPaddle.SetKeys(sf::Keyboard::Up, sf::Keyboard::Down);

	sf::Clock deltaClock;

	while (window->isOpen())
	{
		sf::Time dt = deltaClock.restart();

		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();

		}
		
		leftPaddle.Update(dt.asSeconds(), &event);
		rightPaddle.Update(dt.asSeconds(), &event);

		window->clear();

		leftPaddle.Draw();
		rightPaddle.Draw();


		window->display();
	}

	return 0;
}
