#include "joueur.h"
#include "sceneJeu.h"
#include "gestionnaireDeCommunication.h"

using namespace TP3Client;

/// <summary>
/// Crée un nouveau <see cref="Joueur"/>.
/// </summary>
/// <param name="font">La police utilisée pour afficher le nom du joueur.</param>
Joueur::Joueur(const sf::Font& font)
	:Square(font)
{
}

/// <summary>
/// Gère les inputs du joueur.
/// </summary>
void Joueur::getInputs()
{
	int moveMap = 0;

	// Déplacements...
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveMap |= 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveMap |= 2;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveMap |= 4;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveMap |= 8;

	// Détermine la nouvelle direction.
	sf::Vector2f newDirection = Movable::convertDirection(getDirectionFromInput(moveMap));

	// Si la nouvelle direction est différente de l'ancienne...
	if (newDirection != getDirection())
	{
		// On change la direction.
		setDirection(newDirection);

		// Et on l'envoit au serveur.
		GestionnaireDeCommunication::getInstance()->sendDirection(newDirection);
	}
}

/// <summary>
/// Retourne la direction associée à l'input du joueur.
/// </summary>
/// <param name="moveMap">L'input du joueur.</param>
/// <returns>La direction associée à l'input.</returns>
Movable::Direction Joueur::getDirectionFromInput(int moveMap)
{
	switch (moveMap)
	{
		// Droite
	case 1:
		return Movable::RIGHT;
		break;
		// Gauche
	case 2:
		return Movable::LEFT;
		break;
		// Droite + Gauche
	case 3:
		return Movable::IDLE;
		break;
		// Haut
	case 4:
		return Movable::UP;
		break;
		// Haut + Droite
	case 5:
		return Movable::UP_RIGHT;
		break;
		// Haut + Gauche
	case 6:
		return Movable::UP_LEFT;
		break;
		// Haut + Droite + Gauche
	case 7:
		return Movable::UP;
		break;
		// Bas
	case 8:
		return Movable::DOWN;
		break;
		// Bas + Droite
	case 9:
		return Movable::DOWN_RIGHT;
		break;
		// Bas + Gauche
	case 10:
		return Movable::DOWN_LEFT;
		break;
		// Bas + Droite + Gauche
	case 11:
		return Movable::DOWN;
		break;
		// Bas + Haut
	case 12:
		return Movable::IDLE;
		break;
		// Bas + Haut + Droite
	case 13:
		return Movable::RIGHT;
		break;
		// Bas + Haut + Gauche
	case 14:
		return Movable::LEFT;
		break;
		// Bas + Haut + Droite + Gauche
	case 15:
		return Movable::IDLE;
		break;
		// Aucune commande de mouvement.
	default:
		return Movable::IDLE;
		break;
	}
}