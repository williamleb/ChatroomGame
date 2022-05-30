#include "chat.h"
#include "gestionnaireDeCommunication.h"
#include <cmath>
#include <sstream>
#include <climits>

using namespace TP3Client;

// Les couleurs des textes.
const sf::Color Chat::messageColor = sf::Color::White;
const sf::Color Chat::messageOutlineColor = sf::Color::Black;
const sf::Color Chat::notificationColor = sf::Color(57, 131, 192);
const sf::Color Chat::notificationOutlineColor = sf::Color::Black;

/// <summary>
/// Crée un nouveau <see cref="Chat"/>.
/// </summary>
/// <param name="font">La police utilisée par les textes du chat.</param>
/// <param name="joueur">Le joueur qui utilise le chat.</param>
Chat::Chat(const sf::Font& font, Joueur* joueur)
	:joueur(joueur), selectionne(false), lastTextUsed(0)
{
	gameTextBox.init(TEXTBOX_WIDTH, 255, getPosition(), font);

	// Initialisation des textes.
	for (int i = 0; i < NBR_DE_TEXTES; i++)
	{
		allTexts[i].setPosition(0, 0);
		sf::Vector2f pos = allTexts[i].getPosition();
		allTexts[i].setFont(font);
		allTexts[i].setOutlineThickness(1);
		allTexts[i].setString("");
		allTexts[i].setCharacterSize(14);
	}

	setDimension(CHAT_WIDTH, CHAT_HEIGHT);

	boiteDeDialogue.setFillColor(sf::Color(41, 20, 19, 150));
	boiteDeDialogue.setSize(getDimension());
}

/// <summary>
/// Détermine si une position touche au chat.
/// </summary>
/// <param name="position">La position à vérifier.</param>
/// <returns>True si la position touche au chat; false sinon.</returns>
bool Chat::touche(const Vector2i& position)
{
	bool estTouche = gameTextBox.touche(position);

	return estTouche;
}

/// <summary>
/// Sélectionne le chat.
/// </summary>
void Chat::selectionner()
{
	selectionne = true;
	gameTextBox.selectionner();
}

/// <summary>
/// Déselectionne le chat.
/// </summary>
void Chat::deSelectionner()
{
	selectionne = false;
	gameTextBox.deSelectionner();
}

/// <summary>
/// Détermine si le chat est selectionné.
/// </summary>
/// <returns>true si le chat est sélectionné; false sinon.</returns>
bool Chat::estSelectionne()
{
	return selectionne;
}

/// <summary>
/// Gère les événements qui ont rapport au chat.
/// </summary>
/// <param name="event">L'événement à gérer.</param>
void Chat::getEvents(const sf::Event& event)
{
	bool backSpaceActif = false;;
	bool enterActif = false;
	bool tabActif = false;

	if (event.type == Event::KeyPressed)
	{
		// Déplacement du textbox (pour le débogage, c'est pourquoi il est en commentaire).
		//if (event.key.code == Keyboard::Left)
		//{
		//	setPosition(getPosition() + sf::Vector2f(-10, 0));
		//}
		//if (event.key.code == Keyboard::Up)
		//{
		//	setPosition(getPosition() + sf::Vector2f(0, -10));
		//}
		//if (event.key.code == Keyboard::Down)
		//{
		//	setPosition(getPosition() + sf::Vector2f(0, 10));
		//}
		//if (event.key.code == Keyboard::Right)
		//{
		//	setPosition(getPosition() + sf::Vector2f(10, 0));
		//}

		// Si le joueur appuie sur enter...
		if (event.key.code == Keyboard::Return)
		{
			enterActif = true;
			
			std::string texte(gameTextBox.getTexte());

			while (texte.find("\r") != std::string::npos)
			{
				texte.erase(texte.find("\r"), 1);
			}
			while (texte.find("\n") != std::string::npos)
			{
				texte.erase(texte.find("\n"), 1);
			}

			// S'il y a du texte.
			if (texte != "")
			{

				// On envoie le message au serveur.
				GestionnaireDeCommunication::getInstance()->sendMessage(texte);
				
				// Si c'est un message global (il commence par /g)...
				if ((texte[0] == '\\' || texte[0] == '\/') && tolower(texte[1]) == 'g')
				{
					std::string newMessage = texte;

					// On enlève le /g et le premier espace, s'il y en a un, du message général.
					if (texte[2] == ' ')
					{
						newMessage.erase(0, 3);
					}
					else
					{
						newMessage.erase(0, 2);
					}

					// On l'ajoute en tant que notification;
					ajouterNotification(joueur->getName(), newMessage);
				}
				// Sinon, si c'est un message pour le salon...
				else
				{
					// Il parle.
					joueur->parler(texte);

					// On l'ajoute aux message.
					ajouterMessage(joueur->getName(), texte);
				}

				// Puis on efface le textbox.
				while (gameTextBox.getTexte() != "")
				{
					gameTextBox.retirerChar();
				}
			}
		}

		// Si on appuie sur Tab, le chat de déselectionne.
		if (event.key.code == Keyboard::Tab)
		{
			deSelectionner();
			tabActif = true;
		}

		if (event.key.code == Keyboard::BackSpace)
		{
			gameTextBox.retirerChar();
			backSpaceActif = true;  //Pour s'assurer que backspace n'est pas saisie comme caractère
		}
	}

	if (!backSpaceActif && !enterActif && !tabActif && (event.type == Event::TextEntered))
	{
		if (event.text.unicode < 255 && event.text.unicode >= 32)
		{
			gameTextBox.ajouterChar((char)event.text.unicode);
		}
	}
}

