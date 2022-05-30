#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace TP3Client
{

	class SceneJeu;

	/// <summary>
	/// Représente un objet de jeu qui peut être dessiné.
	/// </summary>
	class GameObject
	{
		static SceneJeu* sceneJeu;

	public:
		static void setSceneJeu(SceneJeu* sceneJeu);

	private:
		sf::Vector2f position;
		sf::Vector2f dimension;
		sf::IntRect collisionRect;

	public:
		virtual ~GameObject();
		GameObject();
		virtual void draw(sf::RenderWindow& window) = 0;

		virtual bool collidesWith(const GameObject& other) const;
		bool contains(const sf::Vector2f& position) const;

		float getDistanceWith(const GameObject& other) const;
		float getDistanceWith(const sf::Vector2f& position) const;

		const sf::Vector2f& getPosition() const;
		const sf::Vector2f& getDimension() const;

		void setPosition(const sf::Vector2f& position);
		void setPosition(const float posX, const float posY);
		void setDimension(const sf::Vector2f& dimension);
		void setDimension(const float width, const float height);

	protected:
		static SceneJeu* getSceneJeu();
	private:
		void computeCollisionRect();
	};
}