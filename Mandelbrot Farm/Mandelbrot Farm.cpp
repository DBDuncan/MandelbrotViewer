


#include <iostream>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <complex>
#include <fstream>
#include <iostream>
#include <condition_variable>
#include <SFML/Graphics.hpp>

#include "Common.h"
#include "Farm.h"
#include "DoLine.h"
#include "DoLineJulia.h"

using namespace std;

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;





uint32_t image[HEIGHT][WIDTH];


std::chrono::steady_clock::time_point t1;
std::chrono::steady_clock::time_point t2;


void imageGen(double left, double right, double top, double bottom, int colour, int set, Farm &f, double yOffset, double xOffset, double zoom, int maxIterations)
{
	//cout << left << " " << right << " " << top << " " << bottom << endl;
	if (set == 0)
	{
		for (int y{ 0 }; y < HEIGHT; ++y)
		{
			f.add_task(new DoLine(&image, left, right, top, bottom, y, colour, yOffset, xOffset, zoom, maxIterations));//-2.0, 1.0, 1.125, -1.125
		}
	}
	else
	{
		for (int y{ 0 }; y < HEIGHT; ++y)
		{
			f.add_task(new DoLineJulia(&image, left, right, top, bottom, y, colour, yOffset, xOffset, zoom, maxIterations));//-2.0, 2.0, 1.125, -1.125
		}
	}
	t1 = std::chrono::steady_clock::now();
	//get workers working
	f.notifyWorkers();
}



