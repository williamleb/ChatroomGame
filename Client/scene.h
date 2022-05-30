#pragma once

#include <SFML/Graphics.hpp>

/// <summary>
/// Représente une scène dans le jeu.
/// </summary>
namespace TP3Client
{
	class Scene
	{
	public:
		// Toutes les scènes du jeu.
		enum scenes { SORTIE, JEU, TITRE };

		virtual ~Scene() {}
		
		/// <summary>
		/// Joue la scène.
		/// </summary>
		/// <returns>La prochaine scène à jouer.</returns>
		virtual scenes run() = 0;
		

		/// <summary>
		/// Initialise la scène.
		/// Doit être appelé une fois avant de jouer la scène.
		/// </summary>
		/// <param name="window">La fenêtre sur laquelle s'affichera la scène.</param>
		/// <returns>True si l'initialisation s'est effectuée correctement; false sinon.</returns>
		virtual bool init(sf::RenderWindow * const window) = 0;		

		/// <summary>
		/// Gère toutes les commandes entrées dans la scène.
		/// </summary>
		virtual void getInputs() = 0;
		
		/// <summary>
		/// Met à jour la scène.
		/// </summary>
		virtual void update() = 0;
		
		/// <summary>
		/// Affiche la scène.
		/// </summary>
		virtual void draw() = 0;

	protected:
		sf::RenderWindow *mainWin;

		sf::Event event;
		
		/// <summary>
		/// À la fin de chaque scène, on indique à Game quelle nouvelle scène elle doit charger.
		/// </summary>
		Scene::scenes transitionVersScene;
		bool isRunning;
	};
}