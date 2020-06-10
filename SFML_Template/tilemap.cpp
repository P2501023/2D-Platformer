#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

int winWidth = 700;
int winHeight = 1000;

int main() {
	//int x;
	//int y;
	int w = 50;
	int h = 50;

	RenderWindow window(VideoMode(winWidth, winHeight), "Sprite Map");

	Texture texture;
	texture.loadFromFile("src/spritesheet.png");
	Sprite spriteSheet(texture);

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
		}
	}

}
