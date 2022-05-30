#include "jeu.h"
#include "salon.h"
#include "joueur.h"
#include <iostream>
#include <time.h>
#include <sstream>
#include <algorithm>

using namespace TP3Serveur;

// Gestion du singleton ---------------------------------------------------------------------------
/// <summary>
/// L'instance en cours de la repr�sentation du jeu.
/// </summary>
Jeu* Jeu::instance = nullptr;

/// <summary>
/// Cr�e un nouveau <see cref="Jeu"/>.
/// </summary>
Jeu::Jeu()
	:isRunning(false)
{
	srand(time(NULL));

	tempsDernierUpdate = std::chrono::high_resolution_clock::now();
}

/// <summary>
/// D�truit un <see cref="Jeu"/>.
/// </summary>
Jeu::~Jeu()
{
	// On d�truit tous les salons.
	for (Salon* salon : tousLesSalons)
	{
		delete salon;
	}
}

/// <summary>
/// Retourne l'instance en cours de la repr�sentation du jeu.
/// </summary>
/// <returns>L'instance en cours de la repr�sentation du jeu.</returns>
Jeu* Jeu::getInstance()
{
	if (instance == nullptr)
	{
		instance = new Jeu();
	}

	return instance;
}

/// <summary>
/// D�truit l'instance en cours de la repr�sentation du jeu.
/// </summary>
void Jeu::deleteInstance()
{
	delete instance;
	instance = nullptr;
}

// Gestion du jeu ---------------------------------------------------------------------------------
/// <summary>
/// Fait rouler la repr�sentation du jeu.
/// </summary>
void Jeu::run()
{
	srand(time(NULL));
	isRunning = true;

	// Tant que la repr�sentation du jeu est en marche...
	while (isRunning)
	{
		//On le met � jour.
		update();
	}
}

/// <summary>
/// Arr�te l'ex�cution de la repr�sentation du jeu.
/// </summary>
void Jeu::stop()
{

	for (Salon* salon : tousLesSalons)
	{
		salon->fermerToutesConnexions();
	}

	isRunning = false;
}

/// <summary>
/// Met � jour la repr�sentation du jeu.
/// </summary>
void Jeu::update()
{
	std::chrono::steady_clock::duration deltaT = std::chrono::high_resolution_clock::now() - tempsDernierUpdate;
	tempsDernierUpdate = std::chrono::high_resolution_clock::now();

	// On update tous les salons.
	for (Salon* salon : tousLesSalons)
	{
		salon->update(std::chrono::duration_cast<std::chrono::microseconds>(deltaT).count());
	}
}

void Jeu::retirerSalon(Salon* salon)
{
	tousLesSalons.remove(salon);
	delete salon;
}

// Gestion de la connexion ------------------------------------------------------------------------
/// <summary>
/// Attribut un salon au joueur qui en fait la demande.
/// </summary>
/// <param name="joueur">Le joueur qui fait la demande d'un salon.</param>
/// <returns>Le salon choisi pour le joueur.</returns>
Salon* Jeu::attacherAUnSalon(Joueur* joueur)
{
	Salon* salonTrouve = nullptr;

	for (Salon* salon : tousLesSalons)
	{
		// Si, parmis les salons d�j� cr��s, on en trouve un qui peut encore accueillir
		// des joueurs, on le s�lectionne.
		if (!salon->isFull())
		{
			salonTrouve = salon;
			break;
		}
	}

	// Si on n'a pas trouv� de salon ad�quat, on en cr�e un nouveau.
	if (salonTrouve == nullptr)
	{
		salonTrouve = new Salon();
		tousLesSalons.push_back(salonTrouve);
	}
	
	// On change le nom du joueur selon ce qu'il y a dans le salon.
	joueur->setNom(salonTrouve->getNouveauNom());

	// Finalement, on ajoute le joueur au salon.
	salonTrouve->ajouterJoueur(joueur);

	return salonTrouve;
}

/// <summary>
/// G�re la r�ception d'un message par un client.
/// </summary>
/// <param name="message">Le message re�u.</param>
/// <param name="client">Le client qui a re�u le message.</param>
void Jeu::reveiveMessage(const std::string& message, Joueur* client)
{
	std::istringstream f(message);

	std::string line;
	while (std::getline(f, line, '\r'))
	{
		std::string code = line.substr(0, 2);

		if (code == "01")
		{
			gererCode01(line, client);
		}
		else if (code == "03")
		{
			gererCode03(line, client);
		}
		else if (code == "04")
		{
			gererCode04(line, client);
		}
		else if (code == "05")
		{
			gererCode05(line, client);
		}
		else if (code == "99")
		{
			gererCode99(client);
		}
	}
}

