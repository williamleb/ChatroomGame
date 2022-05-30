#pragma once

#include<winsock2.h>
#include <string>

#pragma comment(lib,"ws2_32.lib")

namespace TP3Serveur
{
	class Salon;
	
	/// <summary>
	/// Le joueur représente la connexion d'un utilisateur.
	/// Chaque joueur gère sa logique et sa connexion avec son client.
	/// </summary>
	class Joueur
	{
		// Gestion de la connexion ----------------------------------------------------------------------
	private:
		SOCKET clientSocket;

		void init();
		void unload();

		bool hasUnload;
		bool initialised;

	public:
		void connect(SOCKET socketDescription);

		bool sendToClient(const std::string& message);

		void fermerConnexion();

		// Gestion de la logique du joueur --------------------------------------------------------------
	public:
		// Ces constantes doivent être égales à celles présentes dans la classe TP3Client::Square.
		static const int LARGEUR_JOUEUR = 100;
		static const int HAUTEUR_JOUEUR = 100;
		static const int VITESSE_JOUEUR = 250;

	private:
		std::pair<float, float> direction;
		std::pair<float, float> position;

		std::string nom;

		Salon* salon;

		void ajustementDansLeSalon();

	public:
		void update(const long long int deltaT);

		const std::string& getNom() const;
		void setNom(const std::string& nom);

		Salon* getSalon() const;

		const std::pair<float, float>& getPosition() const;
		const std::pair<float, float>& getDirection() const;

		void setPosition(const std::pair<float, float>& position);
		void setDirection(const std::pair<float, float>& direction);

		bool enCollisionAvec(const Joueur& other);

		bool aInitialise();
		void initialiser();
	};
}