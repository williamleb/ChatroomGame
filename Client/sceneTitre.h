#pragma once
#include "Scene.h"
#include "textbox.h"

namespace TP3Client
{	
	/// <summary>
	/// La sc�ne titre repr�sente la sc�ne o� l'utilisateur peut choisir son nom et
	/// l'adresse IP sur laquelle se connecter.
	/// </summary>
	/// <seealso cref="Scene" />
	class SceneTitre : public Scene
	{
	public:

		sf::Texture backgroundT;
		sf::Sprite background;

		// L'interface de la sc�ne
		Textbox TitreLabel;
		Textbox errLabel;
		Textbox nameLabel;
		Textbox name;
		Textbox IPLabel;
		Textbox IP;
		sf::Font font;

		std::vector<Textbox*> allSelectableTextboxs;

		// N�cessaire car on ne peut pas que le code ascii que Enter et Backspace ne soit jamais saisie.
		bool backspaceActif = false;
		bool enterActif = false;
		bool tabActif = false;

		bool errWasInit = false;

		// Sert � choisir le textbox actif.
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

		// Utilis�s pour communiquer avec la sc�ne de jeu.
		std::string chosenName;
		std::string chosenIP;
		const std::string& getChosenName() const;
		const std::string& getChosenIP() const;
		void setInfo(const std::string& erreur, const std::string& name);
	};
}
