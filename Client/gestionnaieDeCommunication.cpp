#include "gestionnaireDeCommunication.h"
#include "sceneJeu.h"
#include <sstream>
#include <thread>
#include <iostream>

using namespace TP3Client;

/// <summary>
/// L'instance en cours du gestionnaire.
/// </summary>
GestionnaireDeCommunication* GestionnaireDeCommunication::instance = nullptr;

/// <summary>
/// Crée un nouveau <see cref="GestionnaireDeCommunication"/>.
/// </summary>
GestionnaireDeCommunication::GestionnaireDeCommunication()
{
}

/// <summary>
/// Détruit un <see cref="GestionnaireDeCommunication"/>.
/// </summary>
GestionnaireDeCommunication::~GestionnaireDeCommunication()
{
	client.unload();
}

/// <summary>
/// Retourne l'instance en cours du gestionnaire.
/// </summary>
/// <returns>L'instance en cours du gestionnaire.</returns>
GestionnaireDeCommunication* GestionnaireDeCommunication::getInstance()
{
	if (instance == nullptr)
	{
		instance = new GestionnaireDeCommunication();
	}

	return instance;
}

/// <summary>
/// Détruit l'instance en cours du gestionnaire.
/// </summary>
void GestionnaireDeCommunication::deleteInstance()
{
	delete instance;
	instance = nullptr;
}

/// <summary>
/// Initialise la communication avec le serveur.
/// </summary>
/// <returns>true si la communication a été bien initialisée; false sinon.</returns>
bool GestionnaireDeCommunication::init()
{
	if (!client.init())
	{
		return false;
	}
	return true;
}

/// <summary>
/// Exécute la communication avec le serveur.
/// La communication doit avoir été initialisée avant d'appeler cette méthode.
/// Cette méthode est blocante.	
/// </summary>
void GestionnaireDeCommunication::run()
{
	client.run();

	sceneJeu->deconnexion();
}

/// <summary>
/// Synchronise les informations du serveur avec ceux du client.
/// Doit être appelée avant de commencer la partie.
/// </summary>
/// <param name="sceneJeu">La scène de jeu.</param>
/// <param name="utilisateur">Le nom d'utilisateur du joueur.</param>
void GestionnaireDeCommunication::sync(SceneJeu* sceneJeu, const std::string& utilisateur)
{
	std::stringstream toServer;
	this->sceneJeu = sceneJeu;
	std::string salut = sceneJeu->getPlayerName();
	toServer << "01" << "\n" << sceneJeu->getPlayerName() << "\n" << sceneJeu->getCurrentTime().asMicroseconds() << '\r';
	
	client.sendToServer(toServer.str());
}

/// <summary>
/// Envoie un message aux autres utilisateurs.
/// </summary>
/// <param name="message">Le message envoyé aux autres utilisateurs.</param>
void GestionnaireDeCommunication::sendMessage(const std::string& message)
{
	// Si c'est une option...
	if ((message[0] == '\\' || message[0] == '\/'))
	{
		// Si c'est /g, c'est un message général
		if (tolower(message[1]) == 'g')
		{
			std::string newMessage = message;
			
			// On enlève le /g et le premier espace, s'il y en a un, du message général.
			if (message[2] == ' ')
			{
				newMessage.erase(0, 3);
			}
			else
			{
				newMessage.erase(0, 2);
			}

			std::stringstream toServer;
			toServer << "04" << "\n" << newMessage << "\n" << sceneJeu->getCurrentTime().asMicroseconds() << '\r';

			client.sendToServer(toServer.str());
		}
	}
	else
	{
		std::stringstream toServer;
		toServer << "03" << "\n" << message << "\n" << sceneJeu->getCurrentTime().asMicroseconds() << '\r';

		client.sendToServer(toServer.str());

	}
}

/// <summary>
/// Envoie la nouvelle direction prise par le joueur.
/// </summary>
/// <param name="direction">La nouvelle direction prise par le joueur.</param>
void GestionnaireDeCommunication::sendDirection(const sf::Vector2f& direction)
{
	std::stringstream toServer;
	toServer << "05" << "\n" << direction.x << ',' << direction.y << "\n" << sceneJeu->getCurrentTime().asMicroseconds() << '\r';

	client.sendToServer(toServer.str());
}

