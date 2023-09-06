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
Font font; // In-game text font

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
	
	if (!font.loadFromFile("StarmapFont.TTF"))
	{
		cout << "No such font file";
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

	// --- Initializing Vectors
	vector<RectangleShape> AllRects;
	vector<ConvexShape> PaddleWingVisuals;

	// --- Initializing Ball ---
	int ballSize = 15.0f;
	float ballForce = 500.0f;
	float ballMass = 1.0f;
	float ballElasticity = 1.0f;
	int randomDirectionChooser;
	Vector2f BallVelocity(0.0f, 0.0f);
	Vector2f BallAcceleration(0.0f, 0.0f);
	Vector2f BallPosition(window_W / 2, (window_H / 4) * 3);
	Vector2f StartingBallPosition(window_W / 2, (window_H / 4) * 3);

	CircleShape Ball(ballSize);
	Ball.setFillColor(Color::Blue);
	Ball.setPosition(BallPosition.x, BallPosition.y);

	// --- Initializing Paddle ---
	Vector2f PaddlePosition(window_W / 2, window_H - 50.0f);
	float PaddleVelocity = 6.0f;
	//Vector2f Paddle_currentVelocity(0, 0);
	Vector2f PaddleSize(100.0f, 20.0f);

	Vector2f PaddleWingSize(20.0f, 20.0f);
	Vector2f PaddleLeftWingPosition((PaddlePosition.x - PaddleWingSize.x), PaddlePosition.y);
	Vector2f PaddleRightWingPosition((PaddlePosition.x + PaddleSize.x), PaddlePosition.y);

	RectangleShape Paddle(PaddleSize);
	Paddle.setPosition(PaddlePosition);
	Paddle.setFillColor(Color::Cyan);
	//Paddle.setOrigin(PaddleSize.x / 2.f, PaddleSize.y / 2.f);

	RectangleShape PaddleLeftWing(PaddleWingSize);
	PaddleLeftWing.setPosition(PaddleLeftWingPosition);
	PaddleLeftWing.setFillColor(Color::Transparent);

	RectangleShape PaddleRightWing(PaddleWingSize);
	PaddleRightWing.setPosition(PaddleRightWingPosition);
	PaddleRightWing.setFillColor(Color::Transparent);

	ConvexShape PaddleLeftWingVisual;
	PaddleLeftWingVisual.setPointCount(3);
	PaddleLeftWingVisual.setPoint(0, Vector2f(0,0));
	PaddleLeftWingVisual.setPoint(1, Vector2f(0, 0 + PaddleWingSize.y));
	PaddleLeftWingVisual.setPoint(2, Vector2f(0 - PaddleWingSize.x, 0 + PaddleWingSize.y));
	PaddleLeftWingVisual.setPosition(Vector2f(PaddleLeftWingPosition.x + PaddleWingSize.x, PaddleLeftWingPosition.y));
	PaddleLeftWingVisual.setFillColor(Color::Cyan);

	ConvexShape PaddleRightWingVisual;
	PaddleRightWingVisual.setPointCount(3);
	PaddleRightWingVisual.setPoint(0, Vector2f(0,0));
	PaddleRightWingVisual.setPoint(1, Vector2f(0, 0 + PaddleWingSize.y));
	PaddleRightWingVisual.setPoint(2, Vector2f(0 + PaddleWingSize.x, 0 + PaddleWingSize.y));
	PaddleRightWingVisual.setPosition(PaddleRightWingPosition);
	PaddleRightWingVisual.setFillColor(Color::Cyan);

	AllRects.push_back(Paddle); //Add Paddle to AllRects. Paddle is now AllRects[0]
	AllRects.push_back(PaddleLeftWing);
	AllRects.push_back(PaddleRightWing);
	PaddleWingVisuals.push_back(PaddleLeftWingVisual);
	PaddleWingVisuals.push_back(PaddleRightWingVisual);
	
	// --- Initializing Bricks ---
	int brickWidth = 100;
	int brickHeight = 50;
	int numberOfBricksX = 6;
	int numberOfBricksY = 3;
	int gapBetweenBricks = 20;
	Vector2f brickInitialPos((window_W - ((brickWidth * numberOfBricksX) + ((gapBetweenBricks * numberOfBricksX) - 1))) / 2, 50); //Making it so that the bricks are centered (at the X axis)

	for (int i = 0; i <= numberOfBricksY - 1; i++) {

		for (int j = 0; j <= numberOfBricksX - 1; j++) {

			AllRects.push_back(RectangleShape(Vector2f(brickWidth, brickHeight)));
			AllRects.back().setPosition(brickInitialPos.x + (brickWidth * j) + (gapBetweenBricks * j) , brickInitialPos.y + (brickHeight * i) + (gapBetweenBricks * i));
			
			if (i == 0)
			{
				AllRects.back().setFillColor(Color::Green);
			}
			
			else if (i == 1)
			{
				AllRects.back().setFillColor(Color::Yellow);
			}			
			
			else
			{
				AllRects.back().setFillColor(Color::Red);
			}

			// DEBUGING ------------------------------------ 
			/*AllRects.back().setOutlineColor(Color::Red);
			AllRects.back().setOutlineThickness(4);*/
			// ---------------------------------------------

		}

	}

	// --- Initializing Brick-Ball Collision Stuff

	float overlapLeft;
	float overlapRight;
	float overlapTop;
	float overlapBottom;
	bool ballFromLeft;
	bool ballFromTop;
	float minOverlapX;
	float minOverlapY;

	// --- Initializing Timestep Stuff ---

	float accumulator = 0.0f;
	float timestep = 1.0f / fps;

	// --- Initializing Clock --- 
	Clock clock;
	Time elapsedTime = seconds(0);
	Int32 elapsedTimeAsSeconds;
	Int32 elapsedTimeAsMilliseconds;
	float deltaTime;

	// --- Initializing Flags ---
	bool w_Flag = false;
	bool s_Flag = false;
	bool a_Flag = false;
	bool d_Flag = false;

	bool leftArrow_Flag = false;
	bool rightArrow_Flag = false;

	bool spaceFlag = false;
	bool spacePressed = false;
	
	bool winFlag = false;
	
	// --- Initializing Font Texts and Game Conditions ---
	Text lives_text;
	Text gameover_text;
	Text youwin_text;
	int lives_count = 3;
	
	lives_text.setFont(font);
	gameover_text.setFont(font);
	youwin_text.setFont(font);
	
	lives_text.setString("Lives: 3");
	gameover_text.setString("GAME OVER");
	youwin_text.setString("YOU WIN!");

	lives_text.setCharacterSize(24);
	gameover_text.setCharacterSize(100);
	youwin_text.setCharacterSize(100);
	
	lives_text.setFillColor(Color::White);
	gameover_text.setFillColor(Color::White);
	youwin_text.setFillColor(Color::White);

	lives_text.setPosition(window_W / 10, window_H - 100);
	gameover_text.setPosition(window_W / 6, window_H / 2);
	youwin_text.setPosition(window_W / 6, window_H / 2);

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
				case sf::Keyboard::W:     w_Flag = true; break;
				case sf::Keyboard::S:    s_Flag = true; break;
				case sf::Keyboard::A:    a_Flag = true; break;
				case sf::Keyboard::D:   d_Flag = true; break;
				case sf::Keyboard::Space:   spaceFlag = true; break;
				case sf::Keyboard::Left:    leftArrow_Flag = true; break;
				case sf::Keyboard::Right:    rightArrow_Flag = true; break;
				default: break;
				}
			}

			// On W, S, A, or D key release
			if (event.type == Event::KeyReleased)
			{
				switch (event.key.code)
				{
					// Process the W (up), S (down), A (left), and D (right) keys
				case sf::Keyboard::W:     w_Flag = false; break;
				case sf::Keyboard::S:    s_Flag = false; break;
				case sf::Keyboard::A:    a_Flag = false; break;
				case sf::Keyboard::D:   d_Flag = false; break;
				case sf::Keyboard::Space:   spaceFlag = false; break;
				case sf::Keyboard::Left:    leftArrow_Flag = false; break;
				case sf::Keyboard::Right:    rightArrow_Flag = false; break;
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

		/*if (d_Flag) {
			BallAcceleration.x += ballForce / ballMass;
		}

		if (a_Flag) {
			BallAcceleration.x -= ballForce / ballMass;
		}

		if (w_Flag) {
			BallAcceleration.y -= ballForce / ballMass;
		}

		if (s_Flag) {
			BallAcceleration.y += ballForce / ballMass;
		}*/

		// ----------------------------------------------

		// Ball Controls

		if (spaceFlag && !spacePressed) { //launch ball at a random direction

			BallVelocity.y = -250;

			randomDirectionChooser = (rand() % 2) + 1; //chooses between one or two

			cout << randomDirectionChooser << endl;

			if (randomDirectionChooser == 1) {
				BallVelocity.x = 250;
			} else {
				BallVelocity.x = -250;
			}

			spacePressed = true;

		}

		// Paddle Controls

		if (leftArrow_Flag || a_Flag) {

			PaddlePosition.x -= PaddleVelocity;
			PaddleLeftWingPosition.x -= PaddleVelocity;
			PaddleRightWingPosition.x -= PaddleVelocity;

		}

		if (rightArrow_Flag || d_Flag) {

			PaddlePosition.x += PaddleVelocity;
			PaddleLeftWingPosition.x += PaddleVelocity;
			PaddleRightWingPosition.x += PaddleVelocity;

		}

		// Physics
		accumulator += deltaTime;
		while (accumulator >= timestep) {

			//Ball Physics
			BallVelocity.x += (BallAcceleration.x * timestep);
			BallVelocity.y += (BallAcceleration.y * timestep);
			BallPosition.x += (BallAcceleration.x * (0.5f * timestep * timestep)) + (BallVelocity.x * timestep);
			BallPosition.y += (BallAcceleration.y * (0.5f * timestep * timestep)) + (BallVelocity.y * timestep);

			//Collision for window boundaries
			if (BallPosition.x + ballSize > window_W || BallPosition.x < 0) {
				BallAcceleration.x = 0;
				BallVelocity.x = -ballElasticity * (BallVelocity.x);
			}

			if (BallPosition.y < 0) {
				BallAcceleration.y = 0;
				BallVelocity.y = -ballElasticity * (BallVelocity.y);
			}

			if (AllRects[2].getPosition().x + PaddleWingSize.x > window_W) {
				PaddlePosition.x = window_W - PaddleSize.x - PaddleWingSize.x;
				PaddleLeftWingPosition.x = window_W - PaddleSize.x - (2 * PaddleWingSize.x);
				PaddleRightWingPosition.x = window_W - PaddleWingSize.x;
			}

			if (AllRects[1].getPosition().x < 0) {
				PaddlePosition.x = 0 + PaddleWingSize.x;
				PaddleLeftWingPosition.x = 0;
				PaddleRightWingPosition.x = 0 + PaddleSize.x + PaddleWingSize.x;
			}
			
			// DEBUGGING ------------------------------------
			// cout << "LIVES COUNT" << endl;
			// cout << lives_count << endl;
			// ----------------------------------------------
			
			if (BallPosition.y + ballSize > window_H)
			{
				BallVelocity = Vector2f(0.0f, 0.0f);
				
				if(lives_count == 3)
				{
					BallPosition.x = StartingBallPosition.x;
					BallPosition.y = StartingBallPosition.y;					
					lives_text.setString("Lives: 2");
					spacePressed = false;
					lives_count = lives_count - 1;
				}
					
				else if(lives_count == 2)
				{
					BallPosition.x = StartingBallPosition.x;
					BallPosition.y = StartingBallPosition.y;					
					lives_text.setString("Lives: 1");
					spacePressed = false;
					lives_count = lives_count - 1;
				}
					
				else if(lives_count == 1)
				{
					lives_text.setString("Lives: 0");
					lives_count = lives_count - 1;
				}
			}

			for (int i = 0; i <= AllRects.size() - 1; i++) {

				if (i == 0) {

					if (BallPosition.x <= AllRects[i].getPosition().x + PaddleSize.x &&
						AllRects[i].getPosition().x <= BallPosition.x + ballSize &&
						BallPosition.y <= AllRects[i].getPosition().y + PaddleSize.y &&
						AllRects[i].getPosition().y <= BallPosition.y + ballSize
						) {

						// DEBUGGING ------------------------------------
						cout << "TOUCHING" << endl;
						// ----------------------------------------------

						BallPosition.y = AllRects[i].getPosition().y - ballSize - 10.0; // the 10.0 is an allowance so that once it "bounces," it does not hit the paddle again.
						BallAcceleration.y = 0;
						BallVelocity.y = -ballElasticity * (BallVelocity.y);
						BallAcceleration.x = 0;
						BallVelocity.x = ballElasticity * (BallVelocity.x);
						
						// added two if else statements, cause otherwise the ball keeps bouncing back to the same point it initially hit the first brick
						
						// not sure if using BallVelocity.x to change BallVelocity.x's direction is a good idea? In the other code, it used a separate BallVelocity float to change BallVelocity.x's value to make the ball consistent and controllable by the paddle (or my math is wrong in the following code)
						/*if(Ball.getPosition().x < AllRects[0].getPosition().x)
						{
							BallVelocity.x = -ballElasticity * (-BallVelocity.x);
							cout << "GOING LEFT (I think)" << endl;
						}
						
						else if(Ball.getPosition().x > AllRects[0].getPosition().x)
						{
							BallVelocity.x = -ballElasticity * (BallVelocity.x);
							cout << "GOING RIGHT (I think)" << endl;
						}*/

					}

				} else if (i == 1) {

					if (BallPosition.x <= AllRects[i].getPosition().x + PaddleSize.x &&
						AllRects[i].getPosition().x <= BallPosition.x + ballSize &&
						BallPosition.y <= AllRects[i].getPosition().y + PaddleSize.y &&
						AllRects[i].getPosition().y <= BallPosition.y + ballSize
						) {

						// DEBUGGING ------------------------------------
						cout << "TOUCHING" << endl;
						// ----------------------------------------------

						BallPosition.y = AllRects[i].getPosition().y - ballSize;
						BallAcceleration.y = 0;
						BallVelocity.y = -ballElasticity * (BallVelocity.y);

						if (BallVelocity.x >= 0) {
							BallAcceleration.x = 0;
							BallVelocity.x = -ballElasticity * (BallVelocity.x);
						} else {
							BallAcceleration.x = 0;
							BallVelocity.x = ballElasticity * (BallVelocity.x);
						}

					}

				} else if (i == 2) {

					if (BallPosition.x <= AllRects[i].getPosition().x + PaddleSize.x &&
						AllRects[i].getPosition().x <= BallPosition.x + ballSize &&
						BallPosition.y <= AllRects[i].getPosition().y + PaddleSize.y &&
						AllRects[i].getPosition().y <= BallPosition.y + ballSize
						) {

						// DEBUGGING ------------------------------------
						cout << "TOUCHING" << endl;
						// ----------------------------------------------

						BallPosition.y = AllRects[i].getPosition().y - ballSize;
						BallAcceleration.y = 0;
						BallVelocity.y = -ballElasticity * (BallVelocity.y);

						if (BallVelocity.x >= 0) {
							BallAcceleration.x = 0;
							BallVelocity.x = ballElasticity * (BallVelocity.x);
						}
						else {
							BallAcceleration.x = 0;
							BallVelocity.x = -ballElasticity * (BallVelocity.x);
						}

					}

				} else {

					if (BallPosition.x <= AllRects[i].getPosition().x + brickWidth &&
						AllRects[i].getPosition().x <= BallPosition.x + ballSize &&
						BallPosition.y <= AllRects[i].getPosition().y + brickHeight &&
						AllRects[i].getPosition().y <= BallPosition.y + ballSize
						) {

						// DEBUGGING ------------------------------------
						cout << "TOUCHING" << endl;
						// ----------------------------------------------

						overlapLeft = (BallPosition.x + ballSize) - AllRects[i].getPosition().x;
						overlapRight = (AllRects[i].getPosition().x + brickWidth) - BallPosition.x;
						overlapTop = (BallPosition.y + ballSize) - AllRects[i].getPosition().y;
						overlapBottom = (AllRects[i].getPosition().y + brickHeight) - BallPosition.y;

						ballFromLeft = abs(overlapLeft) < abs(overlapRight);
						ballFromTop = abs(overlapTop) < abs(overlapBottom);

						if (ballFromLeft) {
							minOverlapX = overlapLeft;
						} else {
							minOverlapX = overlapRight;
						}

						if (ballFromTop) {
							minOverlapY = overlapTop;
						}
						else {
							minOverlapY = overlapBottom;
						}

						if (abs(minOverlapX) < abs(minOverlapY)) {

							BallAcceleration.x = 0;
							BallVelocity.x = -ballElasticity * (BallVelocity.x);

						} else {

							BallAcceleration.y = 0;
							BallVelocity.y = -ballElasticity * (BallVelocity.y);

						}
						
						if (AllRects[i].getFillColor() == Color::Green)
						{
							AllRects[i].setFillColor(Color::Yellow);
						}
						
						else if (AllRects[i].getFillColor() == Color::Yellow)
						{
							AllRects[i].setFillColor(Color::Red);
						}						
						
						else
						{
							AllRects.erase(AllRects.begin()+i);
							cout << "DELETING..." << endl;	
						}

					}

				}

			}
			
			if(AllRects.size() < 4)
			{
				BallVelocity.x = 0;
				BallVelocity.y = 0;
				winFlag = true;
			}

			accumulator -= timestep;

		}


		// DEBUGGING ------------------------------------
		/*if (d_Flag == false && a_Flag == false) {
			BallAcceleration.x = 0.0f;
		}

		if (w_Flag == false && s_Flag == false) {
			BallAcceleration.y = 0.0f;
		}*/
		// ----------------------------------------------

		if ((rightArrow_Flag == false || d_Flag == false) && (leftArrow_Flag == false || a_Flag == false)) {
			PaddlePosition.x += 0.0f;
		}

		Ball.setPosition(BallPosition);

		//Move paddle
		AllRects[0].setPosition(PaddlePosition); 
		AllRects[1].setPosition(PaddleLeftWingPosition);
		AllRects[2].setPosition(PaddleRightWingPosition);
		PaddleWingVisuals[0].setPosition(Vector2f(PaddleLeftWingPosition.x + PaddleWingSize.x, PaddleLeftWingPosition.y));
		PaddleWingVisuals[1].setPosition(PaddleRightWingPosition);


		// DEBUGGING ------------------------------------

		/*cout << "BallAcceleration X: " << BallAcceleration.x << endl;
		cout << "BallAcceleration Y: " << BallAcceleration.y << endl;
		cout << "BallVelocity X: " << BallVelocity.x << endl;
		cout << "BallVelocity Y: " << BallVelocity.y << endl;
		cout << "BallPosition X: " << BallPosition.x << endl;
		cout << "BallPosition Y: " << BallPosition.y << endl;*/

		/*cout << "PaddlePosition X: " << Paddle.getPosition().x << endl;
		cout << "PaddlePosition Y: " << Paddle.getPosition().y << endl;
		cout << "PaddleSize X: " << Paddle.getSize().x << endl;
		cout << "PaddleSize Y: " << Paddle.getSize().y << endl;*/

		// ----------------------------------------------

		// --- Render current state ---

		// Draw all the objects we need to draw

		window.draw(Ball);

		for (int i = 0; i <= AllRects.size() - 1; i++){
			window.draw(AllRects[i]);
		}

		for (int i = 0; i <= PaddleWingVisuals.size() - 1; i++) {
			window.draw(PaddleWingVisuals[i]);
		}
		
		window.draw(lives_text);
		
		if (lives_count < 1)
		{
			window.draw(gameover_text);			
		}
		
		if (winFlag == true)
		{
			window.draw(youwin_text);
		}


		// Finally, tell SFML to render/display whatever we drew to the screen
		window.display();
	}

}
