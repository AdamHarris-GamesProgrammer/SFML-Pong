#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <memory>
#include <iostream>
#include <random>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 720

class GameObject {
public:
	GameObject(sf::RenderWindow* window, sf::Vector2f position, sf::Color colorr, sf::Vector2f size) : window(window), position(position), color(color), size(size) {

	}

	virtual void Draw() = 0;

	virtual void PollInput(float deltaTime, sf::Event* event) {}

	virtual void Update(float deltaTime, sf::Event* event) {
		transformRect.left = position.x;
		transformRect.top = position.y;
	}

	sf::Vector2f GetPosition() { return position; }
	sf::Vector2f GetSize() { return size; }
	sf::Rect<float> GetRect() { return transformRect; }

protected:
	sf::Vector2f position;
	sf::Vector2f size;

	sf::Color color;

	sf::Rect<float> transformRect = sf::Rect<float>(position, size);

	sf::RenderWindow* window;
};

class Paddle : public GameObject {
public:
	Paddle() = default;
	Paddle(sf::RenderWindow* window, float xPos, sf::Color color, sf::Vector2f size, float movementSpeed = 3.0f) : GameObject(window, sf::Vector2f(xPos, SCREEN_HEIGHT / 2), color, size), movementSpeed(movementSpeed)
	{
		paddleSprite.setSize(size);
		paddleSprite.setPosition(sf::Vector2f(xPos, SCREEN_HEIGHT / 2));
		paddleSprite.setFillColor(color);
	}
	~Paddle() {

	}

	void SetKeys(sf::Keyboard::Key up, sf::Keyboard::Key down) {
		upKey = up;
		downKey = down;
	}

	void Draw() override {
		window->draw(paddleSprite);
	}

	void PollInput(float deltaTime, sf::Event* event) override {
		if (sf::Keyboard::isKeyPressed(upKey)) {
			if(position.y > 0)
				position.y = position.y - movementSpeed * deltaTime;
		}
		else if (sf::Keyboard::isKeyPressed(downKey)) {
			if(position.y < SCREEN_HEIGHT - size.y)
				position.y = position.y + movementSpeed * deltaTime;
		}
	}

	void Update(float deltaTime, sf::Event* event) override {
		GameObject::Update(deltaTime, event);

		PollInput(deltaTime, event);
		paddleSprite.setPosition(position);

		
	}

private:
	sf::RectangleShape paddleSprite;

	sf::Keyboard::Key upKey = sf::Keyboard::Up;
	sf::Keyboard::Key downKey = sf::Keyboard::Down;

	float movementSpeed = 3.0f;
};

class Ball : public GameObject {
public:
	Ball(sf::RenderWindow* window, sf::Color color, float radius) : GameObject(window, sf::Vector2f(SCREEN_WIDTH/2,SCREEN_HEIGHT/2), color, sf::Vector2f(radius * 2, radius * 2)) {
		ballSprite = sf::CircleShape(radius);
		Reset();

		if (!scoreSoundBuffer.loadFromFile("Assets/Sounds/paddleHit.Wav")) {
			std::cout << "Failed to load file" << std::endl;
		}
		scoreSound.setBuffer(scoreSoundBuffer);

		if (!wallSoundBuffer.loadFromFile("Assets/Sounds/wallHit.wav")) {
			std::cout << "Failed to load file" << std::endl;
		}
		wallSound.setBuffer(wallSoundBuffer);
	}

	void Draw() override {
		window->draw(ballSprite);
	}

	void Update(float deltaTime, sf::Event* event) override {
		if (position.x < 0) {
			Reset();
		}
		else if (position.x > SCREEN_WIDTH) {
			Reset();
		}

		if (position.y < 0) {
			HitWall();
		}
		else if (position.y + size.y > SCREEN_HEIGHT) {
			HitWall();
		}

		GameObject::Update(deltaTime, event);

		position += velocity * deltaTime;

		ballSprite.setPosition(position);
	}

	void Reset() {
		position = sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

		//generates random seed for the program
		std::random_device rd;  
		std::mt19937 gen(rd()); 
		std::uniform_int_distribution<> dis(0, 1);
		std::uniform_real_distribution<> yDis(0, movementSpeed);

		float xVel = movementSpeed * dis(gen);
		float yVel = movementSpeed * dis(gen);

		if(dis(gen) == 0)
		{
			xVel = -movementSpeed;
		}
		else
		{
			xVel = movementSpeed;
		}

		if (dis(gen) == 0)
		{
			
			yVel = -yDis(gen);

		}
		else
		{
			yVel = yDis(gen);
		}

		velocity = sf::Vector2f(xVel, yVel);
	}

	void HitPaddle() {
		//TODO: Play sound
		scoreSound.play();

		if (position.x < SCREEN_WIDTH / 2) {
			position.x = 0 + 36.0f;
		}
		else {
			position.x = SCREEN_WIDTH - 37.0f - size.x;
		}

		velocity.x = -velocity.x;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 1);
		std::uniform_real_distribution<> yDis(0, movementSpeed);

		float yVel = movementSpeed * dis(gen);

		if (dis(gen) == 0)
		{
			yVel = -yDis(gen);
		}
		else
		{
			yVel = yDis(gen);
		}

		velocity.y = yVel;

	}

	void HitWall() {
		velocity.y = -velocity.y;
		wallSound.play();
	}

private:
	sf::Vector2f velocity;

	sf::CircleShape ballSprite;

	float movementSpeed = 250.0f;

	sf::SoundBuffer scoreSoundBuffer;
	sf::SoundBuffer wallSoundBuffer;
	sf::Sound scoreSound;
	sf::Sound wallSound;
};

bool CheckCollision(GameObject& a, GameObject& b) {
	if (a.GetRect().intersects(b.GetRect())) {
		return true;
	}
	return false;
}

int main()
{
	auto* window = new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "SFML Pong");
	window->setVerticalSyncEnabled(true);
	window->setFramerateLimit(60);

	std::shared_ptr<Paddle> leftPaddle = std::make_shared<Paddle>(window, 15, sf::Color::Red, sf::Vector2f(20,100), 120.0f);
	leftPaddle->SetKeys(sf::Keyboard::W, sf::Keyboard::S);

	std::shared_ptr<Paddle> rightPaddle = std::make_shared<Paddle>(window, SCREEN_WIDTH - 35, sf::Color::Green, sf::Vector2f(20,100), 120.0f);

	std::shared_ptr<Ball> ball = std::make_shared<Ball>(window, sf::Color::White, 16.0f);

	sf::Clock deltaClock;

	while (window->isOpen())
	{
		sf::Time dt = deltaClock.restart();
		float deltaTime = dt.asSeconds();

		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
				ball->Reset();
			}
		}

		leftPaddle->Update(deltaTime, &event);
		rightPaddle->Update(deltaTime, &event);
		ball->Update(deltaTime, &event);

		//Checks collision with the paddle on the side that the ball is currently on
		if (ball->GetPosition().x < SCREEN_WIDTH / 2) {
			if (CheckCollision(*ball, *leftPaddle)) {
				ball->HitPaddle();
			}
		}
		else
		{
			if (CheckCollision(*ball, *rightPaddle)) {
				ball->HitPaddle();
			}
		}

		window->clear();

		leftPaddle->Draw();
		rightPaddle->Draw();
		ball->Draw();

		window->display();
	}
	
	delete window;

	return 0;
}
