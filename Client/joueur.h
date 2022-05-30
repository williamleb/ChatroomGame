#pragma once

#include "square.h"

namespace TP3Client
{	
	/// <summary>
	/// Le joueur est un joueur qui est controllé par l'utilisateur.
	/// </summary>
	/// <seealso cref="Square" />
	class Joueur : public Square
	{

	public:		
		Joueur(const sf::Font& font);
		void getInputs();

	private:
		Movable::Direction getDirectionFromInput(int moveMap);
	};
}