/// <summary>
/// G�re une message du client avec le code "01".
/// Message qui indique que le client vient de se connecter.
/// </summary>
/// <param name="message">Le message envoy�.</param>
/// <param name="client">Le joueur qui a re�u le message de son client.</param>
void Jeu::gererCode01(const std::string& message, Joueur* client)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond au nom du joueur connect�.
		if (i == 1)
		{
			// On change le nom du client.
			client->setNom(client->getSalon()->ajusterNom(line));

			std::cout << "Connexion du joueur " << client->getNom() << std::endl;

			// On lui envoie un message d'initialisation.
			sendInitialisation(client);
			client->initialiser();

			// Puis on envoit aux autres joueurs qu'il s'est connect�.
			sendConnexion(client);
		}
	}
}

/// <summary>
/// G�re une message du client avec le code "03".
/// Message qui indique que le client vient d'envoyer un message.
/// </summary>
/// <param name="message">Le message envoy�.</param>
/// <param name="client">Le joueur qui a re�u le message de son client.</param>
void Jeu::gererCode03(const std::string& message, Joueur* client)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond message envoy� par le joueur.
		if (i == 1)
		{
			// On envoie le message aux joueurs.
			sendMessage(client, line);

			std::cout << "Le joueur " << client->getNom() << " a dit: " << std::endl << line << std::endl;
		}
	}
}

/// <summary>
/// G�re une message du client avec le code "04".
/// Message qui indique que le client vient d'envoyer un message global (pour tous les salons.
/// </summary>
/// <param name="message">Le message envoy�.</param>
/// <param name="client">Le joueur qui a re�u le message de son client.</param>
void Jeu::gererCode04(const std::string& message, Joueur* client)
{
	std::istringstream f(message);

	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond message global envoy� par le joueur.
		if (i == 1)
		{
			// On envoie le message aux joueurs.
			sendNotification(client, line);

			std::cout << "Le joueur " << client->getNom() << " a envoy� � tous les salons: " << std::endl << line << std::endl;
		}
	}
}

/// <summary>
/// G�re une message du client avec le code "05".
/// Message qui indique que le client vient de changer de direction.
/// </summary>
/// <param name="message">Le message envoy�.</param>
/// <param name="client">Le joueur qui a re�u le message de son client.</param>
void Jeu::gererCode05(const std::string& message, Joueur* client)
{
	std::istringstream f(message);

	std::pair<float, float> direction;
	std::string line;
	for (unsigned int i = 0; i < 3; i++)
	{
		std::getline(f, line, '\n');

		// La deuxi�me ligne correspond la nouvelle direction du joueur.
		if (i == 1)
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

			direction.first = x;
			direction.second = y;
		}
		// La troisi�me ligne correspond au temps en millisecondes quand le joueur a chang� de direction.
		else if (i == 2)
		{
			// On envoit la direction aux joueurs.
			long long int temps;
			std::istringstream h(line);
			h >> temps;
			sendDirection(client, direction, temps);

			// On la garde �galement pour repr�senter le jeu.
			client->setDirection(direction);
		}
	}
}

/// <summary>
/// G�re une message du client avec le code "99".
/// Message qui indique que le client vient de demander une fin de connexion.
/// </summary>
/// <param name="message">Le message envoy�.</param>
/// <param name="client">Le joueur qui a re�u le message de son client.</param>
void Jeu::gererCode99(Joueur* client)
{
	sendDeconnexion(client);
	client->fermerConnexion();
}

/// <summary>
/// Envoie un message de connexion aux autres clients qui indique qu'un joueur s'est connect�.
/// </summary>
/// <param name="client">Le client qui s'est connect�.</param>
void Jeu::sendConnexion(Joueur* client)
{
	std::stringstream toOtherPlayers;
	toOtherPlayers << "01" << "\n" << client->getNom() << "\n" << client->getSalon()->getTimeInMicroseconds() << '\r';

	client->getSalon()->broadcast(toOtherPlayers.str(), client);
}

