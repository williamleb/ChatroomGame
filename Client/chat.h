#pragma once

#include "gameObject.h"
#include "joueur.h"
#include "textbox.h"

namespace TP3Client
{	
	/// <summary>
	/// Le chat est l'endroit où sont affichés les messages des joueurs et avec lequel le joueur
	/// peut envoyer ses propres messages.
	/// </summary>
	/// <seealso cref="GameObject" />
	class Chat : public GameObject
	{
		static const int NBR_DE_TEXTES = 15;

		static const sf::Color messageColor;
		static const sf::Color messageOutlineColor;
		static const sf::Color notificationColor;
		static const sf::Color notificationOutlineColor;

		static const int CHAT_WIDTH = 500;
		static const int CHAT_HEIGHT = 300;
		static const int TEXTBOX_HEIGHT = 30;
		static const int TEXTBOX_WIDTH = 490;
		static const int CHAT_PADDING = 5;

		Joueur* joueur;

		Textbox gameTextBox;

		bool selectionne;

		sf::RectangleShape boiteDeDialogue;
		
		sf::Text allTexts[NBR_DE_TEXTES];
		unsigned int lastTextUsed;

	public:
		Chat(const sf::Font& font, Joueur* joueur);

		bool touche(const Vector2i& position);

		void getEvents(const sf::Event& event);

		void draw(sf::RenderWindow& window);

		void ajouterMessage(const std::string& player, const std::string& message);
		void ajouterNotification(const std::string& player, const std::string& message);

		void selectionner();
		void deSelectionner();
		bool estSelectionne();

	private:
		unsigned int getNextText();

		void ajustementsVisuels();
		std::string ajusterMessage(const std::string& message);
	};
}