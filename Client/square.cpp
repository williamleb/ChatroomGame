#include "square.h"
#include "sceneJeu.h"

using namespace TP3Client;

/// <summary>
/// Crée un nouveau <see cref="Square"/>.
/// </summary>
Square::Square(const sf::Font& font)
	: shape(sf::Vector2f(SQUARE_WIDTH, SQUARE_HEIGHT)), bulle(font)
{
	setDimension(SQUARE_WIDTH, SQUARE_HEIGHT);
	setSpeed(SQUARE_DEFAULT_SPEED);

	// Carré ---------------------------------------------------------------
	// Donne au carré une couleur au hasard.
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
/// Détruit le <see cref="Square"/>.
/// </summary>
Square::~Square()
{
}

/// <summary>
/// Affiche le carré sur la fenêtre.
/// </summary>
/// <param name="window">La fenêtre sur laquelle afficher le carré..</param>
void Square::draw(sf::RenderWindow& window)
{
	// Le carré.
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
/// Update le carré selon le temps écoulé.
/// </summary>
/// <param name="deltaT">Le nombre de secondes depuis le dernier appel de la méthode.</param>
void Square::update(const float deltaT)
{
	Movable::update(deltaT);

	bulle.update(deltaT);

	adjustIntoMap();
}

/// <summary>
/// Ajuste la position du carré pour qu'il reste toujours dans les limites du niveau.
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
/// Retourne le nom du joueur représenté par le carré.
/// </summary>
/// <returns>Le nom du joueur représenté par le carré.</returns>
const std::string Square::getName() const
{
	std::string nameString = name.getString();
	return nameString;
}

/// <summary>
/// Change le nom du joueur représenté par le carré.
/// </summary>
/// <param name="name">Le nom du joueur représenté par le carré.</param>
void Square::setName(const std::string& name)
{
	this->name.setString(name);
}

/// <summary>
/// Fait apparaître une bulle de parole au dessus du carré.
/// </summary>
/// <param name="text">Le texte dit par le carré.</param>
void Square::parler(const std::string& text)
{
	std::string name = getName();
	std::string texteADire = text;

	// On coupe le texte en plusieurs étages.
	for (unsigned int i = 0; i < texteADire.size(); i++)
	{
		// On coupe le texte à chaque 25 caractères.
		if ((i + 1) % 25 == 0)
		{
			texteADire.insert(i, "\n");
		}
	}

	bulle.show(texteADire);
}