#pragma once

#include <SFML/Graphics.hpp>

namespace TP3Client
{
	/// <summary>
	/// Représente le jeu.
	/// </summary>
	class Game
	{
	public:		
		Game();
		int run();

	private:
		const int WINDOW_WIDTH = 1280;
		const int WINDOW_HEIGHT = 700;

		sf::RenderWindow mainWin;
		sf::Event event;
	};
}