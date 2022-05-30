#pragma once

#include "gameObject.h"

namespace TP3Client
{
	/// <summary>
	/// Repr�sente un objet qui peut bouger dans le jeu.
	/// </summary>
	/// <seealso cref="GameObject" />
	class Movable : public GameObject
	{
	public:
		/// <summary>
		/// Diff�rentes directions simplifi�es que peut avoir un movable.
		/// </summary>
		enum Direction
		{
			IDLE,
			UP,
			UP_LEFT,
			UP_RIGHT,
			LEFT,
			DOWN,
			DOWN_LEFT,
			DOWN_RIGHT,
			RIGHT
		};

	private:
		/// <summary>
		/// La vitesse est exprim�e en px/sec.
		/// </summary>
		float speed;

		sf::Vector2f direction;

	public:
		virtual ~Movable();
		Movable();
		virtual void update(const float deltaT);

		const sf::Vector2f& getDirection() const;
		float getSpeed() const;

		void setDirection(const Direction direction);
		void setDirection(const float angle);
		void setSpeed(const float speed);

		static sf::Vector2f convertDirection(Direction direction);

	protected:
		static const float directionVerificationThreshold;
		void setDirection(sf::Vector2f direction);

	private:
		// Constante qui repr�sente racine de 2 / 2
		static const float diagonal;
	};
}