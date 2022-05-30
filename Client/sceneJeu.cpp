#include "sceneJeu.h"
#include "movable.h"
#include <thread>
#include "joueur.h"
#include "other.h"
#include "commands.h"
#include "gameObject.h"

using namespace TP3Client;

SceneJeu::SceneJeu()
	: tempsDernierUpdate(sf::microseconds(0)), tempsSalon(sf::microseconds(0)), firstPlayerName(""), joueur(nullptr), removeLock(false), objectLock(false)
{
	srand(time(NULL));
	client = GestionnaireDeCommunication::getInstance();
}

/// <summary>
/// Détruit la <see cref="SceneJeu"/>.
/// </summary>
SceneJeu::~SceneJeu()
{
	for (GameObject* object : allGameObjects)
	{
		delete object;
	}
}

/// <summary>
/// Joue la scène de jeu.
/// </summary>
/// <returns>La prochaine scène à jouer. </returns>
Scene::scenes SceneJeu::run()
{
	std::thread clientThread(&GestionnaireDeCommunication::run, client); 

	client->sync(this, "Guest");

	while (isRunning)
	{
		getInputs();
		update();
		draw();
	}

	deconnexion();
	clientThread.join();

	return transitionVersScene;
}

/// <summary>
/// Retourne le temps du salon de jeu.
/// </summary>
/// <returns>Le temps du salon de jeu.</returns>
const sf::Time& SceneJeu::getCurrentTime() const
{
	return tempsSalon;
}

/// <summary>
/// Retourne le nom du joueur.
/// </summary>
/// <returns>Le nom du joueur.</returns>
const std::string SceneJeu::getPlayerName() const
{
	std::string nameString = joueur->getName();
	return nameString;
}

/// <summary>
/// Change le nom du joueur.
/// </summary>
/// <param name="newName">Le nouveau nom du joueur.</param>
void SceneJeu::changePlayerName(const std::string& newName)
{
	if (joueur)
	{
		joueur->setName(newName);
	}
	else
	{
		firstPlayerName = newName;
	}
}

/// <summary>
/// Initialise la scène de jeu.
/// </summary>
/// <param name="window">Le fenêtre utilisée par la scène de jeu.</param>
/// <returns>true si la scène s'est initialisée correctement; false sinon.</returns>
bool SceneJeu::init(sf::RenderWindow * const window)
{
	if (!client->init())
	{
		return false;
	}
	if (!font.loadFromFile("Ressources\\Fonts\\vcr.ttf"))
	{
		return false;
	}

	if (!backgroundT.loadFromFile("Ressources\\Sprites\\background.png"))
	{
		return false;
	}

	background.setTexture(backgroundT);

	joueur = new Joueur(font);
	allGameObjects.push_back(joueur);
	if (firstPlayerName != "")
	{
		joueur->setName(firstPlayerName);
	}

	chat = new Chat(font, joueur);
	chat->setPosition(0, GAME_TOP + GAME_HEIGHT - chat->getDimension().y);
	allGameObjects.push_back(chat);

	GameObject::setSceneJeu(this);

	mainWin = window;
	tempsDernierUpdate = sf::microseconds(0);
	timer.restart();
	isRunning = true;
	transitionVersScene = scenes::JEU;
	return true;
}

