#pragma once
#include "Scene.h"
#include "textbox.h"

namespace TP3Client
{	
	/// <summary>
	/// La scène titre représente la scène où l'utilisateur peut choisir son nom et
	/// l'adresse IP sur laquelle se connecter.
	/// </summary>
	/// <seealso cref="Scene" />
	class SceneTitre : public Scene
	{
	public:

		sf::Texture backgroundT;
		sf::Sprite background;

		// L'interface de la scène
		Textbox TitreLabel;
		Textbox errLabel;
		Textbox nameLabel;
		Textbox name;
		Textbox IPLabel;
		Textbox IP;
		sf::Font font;

		std::vector<Textbox*> allSelectableTextboxs;

		// Nécessaire car on ne peut pas que le code ascii que Enter et Backspace ne soit jamais saisie.
		bool backspaceActif = false;
		bool enterActif = false;
		bool tabActif = false;

		bool errWasInit = false;

		// Sert à choisir le textbox actif.
		Textbox* textboxActif = nullptr;

		int deltaT = 1000 / 60;
		int temps;

	public:
		SceneTitre();
		~SceneTitre();

		scenes run();
		bool init(sf::RenderWindow * const window);
		void getInputs();
		void update();
		void draw();

		// Utilisés pour communiquer avec la scène de jeu.
		std::string chosenName;
		std::string chosenIP;
		const std::string& getChosenName() const;
		const std::string& getChosenIP() const;
		void setInfo(const std::string& erreur, const std::string& name);
	};
}
