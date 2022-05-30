#include "bulleParole.h"

using namespace TP3Client;

/// <summary>
/// Cr�e une nouvelle <see cref="BulleParole"/>.
/// </summary>
/// <param name="font">La police utilis�e par la bulle.</param>
BulleParole::BulleParole(const sf::Font& font)
	: activated(false), innerTimer(sf::microseconds(0))
{
	// Bulle ----------------------------------------------------------------
	// Donne au carr� une couleur au hasard.
	shape.setFillColor(sf::Color::White);

	shape.setOutlineThickness(2);
	shape.setOutlineColor(sf::Color(230, 230, 230));

	// Texte ---------------------------------------------------------------
	texte.setFont(font);
	texte.setFillColor(sf::Color::Black);
	texte.setString("");
	texte.setCharacterSize(16);
}

/// <summary>
/// Met � jour la bulle de parole.
/// </summary>
/// <param name="deltaT">Le temps en secondes depuis le dernier appel de update.</param>
void BulleParole::update(const float deltaT)
{
	if (activated)
	{
		innerTimer += sf::seconds(deltaT);

		if (innerTimer.asMilliseconds() >= MS_LIFETIME)
		{
			activated = false;
		}

		// Fait dispara�tre peu � peu la bulle.
		float msRestantes = MS_LIFETIME - innerTimer.asMilliseconds();
		if (msRestantes <= 2000)
		{
			sf::Uint8 transparence = (sf::Uint8)(((msRestantes / 2000.0f)) * 255.0f);

			sf::Color color = shape.getFillColor();
			color.a = transparence;
			shape.setFillColor(color);

			color = shape.getOutlineColor();
			color.a = transparence;
			shape.setOutlineColor(color);

			color = texte.getFillColor();
			color.a = transparence;
			texte.setFillColor(color);
		}
	}
}

/// <summary>
/// D�termine si la bulle est pr�sentement activ�e ou non.
/// </summary>
/// <returns>true si la bulle est pr�sentement activ�e; false sinon. </returns>
bool BulleParole::isActivated() const
{
	return activated;
}

/// <summary>
/// Affiche la bulle avec un nouveau texte.
/// </summary>
/// <param name="text">Le texte qui appara�t dans la bulle.</param>
void BulleParole::show(const std::string& text)
{
	texte.setString(text);

	setDimension(texte.getGlobalBounds().width + 20, texte.getGlobalBounds().height + 20);
	shape.setSize(getDimension());

	innerTimer = sf::microseconds(0);

	activated = true;

	// Remet les valeurs de transparence � la normale.
	sf::Color color = shape.getFillColor();
	color.a = 255;
	shape.setFillColor(color);

	color = shape.getOutlineColor();
	color.a = 255;
	shape.setOutlineColor(color);

	color = texte.getFillColor();
	color.a = 255;
	texte.setFillColor(color);
}

/// <summary>
/// Retourne le texte contenu dans la bulle.
/// </summary>
/// <returns>Le texte contenu dans la bulle.</returns>
const std::string& BulleParole::getText() const
{
	return texte.getString();
}

/// <summary>
/// Affiche la bulle de parole � l'�cran.
/// </summary>
/// <param name="window">La fen�tre sur laquelle afficher la bulle.</param>
void BulleParole::draw(sf::RenderWindow& window)
{
	if (activated)
	{
		// Le carr�.
		shape.setPosition(getPosition() - getDimension() * 0.5f);
		window.draw(shape);
		// Le nom.
		texte.setPosition(getPosition() - (sf::Vector2f(texte.getGlobalBounds().width, texte.getGlobalBounds().height) * 0.5f));
		window.draw(texte);
	}
}