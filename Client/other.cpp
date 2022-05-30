#include "other.h"
#include "sceneJeu.h"

using namespace TP3Client;

/// <summary>
/// Cr�e un nouveau <see cref="Other"/>.
/// </summary>
/// <param name="font">La police utilis�e pour afficher le nom du joueur.</param>
Other::Other(const sf::Font& font)
	: Square(font)
{
	// Initialisation du timer interne.
	innerTimer = sf::milliseconds(0);
	timeStarted = getSceneJeu()->getCurrentTime();
}

/// <summary>
/// D�truit un <see cref="Other"/>.
/// </summary>
Other::~Other()
{
	while (!commands.getCommands().empty())
	{
		commands.removeCommand();
	}
}

/// <summary>
/// Prend en charge une commande.
/// Fonction d�pr�ci�e (� ne pas utiliser).
/// </summary>
/// <param name="command">La commande.</param>
void Other::addCommand(OtherCommand_Base* command)
{
	commands.storeCommand(command);
}

/// <summary>
/// Update le carr� selon le temps �coul�.
/// </summary>
/// <param name="deltaT">Le nombre de secondes depuis le dernier appel de la m�thode.</param>
void Other::update(const float deltaT)
{
	Square::update(deltaT);
	innerTimer += sf::seconds(deltaT);

	if (innerTimer.asMilliseconds() >= MS_BEFORE_DECONNEXION)
	{
		getSceneJeu()->removeObject(this);
	}

	// Inutile, car les commandes ne sont pas utilis�s.
	//// Si on a d�pass� le d�lais, le joueur peut commencer � bouger...
	//if (innerTimer >= timeStarted)
	//{
	//	// S'il y a des commandes dans la liste...
	//	if (!commands.getCommands().empty())
	//	{
	//		// On cherche la commande � ex�cuter selon le d�calage...
	//		int currentCommand = 0;
	//		size_t size = commands.getCommands().size();
	//		while (currentCommand < size && innerTimer > commands.getCommands()[currentCommand]->whenHappened)
	//		{
	//			currentCommand++;
	//		}
	//		currentCommand--;

	//		// Si on a trouv� une commande � effectu� selon le d�calage...
	//		if (currentCommand != -1)
	//		{
	//			// Le joueur ex�cute la commande.
	//			commands.getCommands()[currentCommand]->execute(this);
	//			commands.removeCommand();
	//		}
	//	}
	//}
}

/// <summary>
/// Change la direction du joueur.
/// </summary>
/// <param name="direction">La nouvelle direction.</param>
void Other::changeDirection(const sf::Vector2f& direction)
{
	innerTimer = sf::microseconds(0);
	setDirection(direction);
}

/// <summary>
/// Change la position du joueur.
/// </summary>
/// <param name="direction">La nouvelle position.</param>
void Other::changePosition(const sf::Vector2f& position)
{
	innerTimer = sf::microseconds(0);
	setPosition(position);
}

