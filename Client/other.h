#pragma once

#include "square.h"
#include "commandBuffer.h"
#include "commands.h"

namespace TP3Client
{	
	/// <summary>
	/// Représente un joueur qui n'est pas l'utilisateur du programme.
	/// </summary>
	/// <seealso cref="Square" />
	class Other : public Square
	{		
		/// <summary>
		/// Propriété dépréciée (ne pas utiliser).
		/// </summary>
		CommandBuffer commands;
		
		/// <summary>
		/// Nombre de millisecondes sans recevoir de message pour être considéré comme déconnecté.
		/// </summary>
		static const int MS_BEFORE_DECONNEXION = 10'000;

		sf::Time innerTimer;
		sf::Time timeStarted;

	public:				
		Other(const sf::Font& font);
		~Other();

		void update(const float deltaT);

		void changeDirection(const sf::Vector2f& direction);
		void changePosition(const sf::Vector2f& position);

	private:
		// Méthode dépréciée (ne pas utiliser)
		void addCommand(OtherCommand_Base* command);

	};
}