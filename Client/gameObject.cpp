#include "gameObject.h"

#include <cmath>

using namespace TP3Client;

/// <summary>
/// Représente la scène de jeu qui est présentement jouée.
/// </summary>
SceneJeu* GameObject::sceneJeu = nullptr;

/// <summary>
/// Change la scène de jeu qui est présentement jouée pour tous les objets.
/// </summary>
/// <param name="sceneJeu">La scène de jeu présentement jouée.</param>
void GameObject::setSceneJeu(SceneJeu* sceneJeu)
{
	GameObject::sceneJeu = sceneJeu;
}

/// <summary>
/// Retourne la scène de jeu présentement jouée.
/// </summary>
/// <returns>La scène de jeu présentement jouée.</returns>
SceneJeu* GameObject::getSceneJeu()
{
	return GameObject::sceneJeu;
}

/// <summary>
/// Crée un nouveau <see cref="GameObject"/>.
/// </summary>
GameObject::GameObject()
	:position(), dimension(), collisionRect()
{
}

/// <summary>
/// Détruit le <see cref="GameObject"/>.
/// </summary>
GameObject::~GameObject()
{

}

/// <summary>
/// Vérifie la collision entre deux objets.
/// La dimension est la position de l'objet doivent être exactes pour que cette méthode
/// fonctionne comme espéré.
/// </summary>
/// <param name="other">L'objet avec lequel vérifier la colision.</param>
/// <returns>True si les deux objets sont en collision; false sinon.</returns>
bool GameObject::collidesWith(const GameObject& other) const
{
	return collisionRect.intersects(other.collisionRect);
}

/// <summary>
/// Détermine si l'objet contient une position spécifique.
/// La dimension est la position de l'objet doivent être exactes pour que cette méthode
/// fonctionne comme espéré.
/// </summary>
/// <param name="position">La position avec laquelle vérifier la collision.</param>
/// <returns>True si l'objet contient la position; false sinon.</returns>
bool GameObject::contains(const sf::Vector2f& position) const
{
	return collisionRect.contains(position.x, position.y);
}

/// <summary>
/// Calcule la distance entre deux objets.
/// </summary>
/// <param name="other">L'objet avec lequel vérifier la distance.</param>
/// <returns>La distance entre les deux objets.</returns>
float GameObject::getDistanceWith(const GameObject& other) const
{
	return getDistanceWith(other.getPosition());
}

/// <summary>
/// Calcule la distance entre l'objet et une position.
/// </summary>
/// <param name="position">La position avec laquelle calculer la distance.</param>
/// <returns>La distance entre l'objet et la position.</returns>
float GameObject::getDistanceWith(const sf::Vector2f& position) const
{
	float xSqrdVal = (this->position.x - position.x) * (this->position.x - position.x);
	float ySqrdVal = (this->position.y - position.y) * (this->position.y - position.y);

	return sqrt(xSqrdVal + ySqrdVal);
}

/// <summary>
/// Retourne la position du centre de l'objet.
/// </summary>
/// <returns>La position du centre de l'objet.</returns>
const sf::Vector2f& GameObject::getPosition() const
{
	return position;
}

/// <summary>
/// Retourne les dimensions de l'objet.
/// </summary>
/// <returns>Les dimensions de l'objet.</returns>
const sf::Vector2f& GameObject::getDimension() const
{
	return dimension;
}

/// <summary>
/// Change la position du centre de l'objet.
/// </summary>
/// <param name="position">La position du centre de l'objet.</param>
void GameObject::setPosition(const sf::Vector2f& position)
{
	this->position = position;
	computeCollisionRect();
}

/// <summary>
/// Change la position du centre de l'objet.
/// </summary>
/// <param name="posX">La position en x du centre de l'objet.</param>
/// <param name="posY">La position en y du centre de l'objet.</param>
void GameObject::setPosition(const float posX, const float posY)
{
	position.x = posX;
	position.y = posY;
	computeCollisionRect();
}

/// <summary>
/// Change les dimensions de l'objet.
/// </summary>
/// <param name="dimension">Les dimensions de l'objet.</param>
void GameObject::setDimension(const sf::Vector2f& dimension)
{
	this->dimension = dimension;
	computeCollisionRect();
}

/// <summary>
/// Change les dimensions de l'objet.
/// </summary>
/// <param name="width">La largeur de l'objet.</param>
/// <param name="height">La hauteur de l'objet.</param>
void GameObject::setDimension(const float width, const float height)
{
	dimension.x = width;
	dimension.y = height;
	computeCollisionRect();
}

/// <summary>
/// Change le retangle de collision dépendamment des attributs de l'objet.
/// </summary>
void GameObject::computeCollisionRect()
{
	collisionRect.left = position.x - (dimension.x / 2);
	collisionRect.top = position.y - (dimension.y / 2);
	collisionRect.width = dimension.x;
	collisionRect.height = dimension.y;
}