/// <summary>
/// Gère les messages entrants du serveur.
/// </summary>
/// <param name="message">Le message du serveur.</param>
void GestionnaireDeCommunication::receiveMessage(const std::string& message)
{
	std::istringstream f(message);

	std::string line;
	while (std::getline(f, line, '\r'))
	{
		std::string code = line.substr(0, 2);

		if (code == "01")
		{
			manageCode01(line);
		}
		else if (code == "02")
		{
			manageCode02(line);
		}
		else if (code == "03")
		{
			manageCode03(line);
		}
		else if (code == "04")
		{
			manageCode04(line);
		}
		else if (code == "05")
		{
			manageCode05(line);
		}
		else if (code == "06")
		{
			manageCode06(line);
		}
		else if (code == "07")
		{
			manageCode07(line);
		}
		else if (code == "99")
		{
			manageCode99();
		}
	}
}

/// <summary>
/// Arrête la communication avec le serveur.
/// </summary>
void GestionnaireDeCommunication::deconnection()
{
	client.sendToServer("99\0");
}

/// <summary>
/// Change l'adresse IP de la connexion.
/// </summary>
/// <param name="ip">Ladresse IP de la connexion.</param>
void GestionnaireDeCommunication::changeIP(const std::string& ip)
{
	// TODO: Vérifications.
	Client::setIP(ip);
}

