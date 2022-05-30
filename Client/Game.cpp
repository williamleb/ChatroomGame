#include "game.h"
#include "sceneJeu.h"
#include "sceneTitre.h"
#include "gestionnaireDeCommunication.h"
#include <iostream>

using namespace TP3Client;

/// <summary>
/// Crée une nouvelle <see cref="Game"/>.
/// </summary>
Game::Game()
{
	srand(time(NULL));
	mainWin.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32), "Sqroom", sf::Style::Close);
	mainWin.setVerticalSyncEnabled(true);
	sf::Image icon;
	if (icon.loadFromFile("Ressources\\Sprites\\icon.png"))
	{
		mainWin.setIcon(50, 50, icon.getPixelsPtr());
	}
	//mainWin.setFramerateLimit(60);
}

/// <summary>
/// Fait jouer la partie.
/// </summary>
/// <returns>La sortie du programme.</returns>
int Game::run()
{
	GestionnaireDeCommunication::getInstance();

	std::string name("Guest");
	std::string erreur("");

	Scene::scenes selecteurDeScene = Scene::scenes::TITRE;
	Scene::scenes sceneEnRetour;
	Scene* sceneActive = nullptr;

	while (true)
	{
		// Condition de sortie: la scène suivante est celle de sortie.
		if (selecteurDeScene == Scene::scenes::SORTIE)
		{
			mainWin.close();
			GestionnaireDeCommunication::deleteInstance();
			return EXIT_SUCCESS;
		}
		else
		{
			// Sélectionne la classe concrète de la scène choisie.
			SceneJeu* sceneJeu;
			SceneTitre* sceneTitre;
			switch (selecteurDeScene)
			{
			case Scene::JEU:
				sceneActive = new SceneJeu();
				sceneJeu = (SceneJeu*)sceneActive;
				sceneJeu->changePlayerName(name);
				break;
			case Scene::TITRE:
				sceneActive = new SceneTitre();
				sceneTitre = (SceneTitre*)sceneActive;
				sceneTitre->setInfo(erreur, name);
				break;
			}

			erreur = "";

			// On initialise la scène et on la fait jouer.
			if (sceneActive->init(&mainWin))
			{
				sceneEnRetour = sceneActive->run();
			}
			else
			{
				if (dynamic_cast<SceneJeu*>(sceneActive))
				{
					erreur = "Erreur de connexion au serveur.";
					sceneEnRetour = Scene::TITRE;
				}
				else
				{
					// Condition de sortie: la scène ne s'est pas initialisée correctement.
					GestionnaireDeCommunication::deleteInstance();
					delete sceneActive;
					return EXIT_FAILURE;
				}
			}
		}

		if (SceneTitre* scene = dynamic_cast<SceneTitre*>(sceneActive))
		{
			name = scene->getChosenName();
			GestionnaireDeCommunication::getInstance()->changeIP(scene->getChosenIP());
		}

		selecteurDeScene = sceneEnRetour;
		delete sceneActive;
		sceneActive = nullptr;
	}
}
