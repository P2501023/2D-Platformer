#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class PlatformClass {
public:
	Rect<float> bounds;

	PlatformClass(float xpos, float ypos, float xsize, float ysize) {
		bounds = Rect<float>(xpos, ypos, xsize, ysize);
	}
};

class Coin {
public:
	Rect<float> bounds;
	float collected;
	CircleShape shape;

	float time = 0;

	Coin(float xpos, float ypos) {
		bounds = Rect<float>(xpos, ypos, 30, 30);
		collected = false;
		shape = CircleShape(15);
		shape.setFillColor(Color(255, 255, 0));
		shape.setPosition(xpos, ypos);
		shape.setOutlineColor(Color::Black);
	}

	void drawToScreen(RenderWindow& window) {
		window.draw(shape);
	}
};

class NPC {
public:
	float xorig, yorig;
	float magnitude;

	CircleShape shape;
	Rect<float> bounds;

	float time = 0;

	NPC(float xpos, float ypos, float mag) {
		xorig = xpos;
		yorig = ypos;
		magnitude = mag;
		bounds = Rect<float>(xpos, ypos, 40, 40);
		shape = CircleShape(20);
		shape.setFillColor(Color(255, 0, 0));
		shape.setOutlineColor(Color::Black);
	}

	void drawToScreen(RenderWindow &window, float deltaTime) {
		time += deltaTime;
		shape.setPosition(xorig + magnitude * sinf(time), yorig);
		bounds.left = xorig + magnitude * sinf(time);
		window.draw(shape);
	}
};

class MovingPlatform {
public:
	float xorig, yorig;

	Rect<float> bounds;
	Texture tex;
	Sprite shape;

	float magnitude;
	float time = 1;


	MovingPlatform(float xpos, float ypos, float xsize, float ysize, float mag) : tex() {
		xorig = xpos;
		yorig = ypos;
		bounds = Rect<float>(xpos, ypos, xsize, ysize);
		tex.loadFromFile("src/mp.png");
		shape = Sprite(tex);
		shape.setPosition(xpos, ypos);

		magnitude = mag;
	}

	void drawToScreen(RenderWindow& window, float deltaTime) {
		time += deltaTime;
		bounds.top = yorig + magnitude * sinf(time);
		shape.setPosition(xorig, yorig + magnitude * sinf(time));
		window.draw(shape);
	}
};


//--------------- Game Constants --------------------------

const int windowWidth = 1024;
const int windowHeight = 688;
const int halfWinHeight = windowHeight / 2;
const int halfWinWidth = windowWidth / 2;

//--------------- Game objects --------------------------

PlatformClass platforms[3]{ // The platform array
	PlatformClass(48, 288, 96, 384),
	PlatformClass(384, 544, 336, 128),
	PlatformClass(800, 496, 208, 176)
};

int pnum = 3; // number of platforms


CircleShape exitShape(20);
Rect<float> exitBounds(950, 450, 40, 100);

Coin coin(500, 450);
NPC npc(534, 500, 150);

MovingPlatform mp(224, 384, 112, 16, 130);

//----------------------- Classes ----------------------------

class PlayerClass {
public:
	bool playerFaceRight;
	bool onGround;
	float yvel, xvel, xpos, ypos;
	Sprite image;

	Rect<float> bounds;

	PlayerClass(Sprite sprite) {
		image = sprite;
		image.setOrigin(32, 32);

		onGround = false;
		init();
		bounds = Rect<float>(xpos - 32, ypos - 16, 32, 64);
	}

	void init() {
		xpos = 80;
		ypos = 0;
		yvel = 0;
		xvel = 0;
	}

	void updateBounds() {
		bounds.left = xpos - 16;
		bounds.top = ypos - 32;
	}

	bool update(bool up, bool left, bool right) {
		updateBounds();
		onGround = check(0, 2);
		yvel += 0.5f;
		if (right) {
			image.setScale(1, 1);
			xvel = 5;
			if (check(5, -2)) {
				xvel = 0;
			}
		}
		if (left) {
			image.setScale(-1, 1);
			xvel = -5;
			if (check(-5, -2)) {
				xvel = 0;
			}
		}
		if (!(right || left)) {
			xvel = 0;
		}

		ypos += yvel;
		if (yvel > 0) {
			while (collide()) {
				ypos--;
				updateBounds();
			}
		}
		else if (yvel < 0) {
			while (collide()) {
				ypos++;
				updateBounds();
			}
		}
		if (onGround == true) {
			yvel = 0;
			if (up) {
				yvel = -10;
			}
		}

		xpos += xvel;
		if (xvel > 0) {
			while (collide()) {
				xvel = 0;
				xpos--;
				updateBounds();
			}
		}
		else if (xvel < 0) {
			while (collide()) {
				xvel = 0;
				xpos++;
				updateBounds();
			}
		}
		image.setPosition(xpos, ypos);

		if (bounds.intersects(coin.bounds)) coin.collected = true; // magic !

		if (bounds.intersects(exitBounds)) {
			return true;
		}
		return false;
	}
	bool collide() {
		for (int i = 0; i < pnum; i++) {
			if (bounds.intersects(platforms[i].bounds)) return true;
		}
		if (bounds.intersects(mp.bounds)) return true;
		return false;
	}
	bool check(float x, float y) {
		bounds.left += x;
		bounds.top += y;
		if (collide()) {
			bounds.left -= x;
			bounds.top -= y;
			return true;
		}
		bounds.left -= x;
		bounds.top -= y;
		return false;
	}

};