/// <summary>
/// Gère un message du serveur avec le code "01".
/// Message qui indique qu'un joueur s'est connecté.
/// </summary>
/// <param name="message">Le message envoyé par le serveur.</param>
void GestionnaireDeCommunication::manageCode01(const std::string& message)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxième ligne correspond au nom du joueur connecté.
		if (i == 1)
		{
			// On ajoute le joueur.
			sceneJeu->ajouterJoueur(line);

			std::cout << "Le joueur " << line << " s'est connecté à votre salon." << std::endl << std::endl;
		}
		// La troisième ligne correspond au temps salon.
		else if (i == 2)
		{
			// On met à jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// Gère un message du serveur avec le code "02".
/// Message qui indique qu'un joueur s'est déconnecté.
/// </summary>
/// <param name="message">Le message envoyé par le serveur.</param>
void GestionnaireDeCommunication::manageCode02(const std::string& message)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxième ligne correspond au nom du joueur déconnecté.
		if (i == 1)
		{
			// On retire le joueur.
			sceneJeu->enleverJoueur(line);

			std::cout << "Le joueur " << line << " s'est déconnecté." << std::endl << std::endl;
		}
		// La troisième ligne correspond au temps salon.
		else if (i == 2)
		{
			// On met à jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// Gère un message du serveur avec le code "03".
/// Message qui indique qu'un joueur a envoyé un message.
/// </summary>
/// <param name="message">Le message envoyé par le serveur.</param>
void GestionnaireDeCommunication::manageCode03(const std::string& message)
{
	std::istringstream f(message);

	std::string nom;
	std::string line;
	for (unsigned int i = 0; i < 4; i++)
	{
		std::getline(f, line, '\n');

		// La deuxième ligne correspond au nom du joueur qui envoit un message.
		if (i == 1)
		{
			nom = line;
		}
		// La troisième ligne correspond au message envoyé.
		else if (i == 2)
		{
			// On envoit le message.
			sceneJeu->parler(nom, line);

			std::cout << "Le joueur " << nom << " a envoyé: " << std::endl << line << std::endl << std::endl;
		}
		// La quatrième ligne correspond au temps salon.
		else if (i == 3)
		{
			// On met à jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// Gère un message du serveur avec le code "04".
/// Message qui indique qu'un joueur a envoyé un message global.
/// </summary>
/// <param name="message">Le message envoyé par le serveur.</param>
void GestionnaireDeCommunication::manageCode04(const std::string& message)
{
	std::istringstream f(message);

	std::string nom;
	std::string line;
	for (unsigned int i = 0; i < 4; i++)
	{
		std::getline(f, line, '\n');

		// La deuxième ligne correspond au nom du joueur qui envoit un message.
		if (i == 1)
		{
			nom = line;
		}
		// La troisième ligne correspond au message global envoyé.
		else if (i == 2)
		{
			// On envoit le message global.
			sceneJeu->notifier(nom, line);

			std::cout << "Le joueur " << nom << " a envoyé à tous les salons: " << std::endl << line << std::endl << std::endl;
		}
		// La quatrième ligne correspond au temps salon.
		else if (i == 3)
		{
			// On met à jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// Gère un message du serveur avec le code "05".
/// Message qui indique qu'un joueur a changé de direction.
/// </summary>
/// <param name="message">Le message envoyé par le serveur.</param>
void GestionnaireDeCommunication::manageCode05(const std::string& message)
{
	std::istringstream f(message);

	std::string nom;
	sf::Vector2f direction;
	std::string line;
	for (unsigned int i = 0; i < 5; i++)
	{
		std::getline(f, line, '\n');

		// La deuxième ligne correspond au nom du joueur qui a changé de direction.
		if (i == 1)
		{
			nom = line;
		}
		// La troisième ligne correspond  la nouvelle direction du joueur.
		else if (i == 2)
		{
			// On convertit la direction...
			float x = 0;
			float y = 0;

			std::istringstream g(line);

			std::string coord;
			for (unsigned int j = 0; j < 2; j++)
			{
				std::getline(g, coord, ',');

				std::istringstream h(coord);

				// La première coordonnée est celle en x.
				if (j == 0)
				{
					h >> x;
				}
				// La deuxième est celle en y.
				else if (j == 1)
				{
					h >> y;
				}
			}

			direction.x = x;
			direction.y = y;
		}
		// La quatrième ligne correspond au temps où l'action a été effectuée.
		else if (i == 3)
		{
			// On envoit la direction.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->changerDirection(nom, direction, sf::microseconds(temps));
		}
		// La cinquième ligne correspond au temps salon.
		else if (i == 4)
		{
			// On met à jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}


/// <summary>
/// Gère un message du serveur avec le code "06".
/// Message qui indique qu'un joueur a changé de position.
/// </summary>
/// <param name="message">Le message envoyé par le serveur.</param>
void GestionnaireDeCommunication::manageCode06(const std::string& message)
{
	std::istringstream f(message);

	std::string nom;
	sf::Vector2f position;
	std::string line;
	for (unsigned int i = 0; i < 5; i++)
	{
		std::getline(f, line, '\n');

		// La deuxième ligne correspond au nom du joueur qui a changé de position.
		if (i == 1)
		{
			nom = line;
		}
		// La troisième ligne correspond la nouvelle position du joueur.
		else if (i == 2)
		{
			// On convertit la position...
			float x = 0;
			float y = 0;

			std::istringstream g(line);

			std::string coord;
			for (unsigned int j = 0; j < 2; j++)
			{
				std::getline(g, coord, ',');

				std::istringstream h(coord);

				// La première coordonnée est celle en x.
				if (j == 0)
				{
					h >> x;
				}
				// La deuxième est celle en y.
				else if (j == 1)
				{
					h >> y;
				}
			}

			position.x = x;
			position.y = y;
		}
		// La quatrième ligne correspond au temps où l'action a été effectuée.
		else if (i == 3)
		{
			// On envoit la direction.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->changerPosition(nom, position, sf::microseconds(temps));
		}
		// La cinquième ligne correspond au temps salon.
		else if (i == 4)
		{
			// On met à jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// Gère un message du serveur avec le code "07".
/// Message qui est envoyé pour initialiser le jeu local.
/// </summary>
/// <param name="message">Le message envoyé par le serveur.</param>
void GestionnaireDeCommunication::manageCode07(const std::string& message)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxième ligne correspond au nouveau nom du joueur.
		if (i == 1)
		{
			// On met à jour le nom.
			sceneJeu->mettreAJoutNom(line);

			std::cout << "Votre nouveau nom est " << line << std::endl << std::endl;
		}
		// La troisième ligne correspond au temps salon.
		else if (i == 2)
		{
			// On initialise le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// Gère un message du serveur avec le code "99".
/// Message qui signifie la fin de la connexion.
/// </summary>
void GestionnaireDeCommunication::manageCode99()
{
	client.stop();
}