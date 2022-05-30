#include "serveur.h"
#include <thread>
#include <cstdlib>
#include <iostream>
#include "joueur.h"
#include "jeu.h"

using namespace TP3Serveur;

/// <summary>
/// D�truit un <see cref="Serveur"/>.
/// </summary>
Serveur::~Serveur()
{
	unload();
	Jeu::deleteInstance();
}

/// <summary>
/// Initialise le serveur.
/// </summary>
/// <returns>true si le serveur s'est bien initialis�; false sinon.</returns>
bool Serveur::init()
{

	Jeu::getInstance();

	std::cout << "Initialisation du serveur..." << std::endl;

	std::cout << std::endl;

	// Inititialisation de la librairie Winsock.
	std::cout << "Inititialisation de la librairie Winsock..." << std::endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "L'initialisation de la librairie a �chou�." << std::endl << "Erreur: " <<  WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Librairie Winsock initialis�e." << std::endl;

	std::cout << std::endl;

	// Cr�ation du descripteur de socket.
	std::cout << "Cr�ation du descripteur de socket..." << std::endl;
	if ((socketDescription = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Le socket n'a pa pu �tre cr��." << std::endl << "Erreur: " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Le socket a �t� cr��." << std::endl;

	std::cout << std::endl;

	// Pr�paration des informations du serveur.
	std::cout << "Pr�paration des informations du serveur..." << std::endl;
	serveur.sin_family = AF_INET;
	serveur.sin_addr.s_addr = INADDR_ANY;
	serveur.sin_port = htons(PORT);
	std::cout << "Les information du serveur ont �t� pr�par�s." << std::endl;

	std::cout << std::endl;

	// Liaison du socket � son serveur.
	std::cout << "Liaison du socket � son serveur..." << std::endl;
	if (bind(socketDescription, (sockaddr*)&serveur, sizeof(serveur)) == SOCKET_ERROR)
	{
		std::cout << "La liaison a �chou�e." << std::endl << "Erreur: " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Le socket a �t� li� � son serveur." << std::endl;

	std::cout << std::endl;

	std::cout << "Le serveur s'est initialis� avec succ�s." << std::endl;

	return true;
}

/// <summary>
/// D�marre le serveur.
/// </summary>
/// <returns>Le code d'erreur du programme.</returns>
int Serveur::run()
{
	if (!init())
	{
		return EXIT_FAILURE;
	}

	std::cout << "--------------------------------------------------------------------------" << std::endl;

	// On lance l'ex�cution de la repr�sentation du jeu.
	Jeu* jeu = Jeu::getInstance();
	std::thread threadJeu(&Jeu::run, std::ref(*jeu));
	std::thread threadConsole(&Serveur::readConsole, this);

	// On �coute les connexions.
	listen(socketDescription, 3);
	std::cout << "�coute sur le port " << PORT << "." << std::endl << std::endl;

	// On accepte les diff�rentes connexions.
	int c = sizeof(sockaddr_in);
	while ((newSocket = accept(socketDescription, (sockaddr *)&client, &c)) != INVALID_SOCKET)
	{
		std::cout << "Connexion accept�e." << std::endl;

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
/// Lit les entr�es � la console.
/// </summary>
void Serveur::readConsole()
{
	std::string message;

	bool mustContinue = true;

	std::cout << "Entrez \"stop\" pour arr�ter le serveur." << std::endl;
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
/// D�charge le serveur.
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