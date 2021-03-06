#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <memory>
#include <iostream>
#include <random>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 720

class Time {
public:
	static float deltaTime;
	
	static void UpdateTimer() {
		sf::Time dt = deltaClock.restart();
		deltaTime = dt.asSeconds();
	}
private:
	static sf::Clock deltaClock;
};

float Time::deltaTime;
sf::Clock Time::deltaClock;


class SoundEffect {
public:
	SoundEffect() = default;

	SoundEffect(const char* filename) {
		SetPath(filename);
	}

	void SetPath(const char* filename) {
		if (!soundBuffer.loadFromFile(filename)) {
			std::cout << "[ERROR: Sound.cpp]: Sound buffer at " << filename << " could not be found" << std::endl;
			return;
		}
		soundEffect.setBuffer(soundBuffer);
	}
	void Play() {
		soundEffect.play();
	}
	void Pause() {
		soundEffect.pause();
	}
	void Stop() {
		soundEffect.stop();
	}
	void SetLooping(bool isLooped) {
		soundEffect.setLoop(isLooped);
	}

private:
	sf::SoundBuffer soundBuffer;
	sf::Sound soundEffect;
};

class GameObject {
public:
	GameObject(sf::RenderWindow* window, sf::Vector2f position, sf::Color colorr, sf::Vector2f size) : window(window), position(position), color(color), size(size) {

	}

	virtual void Draw() = 0;

	virtual void PollInput(float deltaTime, sf::Event* event) {}

	virtual void Update() {}

	virtual void Update(sf::Event* event) {}

	void UpdateRects() {
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

		scoreFont.loadFromFile("Assets/Fonts/good times.ttf");
		scoreText = sf::Text("0", scoreFont, 30);
		scoreText.setStyle(sf::Text::Bold);
		
		if (xPos < SCREEN_WIDTH / 2) {
			scoreText.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) - 140.0f, 0));
		}
		else
		{
			scoreText.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) + 100.0f, 0));
		}

	}
	~Paddle() {

	}

	void SetKeys(sf::Keyboard::Key up, sf::Keyboard::Key down) {
		upKey = up;
		downKey = down;
	}

	void Draw() override {
		window->draw(paddleSprite);
		window->draw(scoreText);
	}

	void PollInput(float deltaTime, sf::Event* event) override {
		if (sf::Keyboard::isKeyPressed(upKey)) {
			if (position.y > 0)
				position.y = position.y - movementSpeed * deltaTime;
		}
		else if (sf::Keyboard::isKeyPressed(downKey)) {
			if (position.y < SCREEN_HEIGHT - size.y)
				position.y = position.y + movementSpeed * deltaTime;
		}
	}

	void Update(sf::Event* event) override {
		UpdateRects();

		PollInput(Time::deltaTime, event);
		paddleSprite.setPosition(position);
	}

	void IncrementScore() {
		score++;
		scoreText.setString(std::to_string(score));
	}

	void ResetScore() {
		score = 0;
		scoreText.setString(std::to_string(score));
	}

private:
	sf::RectangleShape paddleSprite;

	sf::Keyboard::Key upKey = sf::Keyboard::Up;
	sf::Keyboard::Key downKey = sf::Keyboard::Down;

	float movementSpeed = 3.0f;

	int score = 0;

	sf::Font scoreFont;
	sf::Text scoreText;
};

class Ball : public GameObject {
public:
	Ball(sf::RenderWindow* window, sf::Color color, float radius) : GameObject(window, sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), color, sf::Vector2f(radius * 2, radius * 2)) {
		ballSprite = sf::CircleShape(radius);
		Reset();

		wallSound = std::make_unique<SoundEffect>("Assets/Sounds/wallHit.wav");
		scoreSound = std::make_unique<SoundEffect>("Assets/Sounds/paddleHit.Wav");
	}

	void Draw() override {
		window->draw(ballSprite);
	}

	void Update() override {
		UpdateRects();

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

		position += velocity * Time::deltaTime;

		ballSprite.setPosition(position);
	}

	void Reset() {
		position = sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

		float xVel = rand() % 2 ? xVel = movementSpeed : xVel = -movementSpeed;
		float yVel = rand() % (int)movementSpeed;

		velocity = { xVel, yVel };
	}

	void HitPaddle() {
		scoreSound->Play();

		if (position.x < SCREEN_WIDTH / 2) {
			position.x = 0 + 36.0f;
		}
		else {
			position.x = SCREEN_WIDTH - 37.0f - size.x;
		}

		velocity = { -velocity.x, (float)(-rand() % (int)movementSpeed) };
	}

	void HitWall() {
		velocity.y = -velocity.y;
		wallSound->Play();
	}

private:
	sf::Vector2f velocity;

	sf::CircleShape ballSprite;

	float movementSpeed = 250.0f;

	std::unique_ptr<SoundEffect> scoreSound;
	std::unique_ptr<SoundEffect> wallSound;
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

	std::shared_ptr<Paddle> leftPaddle = std::make_shared<Paddle>(window, 15, sf::Color::Red, sf::Vector2f(20, 100), 120.0f);
	leftPaddle->SetKeys(sf::Keyboard::W, sf::Keyboard::S);

	std::shared_ptr<Paddle> rightPaddle = std::make_shared<Paddle>(window, SCREEN_WIDTH - 35, sf::Color::Green, sf::Vector2f(20, 100), 120.0f);

	std::shared_ptr<Ball> ball = std::make_shared<Ball>(window, sf::Color::White, 16.0f);

	while (window->isOpen())
	{
		Time::UpdateTimer();

		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
				ball->Reset();
			}
		}

		leftPaddle->Update(&event);
		rightPaddle->Update(&event);
		ball->Update();

		//Checks collision with the paddle on the side that the ball is currently on
		if (ball->GetPosition().x < SCREEN_WIDTH / 2) {
			if (CheckCollision(*ball, *leftPaddle)) {
				ball->HitPaddle();
				leftPaddle->IncrementScore();
			}
		}
		else
		{
			if (CheckCollision(*ball, *rightPaddle)) {
				ball->HitPaddle();
				rightPaddle->IncrementScore();
			}
		}

		if (ball->GetPosition().x < 0) {
			leftPaddle->ResetScore();
		}
		else if (ball->GetPosition().x > SCREEN_WIDTH) {
			rightPaddle->ResetScore();
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