/// <summary>
/// Affiche le chat sur la fenêtre.
/// </summary>
/// <param name="window">La fenêtre sur laquelle afficher le chat.</param>
void Chat::draw(sf::RenderWindow& window)
{
	// Le fond.
	boiteDeDialogue.setPosition(getPosition());
	window.draw(boiteDeDialogue);
	
	// Tous les messages.
	for (unsigned int i = 0; i < NBR_DE_TEXTES; i++)
	{
		sf::Vector2f oldPosition = allTexts[i].getPosition();
		if (oldPosition.y >= 30)
		{

			sf::Vector2f newPosition = oldPosition + getPosition() - sf::Vector2f(0, TEXTBOX_HEIGHT);

			allTexts[i].setPosition(newPosition);

			window.draw(allTexts[i]);

			allTexts[i].setPosition(oldPosition);
		}

	}

	// Le textbox.
	gameTextBox.setPosition(getPosition() + sf::Vector2f(CHAT_PADDING, CHAT_HEIGHT - CHAT_PADDING - TEXTBOX_HEIGHT) + sf::Vector2f(TEXTBOX_WIDTH, TEXTBOX_HEIGHT) * 0.5f);
	gameTextBox.dessiner(&window);
}

/// <summary>
/// Ajoute un message au chat.
/// </summary>
/// <param name="player">Le nom du joueur qui a envoyé le message.</param>
/// <param name="message">Le message envoyé par le joueur.</param>
void Chat::ajouterMessage(const std::string& player, const std::string& message)
{
	// On sélectionne le texte à manipuler.
	unsigned int newText = getNextText();

	// On change la couleur du texte.
	allTexts[newText].setOutlineColor(messageColor);
	allTexts[newText].setFillColor(messageOutlineColor);

	// On construit le message à afficher.
	std::stringstream	AAfficher;
	AAfficher << player << ": " << message;

	// On change le visuel du texte.
	allTexts[newText].setStyle(sf::Text::Regular);
	allTexts[newText].setString(ajusterMessage(AAfficher.str()));

	// On ajuste le chat.
	ajustementsVisuels();
}

/// <summary>
/// Ajoute une notification au chat.
/// </summary>
/// <param name="player">Le joueur qui a envoyé la notification.</param>
/// <param name="message">La notification envoyée par le joueur.</param>
void Chat::ajouterNotification(const std::string& player, const std::string& message)
{
	// On sélectionne le texte à manipuler.
	unsigned int newText = getNextText();

	// On change la couleur du texte.
	allTexts[newText].setOutlineColor(notificationColor);
	allTexts[newText].setFillColor(notificationOutlineColor);

	// On construit le message à afficher.
	std::stringstream	AAfficher;
	AAfficher << player << ": " << message;

	// On change le visuel du texte.
	allTexts[newText].setStyle(sf::Text::Bold);
	allTexts[newText].setString(ajusterMessage(AAfficher.str()));

	// On ajuste le chat.
	ajustementsVisuels();
}

/// <summary>
/// Retourne le prochain attribut sf::Text qui sera affiché.
/// </summary>
/// <returns>Le prochain attribut sf::Text qui sera affiché.<returns>
unsigned int Chat::getNextText()
{
	lastTextUsed = (lastTextUsed + 1) % NBR_DE_TEXTES;
	return lastTextUsed;
}

/// <summary>
/// Ajuste visuellement le chat.
/// </summary>
void Chat::ajustementsVisuels()
{	
	sf::Vector2f newMessageDimension(allTexts[lastTextUsed].getGlobalBounds().width, allTexts[lastTextUsed].getGlobalBounds().height);
	int width = allTexts[lastTextUsed].getGlobalBounds().width;
	int height = allTexts[lastTextUsed].getGlobalBounds().height;

	sf::Vector2f position;

	// Pour tous les textes, on les montent pour que le message soit en bas.
	for (unsigned int i = 0; i < NBR_DE_TEXTES; i++)
	{
		position = allTexts[i].getPosition();
		allTexts[i].setPosition(allTexts[i].getPosition() - sf::Vector2f(0, newMessageDimension.y + CHAT_PADDING));
		position = allTexts[i].getPosition();
	}

	// On met le nouveau texte en bas.
	allTexts[lastTextUsed].setPosition(CHAT_PADDING, CHAT_HEIGHT - TEXTBOX_HEIGHT - newMessageDimension.y);
	position = allTexts[lastTextUsed].getPosition();

}

/// <summary>
/// Ajuste un message pour qu'il soit conforme à ce qui rentre dans le chat.
/// </summary>
/// <param name="message">Le message à ajuster.</param>
/// <returns>Le message ajusté.</returns>
std::string Chat::ajusterMessage(const std::string& message)
{
	std::string newMessage = message;

	// On coupe le texte en plusieurs étages.
	for (unsigned int i = 0; i < newMessage.size(); i++)
	{
		// On coupe le texte à chaque 55 caractères.
		if ((i + 1) % 55 == 0)
		{
			newMessage.insert(i, "\n");
		}
	}

	return newMessage;
}