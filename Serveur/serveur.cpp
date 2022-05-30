#include "serveur.h"
#include <thread>
#include <cstdlib>
#include <iostream>
#include "joueur.h"
#include "jeu.h"

using namespace TP3Serveur;

/// <summary>
/// Détruit un <see cref="Serveur"/>.
/// </summary>
Serveur::~Serveur()
{
	unload();
	Jeu::deleteInstance();
}

/// <summary>
/// Initialise le serveur.
/// </summary>
/// <returns>true si le serveur s'est bien initialisé; false sinon.</returns>
bool Serveur::init()
{

	Jeu::getInstance();

	std::cout << "Initialisation du serveur..." << std::endl;

	std::cout << std::endl;

	// Inititialisation de la librairie Winsock.
	std::cout << "Inititialisation de la librairie Winsock..." << std::endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "L'initialisation de la librairie a échoué." << std::endl << "Erreur: " <<  WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Librairie Winsock initialisée." << std::endl;

	std::cout << std::endl;

	// Création du descripteur de socket.
	std::cout << "Création du descripteur de socket..." << std::endl;
	if ((socketDescription = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Le socket n'a pa pu être créé." << std::endl << "Erreur: " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Le socket a été créé." << std::endl;

	std::cout << std::endl;

	// Préparation des informations du serveur.
	std::cout << "Préparation des informations du serveur..." << std::endl;
	serveur.sin_family = AF_INET;
	serveur.sin_addr.s_addr = INADDR_ANY;
	serveur.sin_port = htons(PORT);
	std::cout << "Les information du serveur ont été préparés." << std::endl;

	std::cout << std::endl;

	// Liaison du socket à son serveur.
	std::cout << "Liaison du socket à son serveur..." << std::endl;
	if (bind(socketDescription, (sockaddr*)&serveur, sizeof(serveur)) == SOCKET_ERROR)
	{
		std::cout << "La liaison a échouée." << std::endl << "Erreur: " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Le socket a été lié à son serveur." << std::endl;

	std::cout << std::endl;

	std::cout << "Le serveur s'est initialisé avec succès." << std::endl;

	return true;
}

/// <summary>
/// Démarre le serveur.
/// </summary>
/// <returns>Le code d'erreur du programme.</returns>
int Serveur::run()
{
	if (!init())
	{
		return EXIT_FAILURE;
	}

	std::cout << "--------------------------------------------------------------------------" << std::endl;

	// On lance l'exécution de la représentation du jeu.
	Jeu* jeu = Jeu::getInstance();
	std::thread threadJeu(&Jeu::run, std::ref(*jeu));
	std::thread threadConsole(&Serveur::readConsole, this);

	// On écoute les connexions.
	listen(socketDescription, 3);
	std::cout << "Écoute sur le port " << PORT << "." << std::endl << std::endl;

	// On accepte les différentes connexions.
	int c = sizeof(sockaddr_in);
	while ((newSocket = accept(socketDescription, (sockaddr *)&client, &c)) != INVALID_SOCKET)
	{
		std::cout << "Connexion acceptée." << std::endl;

		allPlayers.push(new Joueur());
		allConnexions.push(new std::thread(&Joueur::connect, *allPlayers.back(), newSocket));

	}

	Jeu::getInstance()->stop();
	threadJeu.join();
	threadConsole.join();

	unload();

	return EXIT_SUCCESS;
}

/// <summary>
/// Lit les entrées à la console.
/// </summary>
void Serveur::readConsole()
{
	std::string message;

	bool mustContinue = true;

	std::cout << "Entrez \"stop\" pour arrêter le serveur." << std::endl;
	while (mustContinue)
	{
		std::cin >> message;
		if (message == "stop")
		{
			mustContinue = false;
			closesocket(socketDescription);
		}
	}

}

/// <summary>
/// Décharge le serveur.
/// </summary>
void Serveur::unload()
{
	while (!allConnexions.empty())
	{
		allConnexions.front()->join();
		delete allConnexions.front();
		allConnexions.pop();
	}

	while (!allPlayers.empty())
	{
		delete allPlayers.front();
		allPlayers.pop();
	}

	closesocket(socketDescription);
	WSACleanup();
}