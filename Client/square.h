#pragma once

#include "movable.h"
#include "bulleParole.h"

namespace TP3Client
{	

	/// <summary>
	/// Représente une entité (un joueur) dans le jeu.
	/// Il peut se déplacer ou lancer une bulle de tchat.
	/// </summary>
	/// <seealso cref="Movable" />
	class Square : public Movable
	{
		static const int SQUARE_WIDTH = 100;
		static const int SQUARE_HEIGHT = 100;

		static const int SQUARE_DEFAULT_SPEED = 150;

		sf::RectangleShape shape;

		sf::Text name;

		BulleParole bulle;
	protected:		
		Square(const sf::Font& font);

	public:
		virtual ~Square();

		void draw(sf::RenderWindow& window);
		void update(const float deltaT);

		const std::string getName() const;
		void setName(const std::string& name);

		void parler(const std::string& text);
	private:
		void adjustIntoMap();
	};
}