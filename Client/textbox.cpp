/*
Fichier créé par François Paradis dans le cadre du cours Programmation Jeu Vidéo III
Modifications par William Lebel.
*/

#include "Textbox.h"
#include <iostream>

using namespace TP3Client;

Textbox::Textbox()
	:isLabel(false)
{
}

Textbox::~Textbox()
{
}

/// <summary>
/// Initiation d'un Textbox.  Le choix ne rien mettre dans le constructeur vient du fait
/// Qu'il est inutile d'initialiser quoi que ce soit tant qu'on a pas une font valide.
/// Où alors on serait obligé de l'utiliser comme pointeur.
/// La méthode init règle la question
/// Certains éléments sont délibéréments hardcodés; on ne veut pas une textbox hyper-flexible
/// Juste de quoi qui fera le travail pour ce TP
/// </summary>
/// <param name="longueur">Longueur du Textbox</param>
/// <param name="limite">Limite de caractères; devrait être 1 pour 20 pixels de longueur du textbox</param>
/// <param name="position">Position à l'écran.  Le Textbox est centrée sur cette position.</param>
/// <param name="font">La font utilisée</param>
/// <param name="next">Un pointeur sur le prochain textbox à sélectionner.</param>
void Textbox::init(const int longueur, const int limite, const Vector2f position, const Font& font, Textbox* next)
{
	int hauteur = 30;
	this->limite = limite;

	boite.setSize(Vector2f(longueur, hauteur));
	boite.setFillColor(sf::Color(15, 15, 15));
	boite.setOutlineColor(sf::Color::White);
	boite.setOutlineThickness(1);
	boite.setOrigin(Vector2f(longueur / 2, hauteur / 2));
	boite.setPosition(position);

	texte.setFont(font);
	texte.setCharacterSize(20);
	texte.setFillColor(Color::White);
	texte.setOutlineColor(Color::Black);
	texte.setOutlineThickness(1);

	this->next = next;

	isLabel = false;
}


/// <summary>
/// Voir précédent, mais utilisé comme Label
/// La boite est toujours présente (pas le choix), mais carrément invisible
///
/// Si le texte notifie une erreur, on le mettra de couleur rouge
/// </summary>
void Textbox::initInfo(const Vector2f position, const Font& font, const bool estErreur)
{
	int hauteur = 30;
	this->limite = 70;// pas de graphique associé, on peut mettre un long texte par défaut.

	boite.setSize(Vector2f(0, 0));
	boite.setFillColor(Color::Transparent);
	boite.setOutlineColor(Color::Transparent);
	boite.setOutlineThickness(0);
	boite.setOrigin(Vector2f(0, 0));
	boite.setPosition(position);

	texte.setFont(font);
	texte.setCharacterSize(20);

	if (estErreur)
	{
		texte.setFillColor(Color::Red);
	}
	else
	{
		texte.setFillColor(Color::White);
	}	

	isLabel = true;
	ajustementsVisuels();
}

/// <summary>
/// Fait juste changer la couleur du cadre.
/// </summary>
void Textbox::selectionner()
{
	boite.setOutlineColor(sf::Color(57, 131, 192));
}

void Textbox::deSelectionner()
{
	boite.setOutlineColor(Color::White);
}

/// <summary>
/// Vérifie si une position touche au textbox
/// </summary>
bool Textbox::touche(const Vector2i& position)
{
	FloatRect coord = boite.getGlobalBounds();

	if (position.x > coord.left && position.x < coord.left + coord.width &&
		position.y > coord.top && position.y < coord.top + coord.height)
	{
		return true;
	}

	return false;
}

String Textbox::getTexte() const
{
	return texte.getString();
}

/// <summary>
/// Le texte est inséré et remplace le précédent.
/// </summary>
/// <param name="leTexte">The le texte.</param>
void Textbox::insererTexte(std::string leTexte)
{
	//Si trop long on concatène
	if (leTexte.size() > limite)
	{
		leTexte = leTexte.substr(0, limite);
	}

	std::cout << leTexte << std::endl;
	texte.setString(leTexte);
	ajustementsVisuels();
}
/// <summary>
/// On ajoute un caractère à la string, si on est pas à la longueur maximale
/// </summary>
void Textbox::ajouterChar(const char leChar)
{
	if (texte.getString().getSize() >= limite) return;

	texte.setString(texte.getString() + leChar);
	ajustementsVisuels();
}

/// <summary>
/// On retire un caractère à la string, si on est pas à la longueur 0
/// </summary>
void Textbox::retirerChar()
{
	if (texte.getString().getSize() == 0) return;

	texte.setString(texte.getString().substring(0, texte.getString().getSize() - 1));
	ajustementsVisuels();
}

/// <summary>
/// Ajustements visuels: le texte est toujours centré sur la position de la boite.
/// </summary>
void Textbox::ajustementsVisuels()
{
	FloatRect coord = texte.getGlobalBounds();
	texte.setPosition(boite.getPosition() - boite.getSize() * 0.5f + sf::Vector2f(5, 5));
}

void Textbox::dessiner(RenderWindow* const window)
{
	window->draw(boite);

	if (isLabel)
	{
		sf::Vector2f pos = texte.getPosition();
		window->draw(texte);
	}
	else
	{
		std::string ancientTexte = texte.getString();

		std::string texteADessiner = ancientTexte;
		while (texte.getGlobalBounds().width > boite.getSize().x - 5)
		{
			texteADessiner.erase(0, 1);
			texte.setString(texteADessiner);
		}
		window->draw(texte);

		texte.setString(ancientTexte);
	}
}

Textbox* Textbox::getNext()
{
	return next;
}

void Textbox::setPosition(const Vector2f& position)
{
	boite.setPosition(position);
}