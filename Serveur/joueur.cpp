#include "joueur.h"
#include "jeu.h"
#include <iostream>
#include "salon.h"
#include <thread>

using namespace TP3Serveur;

/// <summary>
/// G�re la connexion du joueur au serveur.
/// </summary>
/// <param name="socketDescription">Le descripteur de socket de la nouvelle connexion.</param>
void Joueur::connect(SOCKET socketDescription)
{
	hasUnload = false;
	initialised = false;

	init();

	clientSocket = socketDescription;

	int read_size;
	char client_message[2000];

	while ((read_size = recv(clientSocket, client_message, 2000, 0)) > 0)
	{
		if (read_size >= 0 && read_size < 2000)
		{
			client_message[read_size] = '\0';
		}

		// On envoie tous les message re�us au jeu pour qu'il les traite.
		Jeu::getInstance()->reveiveMessage(client_message, this);
	}

	std::cout << "D�connexion de " << nom << "." << std::endl;

	if (!hasUnload)
	{
		unload();
	}
}

/// <summary>
/// Ferme la connexion entre le client et le serveur.
/// </summary>
void Joueur::fermerConnexion()
{
	unload();
	hasUnload = true;
}

/// <summary>
/// Initialise le joueur.
/// </summary>
void Joueur::init()
{
	salon = Jeu::getInstance()->attacherAUnSalon(this);
}

/// <summary>
/// D�charge le joueur.
/// </summary>
void Joueur::unload()
{
	if (!hasUnload)
	{
		// On envoie sa d�connexion aux autres joueurs et � lui-m�me.
		Jeu::getInstance()->sendDeconnexion(this);

		// On le retire de son salon.
		salon->retirerJoueur(this);
		int compteur = 0;
		while (salon->estDansSalon(this))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// Finalement, on ferme la connexion.
		closesocket(clientSocket);
	}
	hasUnload = true;
}

/// <summary>
/// Envoie un message au client.
/// </summary>
/// <param name="message">Le message � envoyer au client.</param>
/// <returns></returns>
bool Joueur::sendToClient(const std::string& message)
{
	// On envoie le messsage au client.
	if (send(clientSocket, message.c_str(), (int)strlen(message.c_str()), 0) == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

/// <summary>
/// Met � jour le joueur dans son salon.
/// </summary>
/// <param name="deltaT">Le temps en microsecondes depuis le dernier appel de la m�thode update.</param>
void Joueur::update(const long long int deltaT)
{
	float seconds = deltaT / 1'000'000.0f;

	
	position.first = position.first + (direction.first * VITESSE_JOUEUR * seconds);
	position.second = position.second + (direction.second * VITESSE_JOUEUR * seconds);

	ajustementDansLeSalon();
}

/// <summary>
/// Retourne le nom du joueur.
/// </summary>
/// <returns>Le nom du joueur.</returns>
const std::string& Joueur::getNom() const
{
	return nom;
}

/// <summary>
/// Change le nom du joueur.
/// </summary>
/// <param name="nom">Le nouveau nom du joueur.</param>
void Joueur::setNom(const std::string& nom)
{
	this->nom = nom;
}

/// <summary>
/// Retourne le salon dans lequel le joueur est situ�.
/// </summary>
/// <returns></returns>
Salon* Joueur::getSalon() const
{
	return salon;
}

/// <summary>
/// Retourne la position du joueur dans son salon.
/// </summary>
/// <returns>La position du joueur.</returns>
const std::pair<float, float>& Joueur::getPosition() const
{
	return position;
}

/// <summary>
/// Retourne la direction du joueur.
/// </summary>
/// <returns>La direction du joueur.</returns>
const std::pair<float, float>& Joueur::getDirection() const
{
	return direction;
}

/// <summary>
/// Change la position du joueur dans son salon.
/// </summary>
/// <param name="position">La nouvelle position du joueur.</param>
void Joueur::setPosition(const std::pair<float, float>& position)
{
	this->position.first = position.first;
	this->position.second = position.second;
}

/// <summary>
/// Change la direction du joueur.
/// </summary>
/// <param name="direction">La nouvelle direction du joueur..</param>
void Joueur::setDirection(const std::pair<float, float>& direction)
{
	this->direction.first = direction.first;
	this->direction.second = direction.second;
}

/// <summary>
/// D�termine si le joueur est en collision avec un autre joueur.
/// </summary>
/// <param name="other">L'autre joueur avec lequel v�rifier la collision.</param>
/// <returns>true s'il y a collision; false sinon.</returns>
bool Joueur::enCollisionAvec(const Joueur& other)
{

	if ((other.position.first >= this->position.first + LARGEUR_JOUEUR) 
		|| (other.position.first + LARGEUR_JOUEUR <= this->position.first) 
		|| (other.position.second >= this->position.second + HAUTEUR_JOUEUR)
		|| (other.position.second + HAUTEUR_JOUEUR <= this->position.second))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/// <summary>
/// Ajuste le joueur pour qu'il reste � l'int�rieur du salon.
/// </summary>
void Joueur::ajustementDansLeSalon()
{
	// Limites horizontales.
	if (position.first < 0)
	{
		position.first = 0;
	}
	else if (position.first + LARGEUR_JOUEUR >= Salon::LARGEUR_SALON)
	{
		position.first = (Salon::LARGEUR_SALON - LARGEUR_JOUEUR - 1);
	}

	// Limites verticales.
	if (position.second < 0)
	{
		position.second = 0;
	}
	else if (position.second + HAUTEUR_JOUEUR >= Salon::HAUTEUR_SALON)
	{
		position.second = (Salon::HAUTEUR_SALON - HAUTEUR_JOUEUR - 1);
	}
}

/// <summary>
/// D�termine si le client a initialis� le joueur.
/// </summary>
/// <returns>true si le client a initialis� le joueur; false sinon.</returns>
bool Joueur::aInitialise()
{
	return initialised;
}

/// <summary>
/// Indique que le client a initialis� le joueur.
/// </summary>
void Joueur::initialiser()
{
	initialised = true;
}