#pragma once

#include <list>
#include <chrono>

namespace TP3Serveur
{
	class Salon;

	class Joueur;
	
	/// <summary>
	/// La représentation du jeu côté serveur.
	/// Elle fait la communication entre les connexions et les salons.
	/// </summary>
	class Jeu
	{
		// Gestion du singleton -----------------------------------------------------------------------
	private:
		static Jeu* instance;

		Jeu();

		Jeu(const Jeu&) = delete;
		Jeu& operator=(const Jeu&) = delete;

	public:
		~Jeu();

		static Jeu* getInstance();
		static void deleteInstance();

		// Gestion du jeu -----------------------------------------------------------------------------
		std::list<Salon*> tousLesSalons;

		bool isRunning;

		std::chrono::steady_clock::time_point tempsDernierUpdate;

	public:
		void run();
		void stop();

		void update();

		void retirerSalon(Salon* salon);

		// Gestion de la communication ----------------------------------------------------------------
	public:
		Salon* attacherAUnSalon(Joueur* joueur);

		void reveiveMessage(const std::string& message, Joueur* client);

	private:
		void gererCode01(const std::string& message, Joueur* client);
		void gererCode03(const std::string& message, Joueur* client);
		void gererCode04(const std::string& message, Joueur* client);
		void gererCode05(const std::string& message, Joueur* client);
		void gererCode99(Joueur* client);

	public:		
		void sendConnexion(Joueur* client);		
		void sendConnexionTo(Joueur* client, const Joueur* connectedPlayer);		
		void sendDeconnexion(Joueur* client);		
		void sendMessage(Joueur* clinet, const std::string& message);		
		void sendNotification(Joueur* client, const std::string& message);		
		void sendDirection(Joueur* client, const std::pair<float, float>& direction, const long long int whenHappened);
		void sendPosition(Joueur* client, const std::pair<float, float>& position, const long long int whenHappened);		
		void sendInitialisation(Joueur* client);
	};
}