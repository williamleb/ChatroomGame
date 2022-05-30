#pragma once

#include "gestionnaireDeCommunication.h"
#include "client.h"
#include <SFML/System.hpp>
#include <string>

namespace TP3Client
{
	class SceneJeu;
	
	/// <summary>
	/// Le gestionnaire de communication sert de couche d'astraction entre le jeu et le serveur.
	/// </summary>
	class GestionnaireDeCommunication
	{
		// Gestion du singleton ----------------------------------------------
	private:
		static GestionnaireDeCommunication* instance;

		GestionnaireDeCommunication();
		GestionnaireDeCommunication(const GestionnaireDeCommunication&) = delete;
		GestionnaireDeCommunication& operator=(const GestionnaireDeCommunication&) = delete;
	public:
		~GestionnaireDeCommunication();

		static GestionnaireDeCommunication* getInstance();
		static void deleteInstance();

		// Gestion de la communication ---------------------------------------
	private:
		Client client;
		SceneJeu* sceneJeu;

	public:

		bool init();
		void run();
		void sync(SceneJeu* sceneJeu, const std::string& utilisateur);
		void sendMessage(const std::string& message);
		void sendDirection(const sf::Vector2f& direction);

		void receiveMessage(const std::string& message);

		void deconnection();
		
		void changeIP(const std::string& ip);

	private:

		void manageCode01(const std::string& message);
		void manageCode02(const std::string& message);
		void manageCode03(const std::string& message);
		void manageCode04(const std::string& message);
		void manageCode05(const std::string& message);
		void manageCode06(const std::string& message);
		void manageCode07(const std::string& message);
		void manageCode99();
	};
}