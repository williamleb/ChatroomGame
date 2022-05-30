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
/// Cr�e un nouveau <see cref="GestionnaireDeCommunication"/>.
/// </summary>
GestionnaireDeCommunication::GestionnaireDeCommunication()
{
}

/// <summary>
/// D�truit un <see cref="GestionnaireDeCommunication"/>.
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
/// D�truit l'instance en cours du gestionnaire.
/// </summary>
void GestionnaireDeCommunication::deleteInstance()
{
	delete instance;
	instance = nullptr;
}

/// <summary>
/// Initialise la communication avec le serveur.
/// </summary>
/// <returns>true si la communication a �t� bien initialis�e; false sinon.</returns>
bool GestionnaireDeCommunication::init()
{
	if (!client.init())
	{
		return false;
	}
	return true;
}

/// <summary>
/// Ex�cute la communication avec le serveur.
/// La communication doit avoir �t� initialis�e avant d'appeler cette m�thode.
/// Cette m�thode est blocante.	
/// </summary>
void GestionnaireDeCommunication::run()
{
	client.run();

	sceneJeu->deconnexion();
}

/// <summary>
/// Synchronise les informations du serveur avec ceux du client.
/// Doit �tre appel�e avant de commencer la partie.
/// </summary>
/// <param name="sceneJeu">La sc�ne de jeu.</param>
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
/// <param name="message">Le message envoy� aux autres utilisateurs.</param>
void GestionnaireDeCommunication::sendMessage(const std::string& message)
{
	// Si c'est une option...
	if ((message[0] == '\\' || message[0] == '\/'))
	{
		// Si c'est /g, c'est un message g�n�ral
		if (tolower(message[1]) == 'g')
		{
			std::string newMessage = message;
			
			// On enl�ve le /g et le premier espace, s'il y en a un, du message g�n�ral.
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
/// G�re les messages entrants du serveur.
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
/// Arr�te la communication avec le serveur.
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
	// TODO: V�rifications.
	Client::setIP(ip);
}

/// <summary>
/// G�re un message du serveur avec le code "01".
/// Message qui indique qu'un joueur s'est connect�.
/// </summary>
/// <param name="message">Le message envoy� par le serveur.</param>
void GestionnaireDeCommunication::manageCode01(const std::string& message)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond au nom du joueur connect�.
		if (i == 1)
		{
			// On ajoute le joueur.
			sceneJeu->ajouterJoueur(line);

			std::cout << "Le joueur " << line << " s'est connect� � votre salon." << std::endl << std::endl;
		}
		// La troisi�me ligne correspond au temps salon.
		else if (i == 2)
		{
			// On met � jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// G�re un message du serveur avec le code "02".
/// Message qui indique qu'un joueur s'est d�connect�.
/// </summary>
/// <param name="message">Le message envoy� par le serveur.</param>
void GestionnaireDeCommunication::manageCode02(const std::string& message)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond au nom du joueur d�connect�.
		if (i == 1)
		{
			// On retire le joueur.
			sceneJeu->enleverJoueur(line);

			std::cout << "Le joueur " << line << " s'est d�connect�." << std::endl << std::endl;
		}
		// La troisi�me ligne correspond au temps salon.
		else if (i == 2)
		{
			// On met � jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// G�re un message du serveur avec le code "03".
/// Message qui indique qu'un joueur a envoy� un message.
/// </summary>
/// <param name="message">Le message envoy� par le serveur.</param>
void GestionnaireDeCommunication::manageCode03(const std::string& message)
{
	std::istringstream f(message);

	std::string nom;
	std::string line;
	for (unsigned int i = 0; i < 4; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond au nom du joueur qui envoit un message.
		if (i == 1)
		{
			nom = line;
		}
		// La troisi�me ligne correspond au message envoy�.
		else if (i == 2)
		{
			// On envoit le message.
			sceneJeu->parler(nom, line);

			std::cout << "Le joueur " << nom << " a envoy�: " << std::endl << line << std::endl << std::endl;
		}
		// La quatri�me ligne correspond au temps salon.
		else if (i == 3)
		{
			// On met � jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// G�re un message du serveur avec le code "04".
/// Message qui indique qu'un joueur a envoy� un message global.
/// </summary>
/// <param name="message">Le message envoy� par le serveur.</param>
void GestionnaireDeCommunication::manageCode04(const std::string& message)
{
	std::istringstream f(message);

	std::string nom;
	std::string line;
	for (unsigned int i = 0; i < 4; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond au nom du joueur qui envoit un message.
		if (i == 1)
		{
			nom = line;
		}
		// La troisi�me ligne correspond au message global envoy�.
		else if (i == 2)
		{
			// On envoit le message global.
			sceneJeu->notifier(nom, line);

			std::cout << "Le joueur " << nom << " a envoy� � tous les salons: " << std::endl << line << std::endl << std::endl;
		}
		// La quatri�me ligne correspond au temps salon.
		else if (i == 3)
		{
			// On met � jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// G�re un message du serveur avec le code "05".
/// Message qui indique qu'un joueur a chang� de direction.
/// </summary>
/// <param name="message">Le message envoy� par le serveur.</param>
void GestionnaireDeCommunication::manageCode05(const std::string& message)
{
	std::istringstream f(message);

	std::string nom;
	sf::Vector2f direction;
	std::string line;
	for (unsigned int i = 0; i < 5; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond au nom du joueur qui a chang� de direction.
		if (i == 1)
		{
			nom = line;
		}
		// La troisi�me ligne correspond  la nouvelle direction du joueur.
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

				// La premi�re coordonn�e est celle en x.
				if (j == 0)
				{
					h >> x;
				}
				// La deuxi�me est celle en y.
				else if (j == 1)
				{
					h >> y;
				}
			}

			direction.x = x;
			direction.y = y;
		}
		// La quatri�me ligne correspond au temps o� l'action a �t� effectu�e.
		else if (i == 3)
		{
			// On envoit la direction.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->changerDirection(nom, direction, sf::microseconds(temps));
		}
		// La cinqui�me ligne correspond au temps salon.
		else if (i == 4)
		{
			// On met � jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}


/// <summary>
/// G�re un message du serveur avec le code "06".
/// Message qui indique qu'un joueur a chang� de position.
/// </summary>
/// <param name="message">Le message envoy� par le serveur.</param>
void GestionnaireDeCommunication::manageCode06(const std::string& message)
{
	std::istringstream f(message);

	std::string nom;
	sf::Vector2f position;
	std::string line;
	for (unsigned int i = 0; i < 5; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond au nom du joueur qui a chang� de position.
		if (i == 1)
		{
			nom = line;
		}
		// La troisi�me ligne correspond la nouvelle position du joueur.
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

				// La premi�re coordonn�e est celle en x.
				if (j == 0)
				{
					h >> x;
				}
				// La deuxi�me est celle en y.
				else if (j == 1)
				{
					h >> y;
				}
			}

			position.x = x;
			position.y = y;
		}
		// La quatri�me ligne correspond au temps o� l'action a �t� effectu�e.
		else if (i == 3)
		{
			// On envoit la direction.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->changerPosition(nom, position, sf::microseconds(temps));
		}
		// La cinqui�me ligne correspond au temps salon.
		else if (i == 4)
		{
			// On met � jour le temps.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sceneJeu->mettreAJourTemps(sf::microseconds(temps));
		}
	}
}

/// <summary>
/// G�re un message du serveur avec le code "07".
/// Message qui est envoy� pour initialiser le jeu local.
/// </summary>
/// <param name="message">Le message envoy� par le serveur.</param>
void GestionnaireDeCommunication::manageCode07(const std::string& message)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond au nouveau nom du joueur.
		if (i == 1)
		{
			// On met � jour le nom.
			sceneJeu->mettreAJoutNom(line);

			std::cout << "Votre nouveau nom est " << line << std::endl << std::endl;
		}
		// La troisi�me ligne correspond au temps salon.
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
/// G�re un message du serveur avec le code "99".
/// Message qui signifie la fin de la connexion.
/// </summary>
void GestionnaireDeCommunication::manageCode99()
{
	client.stop();
}