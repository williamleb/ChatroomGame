#pragma once

#include "gameObject.h"

namespace TP3Client
{	
	/// <summary>
	/// La bulle de parole est l'endroit où est placé le texte d'un joueur qui parle.
	/// </summary>
	/// <seealso cref="GameObject" />
	class BulleParole : public GameObject
	{
		const static int MS_LIFETIME = 15000;

		sf::Text texte;

		sf::RectangleShape shape;

		sf::Time innerTimer;

		bool activated;

	public:		

		BulleParole(const sf::Font& font);

		void update(const float deltaT);

		bool isActivated() const;

		void show(const std::string& text);

		const std::string& getText() const;

		void draw(sf::RenderWindow& window);
	};
}