int main()
{

	cout << "Enter number of mandelbrot worker threads(enter zero to set number of threads to number of cores in system): ";
	int setNumThreads = 0;
	cin >> setNumThreads;

	cout << "Enter number of max iterations (enter 0 for default of 500): ";
	int maxIterations = 0;
	cin >> maxIterations;

	if (maxIterations <= 0)
	{
		maxIterations = 500;
	}


	//creating the farm object which handles the threads
	Farm f(setNumThreads);
	
	//here, setting window to resolution of screen
	int setWidth = sf::VideoMode::getDesktopMode().width;
	int setHeight = sf::VideoMode::getDesktopMode().height;
	sf::RenderWindow window(sf::VideoMode(setWidth, setHeight), "My window");
	window.setFramerateLimit(60);

	//image data array is loaded into here, which is then loaded into sprite to show
	sf::Texture texture;

	//sprite used to show mandelbrot/julia set
	sf::Sprite sprite;
	sprite.setPosition(150, 0); 

	//white side bar
	sf::RectangleShape bar(sf::Vector2f(150, 1000));
	bar.setFillColor(sf::Color(255, 255, 255));

	//change colour
	sf::RectangleShape colourSel(sf::Vector2f(50, 100));
	colourSel.setPosition(50, 100);
	colourSel.setFillColor(sf::Color(255, 0, 0));

	//gen image button
	sf::RectangleShape genImage(sf::Vector2f(50, 100));
	genImage.setPosition(50, 500);
	genImage.setFillColor(sf::Color(255, 0, 0));

	//change from mandelbrot set to julia set button
	sf::RectangleShape changeSet(sf::Vector2f(50, 100));
	changeSet.setPosition(50, 250);
	changeSet.setFillColor(sf::Color(255, 0, 0));

	//black bar to show progress
	sf::RectangleShape showDone(sf::Vector2f(100, 20));
	showDone.setPosition(25, 700);
	showDone.setFillColor(sf::Color(0, 0, 0));






	int colour = 0;
	int set = 0;

	double mandLeft = -2.0;
	double mandRight = 1.0;
	double mandTop = 1.125;
	double mandBottom = -1.125;

	double juliaLeft = -2.0;
	double juliaRight = 2.0;
	double juliaTop = 1.125;
	double juliaBottom = -1.125;

	bool toSetImage = false;

	double mandXOffset = 0.0;
	double mandYOffset = 0.0;
	double mandZoom = 1.0;

	double juliaXOffset = 0.0;
	double juliaYOffset = 0.0;
	double juliaZoom = 1.0;
	
	int zoomLevel = 0;

	
	texture.create(WIDTH, HEIGHT);
	
	//run the program as long as the window is open
	while (window.isOpen())
	{

		int currentTasks = f.getTasks();
		
		//check that an image is to be set and all tasks are done, which signals the image generation is done
		if (currentTasks == 0 && toSetImage == true)
		{
			
			t2 = std::chrono::steady_clock::now();

			//milliseconds or microseconds
			auto elapsed{ duration_cast<milliseconds>(t2 - t1).count() };
			cout << elapsed << " milliseconds" << std::endl;

			toSetImage = false;
			showDone.setSize(sf::Vector2f(100, 20));
			
			//this line for some reason took some time to come up with
			uint8_t* pixelData = (uint8_t*)image;//&image[0][0] or just image
			texture.update(pixelData);
			sprite.setTexture(texture);

		}



		//event loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			//check if the window exit button has been pressed
			if (event.type == sf::Event::Closed)
			{
				f.exitThreads();
				window.close();
			}

			if (event.type == sf::Event::MouseButtonReleased && toSetImage == false)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//Mouse position relative to the window
					auto mouse_pos = sf::Mouse::getPosition(window); 
					//Mouse position translated into world coordinates
					auto translated_pos = window.mapPixelToCoords(mouse_pos); 
					
					//colourSel detect if clicked
					if (colourSel.getGlobalBounds().contains(translated_pos)) 
					{
						if (colour < 4)
						{
							colour++;
						}
						else
						{
							colour = 0;
						}
						cout << colour << endl;
					}

					//genImage detect if clicked
					if (genImage.getGlobalBounds().contains(translated_pos))
					{
						if (set == 0)
						{
							imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
						}
						else
						{
							imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
						}

						toSetImage = true;
					}

					//change set button
					if (changeSet.getGlobalBounds().contains(translated_pos))
					{
						if (set < 1)
						{
							set++;
						}
						else
						{
							set = 0;
						}
					}
				}
			}

			if (event.type == sf::Event::KeyPressed && toSetImage == false)
			{

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				{
					if (set == 0)
					{
						mandXOffset += 0.2 * mandZoom;
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						juliaXOffset += 0.2 * juliaZoom;
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				{
					if (set == 0)
					{
						mandXOffset -= 0.2 * mandZoom;
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						juliaXOffset -= 0.2 * juliaZoom;
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				{
					if (set == 0)
					{
						mandYOffset -= 0.2 * mandZoom;
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						juliaYOffset -= 0.2 * juliaZoom;
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				{
					if (set == 0)
					{
						mandYOffset += 0.2 * mandZoom;
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						juliaYOffset += 0.2 * juliaZoom;
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
				{
					cout << "pressed z" << endl;
					zoomLevel++;
					cout << "zoom level: " << zoomLevel << endl;

					if (set == 0)
					{
						mandZoom -= (mandZoom / 10);
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						juliaZoom -= (juliaZoom / 10);
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
				{
					cout << "pressed x" << endl;
					zoomLevel--;
					cout << "zoom level: " << zoomLevel << endl;

					if (set == 0)
					{
						mandZoom += (mandZoom / 10);
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						juliaZoom += (juliaZoom / 10);
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
				{
					cout << "pressed c" << endl;
					maxIterations++;
					cout << "iterations level: " << maxIterations << endl;

					if (set == 0)
					{
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
				{
					cout << "pressed v" << endl;
					maxIterations--;
					cout << "iterations level: " << maxIterations << endl;

					if (set == 0)
					{
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
				{
					cout << "pressed b" << endl;
					cout << "Enter number of iterations: ";

					//does cause issues with making windows think the window is frozen, but much quicker then adding one to iterations each time
					cin >> maxIterations;


					cout << "iterations level: " << maxIterations << endl;

					if (set == 0)
					{
						imageGen(mandLeft, mandRight, mandTop, mandBottom, colour, set, f, mandYOffset, mandXOffset, mandZoom, maxIterations);
					}
					else
					{
						imageGen(juliaLeft, juliaRight, juliaTop, juliaBottom, colour, set, f, juliaYOffset, juliaXOffset, juliaZoom, maxIterations);
					}

					toSetImage = true;
				}

			}
		}

		//changing loading bar size
		if (toSetImage == true)
		{
			double size = 100.0;
			size = size * (f.getPercentDone() / 100);

			showDone.setSize(sf::Vector2f(size, 20));

		}
		else
		{
			showDone.setSize(sf::Vector2f(100, 20));
			f.resetPercentDone();
		}










		// clear the window with black color
		window.clear(sf::Color::Black);

		//draw all the graphics
		window.draw(sprite);
		window.draw(bar);
		window.draw(colourSel);
		window.draw(genImage);
		window.draw(changeSet);
		window.draw(showDone);

		//end frame and display everything
		window.display();
	}

}

