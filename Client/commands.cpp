#include "commands.h"
#include "other.h"

using namespace TP3Client;

/// <summary>
/// Crée une nouvelle <see cref="PlayerCommand_Base"/>.
/// </summary>
/// <param name="whenHappened">Le temps où la commande a été faite.</param>
OtherCommand_Base::OtherCommand_Base(const sf::Time& whenHappened)
	:whenHappened(whenHappened)
{
}

/// <summary>
/// Crée une nouvelle <see cref="PlayerCommand_ChangeDirection"/>.
/// </summary>
/// <param name="direction">La direction de la commande.</param>
/// <param name="whenHappened">Le temps où la commande a été faite.</param>
OtherCommand_ChangeDirection::OtherCommand_ChangeDirection(const sf::Vector2f& direction, const sf::Time& whenHappened)
	:direction(direction), OtherCommand_Base(whenHappened)
{
}

/// <summary>
/// Exécute la commande: change la direction du joueur.
/// </summary>
/// <param name="player">Le joueur avec lequel changer la direction.</param>
void OtherCommand_ChangeDirection::execute(Other* player) const
{
	player->changeDirection(direction);
}

/// <summary>
/// Retourne la direction de la commande.
/// </summary>
/// <returns>La direction de la commande.</returns>
const sf::Vector2f OtherCommand_ChangeDirection::getDirection() const
{
	return direction;
}

/// <summary>
/// Crée une nouvelle <see cref="PlayerCommand_Fire"/>.
/// </summary>
/// <param name="position">La position de la commande.</param>
/// <param name="whenHappened">Le temps où la commande a été faite.</param>
OtherCommand_ChangePosition::OtherCommand_ChangePosition(const sf::Vector2f& position, const sf::Time& whenHappened)
	:OtherCommand_Base(whenHappened), position(position)
{
}

/// <summary>
/// Exécute la commande: change la position du joueur.
/// </summary>
/// <param name="player">Le joueur avec lequel changer la position.</param>
void OtherCommand_ChangePosition::execute(Other* player) const
{
	player->changePosition(position);
}

/// <summary>
/// Retourne la position de la commande.
/// </summary>
/// <returns>La position de la commande.</returns>
const sf::Vector2f OtherCommand_ChangePosition::getPosition() const
{
	return position;
}