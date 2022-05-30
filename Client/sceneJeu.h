#pragma once
#include "scene.h"
#include "gestionnaireDeCommunication.h"
#include "gameObject.h"
#include <list>
#include <map>
#include "chat.h"

namespace TP3Client
{
	class Joueur;

	class Other;
	
	/// <summary>
	/// La scène de jeu représente la scène où l'utilisateur peut joueur au jeu.
	/// </summary>
	/// <seealso cref="Scene" />
	class SceneJeu : public Scene
	{
	public:
		// Dimensions du jeu (pour l'instant, ce sont des valeurs temporaires).
		static const int GAME_WIDTH = 1280;
		static const int GAME_HEIGHT = 700;		

		// Position du coin supérieur droit de la fenêtre de jeu.
		static const int GAME_TOP = 0;
		static const int GAME_LEFT = 0;		

		SceneJeu();
		~SceneJeu();
		
		scenes run();		
		bool init(sf::RenderWindow* const window);		
		void getInputs();		
		void update();		
		void draw();

		void ajouterJoueur(const std::string& username);
		void enleverJoueur(const std::string& username);

		void parler(const std::string& username, const std::string& message);
		void notifier(const std::string& username, const std::string& message);

		void changerDirection(const std::string& username, const sf::Vector2f direction, const sf::Time whenHappened);
		void changerPosition(const std::string& username, const sf::Vector2f position, const sf::Time whenHappened);

		void mettreAJourTemps(const sf::Time& temps);
		void mettreAJoutNom(const std::string& name);

		void deconnexion();

		const sf::Time& getCurrentTime() const;
		const std::string getPlayerName() const;

		void changePlayerName(const std::string& newName);

		void removeObject(GameObject* object);

	private:	
		sf::Texture backgroundT;
		sf::Sprite background;

		std::string firstPlayerName;

		sf::Font font;

		sf::Time tempsDernierUpdate;
		sf::Time tempsSalon;

		sf::Clock timer;

		GestionnaireDeCommunication* client;

		Joueur* joueur;
		Chat* chat;
		std::list<GameObject*> allGameObjects;
		std::map<std::string, Other*> allOtherPlayers;
		std::list<GameObject*> objectsToRemove;

		bool enCollisionAvecAutresJoueurs(const Square* player);

		bool removeLock;
		bool objectLock;
	};
}