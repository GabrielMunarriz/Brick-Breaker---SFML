// We include SFML's graphics library, which gives us
// access to functions and classes that allow us to render 
// things on the screen.
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <algorithm>
using namespace std;
using namespace sf;

int window_W; // Window width
int window_H; // Window height
int fps; // Maximum frames per second

template <class T1, class T2>
bool isPadBalIntersecting(T1& mA, T2& mB)
{
    return (mA.getPosition().x + (mA.getSize().x / 2.f)) >= (mB.getPosition().x - (mB.getRadius())) && (mA.getPosition().x - (mA.getSize().x / 2.f)) <= (mB.getPosition().x + (mB.getRadius())) &&
           (mA.getPosition().y + (mA.getSize().y / 2.f)) >= (mB.getPosition().y - (mB.getRadius())) && (mA.getPosition().y - (mA.getSize().y / 2.f)) <= (mB.getPosition().y + (mB.getRadius()));
}

int main()
{

	// --- Input from settings.txt file ---
	string input;

	fstream settingsFile;
	settingsFile.open("settings.txt");

	if (!settingsFile) {

		cout << "No such file";
		exit(EXIT_FAILURE);

	}

	// Input format: ScreenWidth ScreenHeight FPS 
	while (settingsFile.good()) {

		settingsFile >> input;
		window_W = stoi(input);

		settingsFile >> input;
		window_H = stoi(input);

		settingsFile >> input;
		fps = stoi(input);

	}

	settingsFile.close();

	// --- Window Settings ---

	// Create an SFML window of size 800x600 with "Hello SFML" as the title
	RenderWindow window(VideoMode(window_W, window_H), "Munarriz, De Los Reyes - Final Output");

	// We tell SFML to limit our application to whatever FPS' value is
	window.setFramerateLimit(fps);

	// Instead of an explicit "isPlaying" bool variable,
	// we check if the window is still open or not to signify
	// whether we are still playing or not.
	// We use the SFML window's isOpen() function, which returns a bool value,
	// to check whether the window is open or not.

	// --- Initializing Ball ---
	int ballSize = 15.0f;
	float ballForce = 10.0f;
	float ballMass = 1.0f;
	float ballElasticity = 1.0f;
	float BallVelocity{8.f};
	Vector2f Ball_currentVelocity(0.0f, 0.0f);
	Vector2f BallAcceleration(0.0f, 0.0f);
	Vector2f BallPosition(window_W / 2, window_H / 2);

	CircleShape Ball(ballSize);
	Ball.setFillColor(Color::Blue);
	Ball.setPosition(BallPosition.x, BallPosition.y);
	
	Vector2f PaddlePosition(window_W / 2, window_H - 50);
	float PaddleVelocity{6.f};
	Vector2f Paddle_currentVelocity(0, 0);
	Vector2f PaddleSize(100.0f, 20.0f);
	
	RectangleShape Paddle(PaddleSize);
	Paddle.setPosition(PaddlePosition);
	Paddle.setFillColor(Color::Blue);
	Paddle.setOrigin(PaddleSize.x / 2.f, PaddleSize.y / 2.f);
	

	// -- Initializing Timestep Stuff

	float accumulator = 0.0f;
	float timestep = 1.0f / fps;

	// --- Initializing Clock --- 
	Clock clock;
	Time elapsedTime = seconds(0);
	Int32 elapsedTimeAsSeconds;
	Int32 elapsedTimeAsMilliseconds;
	float deltaTime;

	// --- Initializing Flags ---
	bool ball_upFlag = false;
	bool ball_downFlag = false;
	bool ball_leftFlag = false;
	bool ball_rightFlag = false;
	
	bool paddle_leftFlag = false;
	bool paddle_rightFlag = false;
	
	bool spaceFlag = false;

	// --- Application loop ---
	while (window.isOpen())
	{
		// --- Clear screen ---

		// Clear the screen using the SFML window's clear() function.
		// By default, SFML clears the screen by replacing all the colors 
		// in the background with black.
		window.clear();

		// Alternatively, we can provide a color to clear the screen with.
		// If you replace the previous line with the one below, 
		// you will see a red background instead of black.
		// window.clear(sf::Color::Red);


		// --- Process input/events ---

		// We create a variable to store information about 
		// events that happen to our window, e.g., window closed, key pressed, etc.
		Event event;

		// We now check if there were events triggered by the window since the last
		// iteration of the loop, and react to them accordingly.
		// As mentioned in the slides, we can also directly update the current state 
		// at this point, e.g., change player position when a keyboard input event happened
		while (window.pollEvent(event))
		{
			// We check if the event was from the user requesting to close the window.
			// If it is, we tell SFML window to close the window.
			if (event.type == Event::Closed)
			{
				window.close();
			}

			// On W, S, A, or D key press
			if (event.type == Event::KeyPressed)
			{
				switch (event.key.code)
				{
					// Process the W (up), S (down), A (left), and D (right) keys
				case sf::Keyboard::W:     ball_upFlag = true; break;
				case sf::Keyboard::S:    ball_downFlag = true; break;
				case sf::Keyboard::A:    ball_leftFlag = true; break;
				case sf::Keyboard::D:   ball_rightFlag = true; break;
				case sf::Keyboard::Space:   spaceFlag = true; break;
				case sf::Keyboard::Left:    paddle_leftFlag = true; break;
				case sf::Keyboard::Right:    paddle_rightFlag = true; break;
				default: break;
				}
			}

			// On W, S, A, or D key release
			if (event.type == Event::KeyReleased)
			{
				switch (event.key.code)
				{
					// Process the W (up), S (down), A (left), and D (right) keys
				case sf::Keyboard::W:     ball_upFlag = false; break;
				case sf::Keyboard::S:    ball_downFlag = false; break;
				case sf::Keyboard::A:    ball_leftFlag = false; break;
				case sf::Keyboard::D:   ball_rightFlag = false; break;
				case sf::Keyboard::Space:   spaceFlag = false; break;
				case sf::Keyboard::Left:    paddle_leftFlag = false; break;
				case sf::Keyboard::Right:    paddle_rightFlag = false; break;				
				default: break;
				}
			}
			// Alternatively, you can use a switch statement instead of an if-else-if chain

		}

		// --- Update/advance state ---

		// Anything that involves updating the state can be done here
		// For example, adjusting the position of falling objects due to gravity, etc.

		// deltaTime
		Time iterationTime = clock.restart();
		deltaTime = iterationTime.asSeconds(); // we use this for movement since we can't directly use time objects for calculations.
		elapsedTime += iterationTime;
		elapsedTimeAsSeconds = elapsedTime.asSeconds(); //we use this for printing since we can't directly print Time Objects	

		// DEBUGGING ------------------------------------ 
		if (ball_rightFlag) {
			BallAcceleration.x += ballForce / ballMass;
		}

		if (ball_leftFlag) {
			BallAcceleration.x -= ballForce / ballMass;
		}

		if (ball_upFlag) {
			BallAcceleration.y -= ballForce / ballMass;
		}

		if (ball_downFlag) {
			BallAcceleration.y += ballForce / ballMass;
		}

		// ----------------------------------------------

		// Physics
		accumulator += deltaTime;
		while (accumulator >= timestep) {

			//Ball Physics
			Ball_currentVelocity.x += (BallAcceleration.x * timestep);
			Ball_currentVelocity.y += (BallAcceleration.y * timestep);
			BallPosition.x += (BallAcceleration.x * (0.5f * timestep * timestep)) + (Ball_currentVelocity.x * timestep);
			BallPosition.y += (BallAcceleration.y * (0.5f * timestep * timestep)) + (Ball_currentVelocity.y * timestep);

			// Ball Collision for window boundaries
			if (BallPosition.x + ballSize >= window_W) {
				BallPosition.x = window_W - ballSize;
				BallAcceleration.x = 0;
				Ball_currentVelocity.x = -ballElasticity * (Ball_currentVelocity.x);
			}

			if (BallPosition.x <= 0) {
				BallPosition.x = ballSize;
				BallAcceleration.x = 0;
				Ball_currentVelocity.x = -ballElasticity * (Ball_currentVelocity.x);
			}

			if (BallPosition.y <= 0) {
				BallPosition.y = ballSize;
				BallAcceleration.y = 0;
				Ball_currentVelocity.y = -ballElasticity * (Ball_currentVelocity.y);
			}

			/*if (BallPosition.y + ballSize >= window_H) {
				BallPosition.y = window_H - ballSize;
				BallAcceleration.y = 0;
				Ball_currentVelocity.y = -ballElasticity * (Ball_currentVelocity.y);
			}*/

			if (isPadBalIntersecting(Paddle, Ball))
			{
				Ball_currentVelocity.y = -BallVelocity;
				
				if(Ball.getPosition().x < Paddle.getPosition().x)
				{
					Ball_currentVelocity.x = -BallVelocity;
				}
				
				else
				{
					Ball_currentVelocity.x = BallVelocity;
				}
			}

			// Paddle Collision for Window Boundaries
			if (paddle_leftFlag && (Paddle.getPosition().x - (Paddle.getSize().x)/2.f) >= 0)
			{
				Paddle_currentVelocity.x = -PaddleVelocity;
			}
			
			else if (paddle_rightFlag && (Paddle.getPosition().x + (Paddle.getSize().x)/2.f) <= window_W)
			{
				Paddle_currentVelocity.x = PaddleVelocity;
			}
			
			else
			{
				Paddle_currentVelocity.x = 0;
			}

			accumulator -= timestep;
		}

		if (ball_rightFlag == false && ball_leftFlag == false) {
			BallAcceleration.x = 0.0f;
		}

		if (ball_upFlag == false && ball_downFlag == false) {
			BallAcceleration.y = 0.0f;
		}

		Ball.setPosition(BallPosition.x, BallPosition.y);
		Paddle.move(Paddle_currentVelocity);

		// DEBUGGING ------------------------------------
		
		cout << "BallAcceleration X: " << BallAcceleration.x << endl;
		cout << "BallAcceleration Y: " << BallAcceleration.y << endl;
		cout << "Ball_currentVelocity X: " << Ball_currentVelocity.x << endl;
		cout << "Ball_currentVelocity Y: " << Ball_currentVelocity.y << endl;
		cout << "BallPosition X: " << BallPosition.x << endl;
		cout << "BallPosition Y: " << BallPosition.y << endl;
		
		cout << "PaddlePosition X: " << Paddle.getPosition().x << endl;
		cout << "PaddlePosition Y: " << Paddle.getPosition().y << endl;
		cout << "PaddleSize X: " << Paddle.getSize().x << endl;
		//cout << "PaddleSize Y: " << Paddle.getSize().y << endl;
		
		

		// ----------------------------------------------

		// --- Render current state ---

		// Draw all the objects we need to draw

		window.draw(Ball);
		window.draw(Paddle);

		// Finally, tell SFML to render/display whatever we drew to the screen
		window.display();
	}

}


