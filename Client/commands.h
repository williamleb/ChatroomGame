/*
	Auteur: William Lebel

	Fichier non utilisé.
*/
#pragma once
#include <SFML\Graphics.hpp>

namespace TP3Client
{
	class Other;
	
	/// <summary>
	/// La commande de base.
	/// </summary>
	class OtherCommand_Base
	{
	public:
		sf::Time whenHappened;
		
		OtherCommand_Base(const sf::Time& whenHappened);
		virtual void execute(Other* player) const = 0;
	};
	
	/// <summary>
	/// Une commande de changement de direction.
	/// </summary>
	/// <seealso cref="OtherCommand_Base" />
	class OtherCommand_ChangeDirection : public OtherCommand_Base
	{
		sf::Vector2f direction;

	public:
		OtherCommand_ChangeDirection(const sf::Vector2f& direction, const sf::Time& whenHappened);
		void execute(Other* player) const;

		const sf::Vector2f getDirection() const;
	};
	
	/// <summary>
	/// Une commande de changement de position.
	/// </summary>
	/// <seealso cref="OtherCommand_Base" />
	class OtherCommand_ChangePosition : public OtherCommand_Base
	{		
		sf::Vector2f position;

	public:
		OtherCommand_ChangePosition(const sf::Vector2f& direction, const sf::Time& whenHappened);
		void execute(Other* player) const;

		const sf::Vector2f getPosition() const;
	};
}