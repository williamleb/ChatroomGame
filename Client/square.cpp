#include "square.h"
#include "sceneJeu.h"

using namespace TP3Client;

/// <summary>
/// Cr�e un nouveau <see cref="Square"/>.
/// </summary>
Square::Square(const sf::Font& font)
	: shape(sf::Vector2f(SQUARE_WIDTH, SQUARE_HEIGHT)), bulle(font)
{
	setDimension(SQUARE_WIDTH, SQUARE_HEIGHT);
	setSpeed(SQUARE_DEFAULT_SPEED);

	// Carr� ---------------------------------------------------------------
	// Donne au carr� une couleur au hasard.
	sf::Color color(rand() % 255, rand() % 255, rand() % 255);
	shape.setFillColor(color);

	shape.setOutlineThickness(4);
	shape.setOutlineColor(sf::Color(color.r - 5, color.g - 5, color.b - 5));
	shape.setOrigin(SQUARE_WIDTH / 2, SQUARE_HEIGHT / 2);

	// Texte ---------------------------------------------------------------

	name.setFont(font);
	name.setFillColor(sf::Color::White);
	name.setOutlineThickness(1);
	name.setOutlineColor(sf::Color::Black);
	name.setString("Guest");
}

/// <summary>
/// D�truit le <see cref="Square"/>.
/// </summary>
Square::~Square()
{
}

/// <summary>
/// Affiche le carr� sur la fen�tre.
/// </summary>
/// <param name="window">La fen�tre sur laquelle afficher le carr�..</param>
void Square::draw(sf::RenderWindow& window)
{
	// Le carr�.
	shape.setPosition(getPosition());
	window.draw(shape);
	
	// Le nom.
	name.setPosition(getPosition() - (sf::Vector2f(name.getGlobalBounds().width, name.getGlobalBounds().height) * 0.5f));
	window.draw(name);

	// La bulle de parole.
	if (bulle.isActivated())
	{
		bulle.setPosition(getPosition() + sf::Vector2f(0, -getDimension().y));
		bulle.draw(window);
	}
}

/// <summary>
/// Update le carr� selon le temps �coul�.
/// </summary>
/// <param name="deltaT">Le nombre de secondes depuis le dernier appel de la m�thode.</param>
void Square::update(const float deltaT)
{
	Movable::update(deltaT);

	bulle.update(deltaT);

	adjustIntoMap();
}

/// <summary>
/// Ajuste la position du carr� pour qu'il reste toujours dans les limites du niveau.
/// </summary>
void Square::adjustIntoMap()
{
	sf::Vector2f pos = getPosition();
	sf::Vector2f halfSize = getDimension() * 0.5f;
	SceneJeu* game = getSceneJeu();

	// Limites horizontales.
	if (pos.x - halfSize.x < game->GAME_LEFT)
	{
		setPosition(game->GAME_LEFT + halfSize.x, pos.y);
	}
	else if (pos.x + halfSize.x >= game->GAME_LEFT + game->GAME_WIDTH)
	{
		setPosition(game->GAME_LEFT + game->GAME_WIDTH - halfSize.x - 1, pos.y);
	}

	pos = getPosition();

	// Limites vertivales.
	if (pos.y - halfSize.y < game->GAME_TOP)
	{
		setPosition(pos.x, game->GAME_TOP + halfSize.y);
	}
	else if (pos.y + halfSize.y >= game->GAME_TOP + game->GAME_HEIGHT)
	{
		setPosition(pos.x, game->GAME_TOP + game->GAME_HEIGHT - halfSize.y - 1);
	}
}

/// <summary>
/// Retourne le nom du joueur repr�sent� par le carr�.
/// </summary>
/// <returns>Le nom du joueur repr�sent� par le carr�.</returns>
const std::string Square::getName() const
{
	std::string nameString = name.getString();
	return nameString;
}

/// <summary>
/// Change le nom du joueur repr�sent� par le carr�.
/// </summary>
/// <param name="name">Le nom du joueur repr�sent� par le carr�.</param>
void Square::setName(const std::string& name)
{
	this->name.setString(name);
}

/// <summary>
/// Fait appara�tre une bulle de parole au dessus du carr�.
/// </summary>
/// <param name="text">Le texte dit par le carr�.</param>
void Square::parler(const std::string& text)
{
	std::string name = getName();
	std::string texteADire = text;

	// On coupe le texte en plusieurs �tages.
	for (unsigned int i = 0; i < texteADire.size(); i++)
	{
		// On coupe le texte � chaque 25 caract�res.
		if ((i + 1) % 25 == 0)
		{
			texteADire.insert(i, "\n");
		}
	}

	bulle.show(texteADire);
}