/// <summary>
/// Gère toutes les commandes entrées dans la scène de jeu.
/// </summary>
void SceneJeu::getInputs()
{
	bool keyPressed = false;
	while (mainWin->pollEvent(event))
	{
		// Si le chat est sélectionné, il gère ses events.
		if (chat->estSelectionne())
		{
			chat->getEvents(event);
		}

		if (timer.getElapsedTime().asSeconds() >= 2)
		{
			// Si le joueur ferme la fenêtre...
			if (event.type == sf::Event::Closed)
			{
				// On se déconnecte vers la scène de sorte.
				client->deconnection();
				transitionVersScene = Scene::scenes::SORTIE;
			}
		}


		if (event.type == Event::KeyPressed)
		{
			keyPressed = true;

			if (timer.getElapsedTime().asSeconds() >= 2)
			{
				// Si le joueur appuie sur ESC
				if (event.key.code == Keyboard::Escape)
				{
					// On se déconnecte vers la scène titre.
					client->deconnection();
					transitionVersScene = Scene::scenes::TITRE;
				}
			}

			// Si le joueur appuie sur enter...
			if (event.key.code == Keyboard::Return)
			{

				// On change le focus.
				if (chat->estSelectionne())
				{
					chat->deSelectionner();
				}
				else
				{
					chat->selectionner();
				}
			}
		}
		if (event.type == Event::KeyReleased)
		{
			keyPressed = true;
		}

		// Si on a un événement de click de souris
		if (event.type == Event::MouseButtonPressed)
		{
			if (chat->touche(Mouse::getPosition(*mainWin)))
			{
				chat->selectionner();
			}
			else
			{
				chat->deSelectionner();
			}
		}
	}

	// Si le chat est sélectionné, le joueur ne bouge plus.
	if (keyPressed && !chat->estSelectionne())
	{
		joueur->getInputs();
	}
}

/// <summary>
/// Met à jour la scène de jeu.
/// </summary>
void SceneJeu::update()
{	

	// Gère le temps.
	float deltaT = timer.getElapsedTime().asSeconds() - tempsDernierUpdate.asSeconds();
	tempsSalon += timer.getElapsedTime() - tempsDernierUpdate;
	tempsDernierUpdate = timer.getElapsedTime();

	// Enlève les objets à enlever.
	while (removeLock)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	removeLock = true;

	for (GameObject* object : objectsToRemove)
	{
		allGameObjects.remove(object);
	}

	removeLock = false;

	// Met à jour tous les objets.
	for (GameObject* object : allGameObjects)
	{
		if (Movable* movable = dynamic_cast<Movable*>(object))
		{
			if (Square* square = dynamic_cast<Square*>(movable))
			{
				sf::Vector2f oldPos = square->getPosition();
				square->update(deltaT);
				if (enCollisionAvecAutresJoueurs(square))
				{
					square->setPosition(oldPos);
				}
			}
			else
			{
				movable->update(deltaT);
			}
		}
	}
}

/// <summary>
/// Enlève un objet du jeu.
/// </summary>
/// <param name="object">L'objet à enlever.</param>
void SceneJeu::removeObject(GameObject* object)
{
	while (removeLock)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	removeLock = true;

	if (std::find(allGameObjects.begin(), allGameObjects.end(), object) != allGameObjects.end())
	{
		objectsToRemove.push_back(object);
	}

	removeLock = false;
}

bool SceneJeu::enCollisionAvecAutresJoueurs(const Square* player)
{
	for (std::pair<std::string, Other*> other : allOtherPlayers)
	{
		if (player != other.second)
		{
			if (player->collidesWith(*other.second))
			{
				return true;
			}
		}
	}

	if (player != joueur)
	{
		if (player->collidesWith(*joueur))
		{
			return true;
		}
	}

	return false;
}

/// <summary>
/// Affiche la scène de jeu.
/// </summary>
void SceneJeu::draw()
{
	mainWin->clear();

	mainWin->draw(background);

	while (objectLock)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	objectLock = true;

	for (GameObject* object : allGameObjects)
	{
		object->draw(*mainWin);
	}

	objectLock = false;

	mainWin->display();
}

/// <summary>
/// Ajoute un joueur au jeu.
/// </summary>
/// <param name="username">Le nom du joueur ajouté.</param>
void SceneJeu::ajouterJoueur(const std::string& username)
{
	if (allOtherPlayers.find(username) == allOtherPlayers.end())
	{
		while (objectLock)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		objectLock = true;

		Other* newPlayer = new Other(font);
		newPlayer->setName(username);
		allOtherPlayers[username] = newPlayer;
		allGameObjects.push_front(newPlayer);

		objectLock = false;
	}
}