/// <summary>
/// Envoie un message de connexion � un joueur sp�cifique.
/// </summary>
/// <param name="client">Le joueur auquel envoyer le message de connexion.</param>
/// <param name="connectedPlayer">Le joueur qui s'est connect�.</param>
void Jeu::sendConnexionTo(Joueur* client, const Joueur* connectedPlayer)
{
	std::stringstream toClient;
	toClient << "01" << "\n" << connectedPlayer->getNom() << "\n" << client->getSalon()->getTimeInMicroseconds() << '\r';

	client->sendToClient(toClient.str());
}

/// <summary>
/// Envoie un message de d�connexion aux autres clients qui indique qu'un joueur s'est d�connect�.
/// </summary>
/// <param name="client">Le client qui s'est d�connect�.</param>
void Jeu::sendDeconnexion(Joueur* client)
{
	std::stringstream toOtherPlayers;
	toOtherPlayers << "02" << "\n" << client->getNom() << "\n" << client->getSalon()->getTimeInMicroseconds() << '\r';

	client->getSalon()->broadcast(toOtherPlayers.str(), client);

	client->sendToClient("99\0");
}

/// <summary>
/// Envoie un message aux autres clients qui indique qu'un joueur a envoy� un message.
/// </summary>
/// <param name="clinet">Le client qui a envoy� le message.</param>
/// <param name="message">Le message envoy� par le client.</param>
void Jeu::sendMessage(Joueur* client, const std::string& message)
{
	std::stringstream toOtherPlayers;
	toOtherPlayers << "03" << "\n" << client->getNom() << "\n" << message << "\n" << client->getSalon()->getTimeInMicroseconds() << '\r';

	client->getSalon()->broadcast(toOtherPlayers.str(), client);
}

/// <summary>
/// Envoie un message aux autres clients de tous les serveurs qui indique qu'un joueur a envoy� un message global.
/// </summary>
/// <param name="client">Le client qui a envoy� le message global.</param>
/// <param name="message">Le message global envoy� par le client.</param>
void Jeu::sendNotification(Joueur* client, const std::string& message)
{
	std::stringstream toAllOtherPlayers;
	toAllOtherPlayers << "04" << "\n" << client->getNom() << "\n" << message << "\n" << client->getSalon()->getTimeInMicroseconds() << '\r';
	
	for (Salon* salon : tousLesSalons)
	{
		salon->broadcast(toAllOtherPlayers.str(), client);
	}
}

/// <summary>
/// Envoie un message aux autres clients qui indique qu'un joueur a chang� de direction.
/// </summary>
/// <param name="client">Le client qui a chang� de direction.</param>
/// <param name="direction">La nouvelle direction du client..</param>
/// <param name="whenHappened">Le moment o� le changement de direction a �t� effectu�.</param>
void Jeu::sendDirection(Joueur* client, const std::pair<float, float>& direction, const long long int whenHappened)
{
	std::stringstream toOtherPlayers;
	toOtherPlayers << "05" << "\n" << client->getNom() << "\n" << direction.first << ',' << direction.second << "\n" << whenHappened << "\n" << client->getSalon()->getTimeInMicroseconds() << '\r';

	client->getSalon()->broadcast(toOtherPlayers.str(), client);
}

/// <summary>
/// Envoie un message aux clients qui indique qu'un joueur a chang� de position.
/// </summary>
/// <param name="client">Le client qui a chang� de position.</param>
/// <param name="position">La nouvelle position du client..</param>
/// <param name="whenHappened">Le moment o� le changement de position a �t� effectu�.</param>
void Jeu::sendPosition(Joueur* client, const std::pair<float, float>& position, const long long int whenHappened)
{
	std::stringstream toEveryone;
	toEveryone << "06" << "\n" << client->getNom() << "\n" << position.first + (Joueur::LARGEUR_JOUEUR * 0.5f) << ',' << position.second + (Joueur::HAUTEUR_JOUEUR * 0.5f) << "\n" << whenHappened << "\n" << client->getSalon()->getTimeInMicroseconds() << '\r';

	client->getSalon()->broadcast(toEveryone.str());
}

/// <summary>
/// Envoie des messages au client pour initialiser son jeu niveau client.
/// </summary>
/// <param name="client">Le client initialiser.</param>
void Jeu::sendInitialisation(Joueur* client)
{
	std::stringstream toClient;
	toClient << "07" << "\n" << client->getNom() << "\n" << client->getSalon()->getTimeInMicroseconds() << '\r';

	client->getSalon()->envoyerTousLesJoueursConnectes(client);

	client->sendToClient(toClient.str());

	sendPosition(client, client->getPosition(), client->getSalon()->getTimeInMicroseconds());
}