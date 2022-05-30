#include "movable.h"
#include <cassert>

using namespace TP3Client;

const float Movable::diagonal = 0.707106781f;
const float Movable::directionVerificationThreshold = 0.5f;

/// <summary>
/// Crée un nouveau <see cref="Movable"/>.
/// </summary>
/// <param name="speed">La vitesse de l'objet.</param>
Movable::Movable()
	: direction(0, 0), GameObject()
{
}

/// <summary>
/// Détruit un <see cref="Movable"/>.
/// </summary>
Movable::~Movable()
{
}

/// <summary>
/// Update l'objet selon le temps écoulé.
/// </summary>
/// <param name="deltaT">Le nombre de secondes depuis le dernier appel de la méthode.</param>
void Movable::update(const float deltaT)
{
	setPosition(getPosition() + (direction * speed * deltaT));
}

/// <summary>
/// Retourne la direction de l'objet.
/// </summary>
/// <returns>La direction de l'objet.</returns>
const sf::Vector2f& Movable::getDirection() const
{
	return direction;
}

/// <summary>
/// Retourne la vitesse de l'objet.
/// </summary>
/// <returns>La vitesse de l'objet.</returns>
float Movable::getSpeed() const
{
	return speed;
}

/// <summary>
/// Change la direction de l'objet.
/// </summary>
/// <param name="direction">La direction de l'objet.</param>
void Movable::setDirection(const Direction direction)
{
	this->direction = convertDirection(direction);
}

/// <summary>
/// Change la direction de l'objet.
/// </summary>
/// <param name="angle">L'angle de l'objet.</param>
void Movable::setDirection(const float angle)
{
	direction.x = cos(angle);
	direction.y = sin(angle);
}

/// <summary>
/// Change la direction de l'objet.
/// </summary>
/// <param name="direction">La direction de l'objet.</param>
void Movable::setDirection(sf::Vector2f direction)
{
	// Il faut que l'hypothénuse de la direction soit égale à 1 (car c'est la propriété speed qui influence la vitesse)
	// ou à 0 (si l'objet est immobile).
	assert(abs(1 - (direction.x * direction.x + direction.y * direction.y)) <= directionVerificationThreshold
				|| (direction.x == 0 && direction.y == 0));
	this->direction = direction;
}

/// <summary>
/// Change la vitesse de l'objet.
/// </summary>
/// <param name="speed">a vitesse de l'objet.</param>
void Movable::setSpeed(const float speed)
{
	this->speed = speed;
}

/// <summary>
/// Convertit une direction provenant de l'enum en vecteur.
/// </summary>
/// <param name="direction">La direction provenant de l'enum.</param>
/// <returns>La représentation en vecteur de la direction.</returns>
sf::Vector2f Movable::convertDirection(Direction direction)
{
	sf::Vector2f convertedDirection(0, 0);

	switch (direction)
	{
	case Movable::IDLE:
		convertedDirection.x = 0;
		convertedDirection.y = 0;
		break;
	case Movable::UP:
		convertedDirection.x = 0;
		convertedDirection.y = -1;
		break;
	case Movable::UP_LEFT:
		convertedDirection.x = -diagonal;
		convertedDirection.y = -diagonal;
		break;
	case Movable::UP_RIGHT:
		convertedDirection.x = diagonal;
		convertedDirection.y = -diagonal;
		break;
	case Movable::LEFT:
		convertedDirection.x = -1;
		convertedDirection.y = 0;
		break;
	case Movable::DOWN:
		convertedDirection.x = 0;
		convertedDirection.y = 1;
		break;
	case Movable::DOWN_LEFT:
		convertedDirection.x = -diagonal;
		convertedDirection.y = diagonal;
		break;
	case Movable::DOWN_RIGHT:
		convertedDirection.x = diagonal;
		convertedDirection.y = diagonal;
		break;
	case Movable::RIGHT:
		convertedDirection.x = 1;
		convertedDirection.y = 0;
		break;
	}

	return convertedDirection;
}