/// <summary>
/// Enlève un joueur du jeu.
/// </summary>
/// <param name="username">Le nom du joueur à enlever.</param>
void SceneJeu::enleverJoueur(const std::string& username)
{
	if (allOtherPlayers.find(username) != allOtherPlayers.end())
	{
		while (objectLock)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		objectLock = true;

		Other* it = allOtherPlayers[username];
		delete it;
		allOtherPlayers.erase(username);
		allGameObjects.remove(it);

		objectLock = false;
	}
}

/// <summary>
/// Fait parler un joueur.
/// </summary>
/// <param name="username">Le nom du joueur qui parle.</param>
/// <param name="message">Le message entré par le joueur qui parle.</param>
void SceneJeu::parler(const std::string& username, const std::string& message)
{
	if (allOtherPlayers.find(username) != allOtherPlayers.end())
	{
		allOtherPlayers[username]->parler(message);
	}
	chat->ajouterMessage(username, message);
}

/// <summary>
/// Fait parler un joueur sur plusieurs salons.
/// </summary>
/// <param name="username">Le joueur qui a parlé.</param>
/// <param name="message">Le message envoyé aux salons.</param>
void SceneJeu::notifier(const std::string& username, const std::string& message)
{
	chat->ajouterNotification(username, message);
}

/// <summary>
/// Change la direction d'un joueur.
/// </summary>
/// <param name="username">Le nom du joueur qui a changé de direction.</param>
/// <param name="direction">La nouvelle direction du joueur.</param>
/// <param name="whenHappened">Le temps salon quand le changement de direction a été effectué.</param>
void SceneJeu::changerDirection(const std::string& username, const sf::Vector2f direction, const sf::Time whenHappened)
{
	if (allOtherPlayers.find(username) != allOtherPlayers.end())
	{
		allOtherPlayers[username]->changeDirection(direction);
		//allOtherPlayers[username]->addCommand(new OtherCommand_ChangeDirection(direction, whenHappened));
	}
}

/// <summary>
/// Change la position d'un joueur.
/// </summary>
/// <param name="username">Le nom du joueur qui a changé de position.</param>
/// <param name="direction">La nouvelle position du joueur.</param>
/// <param name="whenHappened">Le temps salon quand le changement de position a été effectué.</param>
void SceneJeu::changerPosition(const std::string& username, const sf::Vector2f position, const sf::Time whenHappened)
{
	if (username == joueur->getName())
	{
		joueur->setPosition(position);
	}
	else
	{
		if (allOtherPlayers.find(username) != allOtherPlayers.end())
		{
			allOtherPlayers[username]->changePosition(position);
			//allOtherPlayers[username]->addCommand(new OtherCommand_ChangePosition(position, whenHappened));
		}
		else
		{
			if (timer.getElapsedTime().asMilliseconds() >= 5000)
			{
				ajouterJoueur(username);
				allOtherPlayers[username]->changePosition(position);
			}
		}
	}
}

/// <summary>
/// Met à jour le temps du jeu avec le temps du salon.
/// </summary>
/// <param name="temps">Le temps du salon.</param>
void SceneJeu::mettreAJourTemps(const sf::Time& temps)
{
	tempsSalon = temps;
}

/// <summary>
/// Met à jour le nom du joueur.
/// </summary>
/// <param name="name">Le nouveau nom du joueur.</param>
void SceneJeu::mettreAJoutNom(const std::string& name)
{
	joueur->setName(name);
}

/// <summary>
/// Indique au jeu qu'il y a eu déconnexion.
/// </summary>
void SceneJeu::deconnexion()
{
	isRunning = false;

	if (transitionVersScene != scenes::SORTIE)
	{
		transitionVersScene = scenes::TITRE;
	}
}