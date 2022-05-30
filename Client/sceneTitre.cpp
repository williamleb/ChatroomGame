#include "SceneTitre.h"
#include "client.h"

using namespace TP3Client;

/// <summary>
/// Cr�e une nouvelle <see cref="SceneTitre" />.
/// </summary>
SceneTitre::SceneTitre()
	:temps(0)
{
}

/// <summary>
/// D�truit une <see cref="SceneTitre"/>.
/// </summary>
SceneTitre::~SceneTitre()
{
}

/// <summary>
/// Retourne le nom choisi par l'utilisateur.
/// </summary>
/// <returns>Le nom choisi par l'utilisateur.</returns>
const std::string& SceneTitre::getChosenName() const
{
	return chosenName;
}

/// <summary>
/// L'IP choisie par l'utilisateur.
/// </summary>
/// <returns>LIP choisie par l'utilisateur.</returns>
const std::string& SceneTitre::getChosenIP() const
{
	return chosenIP;
}

/// <summary>
/// Change certaines informations de la sc�ne.
/// </summary>
/// <param name="erreur">L'erreur.</param>
/// <param name="name">Le nom de du joueur.</param>
void SceneTitre::setInfo(const std::string& erreur, const std::string& name)
{
	//errLabel.initInfo(Vector2f(650, 70), font, true);
	errLabel.insererTexte(erreur);
	this->name.insererTexte(name);
	errWasInit = true;
}

/// <summary>
/// Joue la sc�ne titre.
/// </summary>
/// <returns>
/// La prochaine sc�ne � jouer.
/// </returns>
Scene::scenes SceneTitre::run()
{
	while (isRunning)
	{
		getInputs();
		update();
		draw();
	}

	return transitionVersScene;
}

/// <summary>
/// Initialise la sc�ne titre.
/// </summary>
/// <param name="window">La fen�tre sur laquelle afficher la sc�ne.</param>
/// <returns>true si la sc�ne s'est ien initialis�e; false sinon.</returns>
bool SceneTitre::init(RenderWindow * const window)
{

	if (!backgroundT.loadFromFile("Ressources\\Sprites\\background.png"))
	{
		return false;
	}

	if (!font.loadFromFile("Ressources\\Fonts\\vcr.ttf"))
	{
		return false;
	}

	background.setTexture(backgroundT);

	// Initialise les textboxs
	TitreLabel.initInfo(Vector2f(200, 20), font, false);
	errLabel.initInfo(Vector2f(100, 70), font, true);// TODO: Fix this
	nameLabel.initInfo(Vector2f(120, 120), font, false);
	name.init(250, 10, Vector2f(250, 170), font, &IP);
	IPLabel.initInfo(Vector2f(120, 220), font, false);
	IP.init(250, 15, Vector2f(250, 270), font, &name);

	TitreLabel.insererTexte("Sqroom");
	nameLabel.insererTexte("Nom d'utilisateur");
	name.ajouterChar(' ');
	name.retirerChar();
	IPLabel.insererTexte("IP du serveur");
	IP.insererTexte(Client::getIP());

	chosenName = name.getTexte();
	chosenIP = IP.getTexte();

	allSelectableTextboxs.push_back(&name);
	allSelectableTextboxs.push_back(&IP);

	this->mainWin = window;
	isRunning = true;

	// S�lectionne le premier textbox.
	textboxActif = &name;
	name.selectionner();

	return true;
}

/// <summary>
/// G�re toutes les commandes entr�es dans la sc�ne.
/// </summary>
void SceneTitre::getInputs()
{
	while (mainWin->pollEvent(event))
	{
		//x sur la fen�tre
		if (event.type == Event::Closed)
		{
			isRunning = false;
			transitionVersScene = Scene::scenes::SORTIE;
		}
		// Escape
		if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
		{
			isRunning = false;
			transitionVersScene = Scene::scenes::SORTIE;
		}

		// Si on a un �v�nement de click de souris
		if (event.type == Event::MouseButtonPressed)
		{
			bool wasATextbox = false;
			// Pour toutes les textboxs o� on peut entrer de l'information...
			for (std::vector<Textbox*>::iterator i = allSelectableTextboxs.begin(); i != allSelectableTextboxs.end(); i++)
			{
				//Si on touche � la textbox avec la souris
				if ((*i)->touche(Mouse::getPosition(*mainWin)))
				{
					textboxActif = (*i); //Ce textbox devient actif
					(*i)->selectionner();  //on l'affiche comme �tant s�lectionn�
					errLabel.insererTexte(""); //on efface le message d'erreur.
					wasATextbox = true;
				}
				else
				{
					//Sinon, le textbox n'est pas actif.
					(*i)->deSelectionner();
				}
			}

			// Si le click de la souris n'�tait pas sur un textbox, aucun textbox n'est actif.
			if (!wasATextbox)
			{
				textboxActif = nullptr;
			}
		}

		//Un �v�nement de touche de clavier AVEC un textobx actif
		if (event.type == Event::KeyPressed && textboxActif != nullptr)
		{
			if (event.key.code == Keyboard::Return)
			{
				enterActif = true; //Pour s'assurer que enter n'est pas saisie comme caract�re

				if (temps > 2000)
				{
					if (name.getTexte() != "" && IP.getTexte() != "")
					{
						chosenName = name.getTexte();
						chosenIP = IP.getTexte();
						isRunning = false;
						transitionVersScene = scenes::JEU;
					}
					else
					{
						errLabel.insererTexte("Vous devez entrer un nom et une adresse IP pour vous connecter.");
					}
				}
				else
				{
					errLabel.insererTexte("Vous devez attendre 2 secondes avant de vous reconnecter.");
				}
			}
			else if (event.key.code == Keyboard::BackSpace)
			{
				textboxActif->retirerChar();
				backspaceActif = true;  //Pour s'assurer que backspace n'est pas saisie comme caract�re
			}
			else if (event.key.code == Keyboard::Tab && textboxActif->getNext() != nullptr)
			{
				// Si l'utilisateur a appuyer sur tab, on change le textbox actif pour le suivant.
				textboxActif->deSelectionner();
				textboxActif = textboxActif->getNext();
				textboxActif->selectionner();

				tabActif = true;
			}
		}

		if (!tabActif && !backspaceActif && !enterActif && textboxActif != nullptr && (event.type == Event::TextEntered))
		{
			if (event.text.unicode < 255 && event.text.unicode > 33) 
			{
				textboxActif->ajouterChar((char)event.text.unicode);
			}
		}
	}

	// Dans tous les cas on netoie ces conditions apr�s chaque boucle.
	enterActif = false;
	backspaceActif = false;
	tabActif = false;
}

/// <summary>
/// Met � jour la sc�ne.
/// </summary>
void SceneTitre::update()
{
	temps += deltaT;
}

/// <summary>
/// Affiche la sc�ne.
/// </summary>
void SceneTitre::draw()
{
	mainWin->clear();
	mainWin->draw(background);
	TitreLabel.dessiner(mainWin);
	errLabel.dessiner(mainWin);
	nameLabel.dessiner(mainWin);
	name.dessiner(mainWin);
	IPLabel.dessiner(mainWin);
	IP.dessiner(mainWin);
	mainWin->display();
}