// ------------------------------- Game Loop ----------------------------

int main() { //<! Entry point for the application

	RenderWindow window(VideoMode(windowWidth, windowHeight), "2D Platformer");
	window.setFramerateLimit(60);

	bool up = false, left = false, right = false; // To keep track of the pressed keys
	// game state variables
	bool end = false; //end : if we are displaying an end message
	bool dead = false; // if the player is dead

	bool menu = true; // If we are on menu state 

	Clock clock; // to keep track of the time

	// ---------------------------------------------- Text objects -------------------------

	Font conolasFont;
	conolasFont.loadFromFile("src/comic.ttf");

	Text winText("        You won ! \n Press any key to restart ...", conolasFont, 50);
	winText.setPosition(halfWinWidth - winText.getGlobalBounds().width / 2, halfWinHeight - winText.getGlobalBounds().height / 2);
	winText.setFillColor(Color(0, 0, 0));

	Text endText("      You ded \n Press any key to restart ...", conolasFont, 50);
	endText.setPosition(halfWinWidth - endText.getGlobalBounds().width / 2, halfWinHeight - endText.getGlobalBounds().height / 2);
	endText.setFillColor(Color(0, 0, 0));

	Text menuText("Menu", conolasFont, 50);
	Text playText("Play", conolasFont, 40);
	Text quitText("Quit", conolasFont, 40);

	menuText.setFillColor(Color(0, 0, 200));
	playText.setFillColor(Color(0, 0, 200));
	quitText.setFillColor(Color(0, 0, 200));

	menuText.setPosition(halfWinWidth - menuText.getGlobalBounds().width / 2, 150); // set the text positions at the center of the screen
	playText.setPosition(halfWinWidth - playText.getGlobalBounds().width / 2, 300);
	quitText.setPosition(halfWinWidth - quitText.getGlobalBounds().width / 2, 450);

	//-------------- Game objects settings ------------------

	Texture playerTexture1;
	playerTexture1.loadFromFile("src/snail.png");
	Sprite playerSprite1(playerTexture1);

	PlayerClass playerObject(playerSprite1);


	exitShape.setPosition(exitBounds.left, exitBounds.top);
	exitShape.setFillColor(Color(0, 0, 0));
	exitShape.setOutlineColor(Color(0, 150, 255));
	exitShape.setOutlineThickness(5);

	Texture levelt, backgroundt;
	levelt.loadFromFile("src/level.png");
	backgroundt.loadFromFile("src/bg.png");

	Sprite level(levelt);
	Sprite background(backgroundt);

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) { // Event management
			if (event.type == Event::Closed) {
				window.close();
			}
			if (event.type == Event::KeyPressed && end) {
				end = false;
				playerObject.init();
				coin.collected = false;
			}
			if (event.type == Event::MouseButtonPressed) {
				Vector2f mouse = Vector2f(float(Mouse::getPosition(window).x), float(Mouse::getPosition(window).y)); // Mouse coordinates
				if (playText.getGlobalBounds().contains(mouse)) menu = false; // test for collisions between the mouse and the texts bounding boxes
				else if (quitText.getGlobalBounds().contains(mouse)) {
					cout << "Goodbye !" << endl;
					exit(0);
				}

			}
		}

		float deltaTime = clock.getElapsedTime().asSeconds();
		clock.restart();

		right = Keyboard::isKeyPressed(Keyboard::Right);
		left = Keyboard::isKeyPressed(Keyboard::Left);
		up = Keyboard::isKeyPressed(Keyboard::Up);

		if (!end) {
			dead = false;
			end = playerObject.update(up && !menu, left && !menu, right && !menu); // to disable the player movement if in menu state
			if (playerObject.ypos > windowHeight) {
				end = true;
				dead = true;
			}
			if (playerObject.bounds.intersects(npc.bounds)) {
				end = true;
				dead = true;
			}
		}

		window.clear();

		window.draw(background);
		window.draw(level);

		window.draw(playerObject.image);

		window.draw(exitShape);

		if (!coin.collected) coin.drawToScreen(window);

		npc.drawToScreen(window, end ? 0 : deltaTime); // ternary operator to move the objects only if we are not in end state
		mp.drawToScreen(window, end ? 0 : deltaTime);

		if (end) { // drawing the correct messages at the end;
			if (dead)
				window.draw(endText);
			else
				window.draw(winText);
		}

		//Making the buttons react to mouse hovering

		Vector2f mouse = Vector2f(float(Mouse::getPosition(window).x), float(Mouse::getPosition(window).y)); // Mouse coordinates
		float mscale = menuText.getGlobalBounds().contains(mouse) ? 1.4f : 1.0f;
		float pscale = playText.getGlobalBounds().contains(mouse) ? 1.4f : 1.0f;
		float qscale = quitText.getGlobalBounds().contains(mouse) ? 1.4f : 1.0f;

		menuText.setScale(mscale, mscale);
		playText.setScale(pscale, pscale);
		quitText.setScale(qscale, qscale);

		if (menu) {// drawing menu text
			window.draw(menuText);
			window.draw(playText);
			window.draw(quitText);
		}

		window.display();
	}
}
