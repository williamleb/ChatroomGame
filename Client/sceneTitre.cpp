#include "SceneTitre.h"
#include "client.h"

using namespace TP3Client;

/// <summary>
/// Crée une nouvelle <see cref="SceneTitre" />.
/// </summary>
SceneTitre::SceneTitre()
	:temps(0)
{
}

/// <summary>
/// Détruit une <see cref="SceneTitre"/>.
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
/// Change certaines informations de la scène.
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
/// Joue la scène titre.
/// </summary>
/// <returns>
/// La prochaine scène à jouer.
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
/// Initialise la scène titre.
/// </summary>
/// <param name="window">La fenêtre sur laquelle afficher la scène.</param>
/// <returns>true si la scène s'est ien initialisée; false sinon.</returns>
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

	// Sélectionne le premier textbox.
	textboxActif = &name;
	name.selectionner();

	return true;
}

/// <summary>
/// Gère toutes les commandes entrées dans la scène.
/// </summary>
void SceneTitre::getInputs()
{
	while (mainWin->pollEvent(event))
	{
		//x sur la fenêtre
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

		// Si on a un événement de click de souris
		if (event.type == Event::MouseButtonPressed)
		{
			bool wasATextbox = false;
			// Pour toutes les textboxs où on peut entrer de l'information...
			for (std::vector<Textbox*>::iterator i = allSelectableTextboxs.begin(); i != allSelectableTextboxs.end(); i++)
			{
				//Si on touche à la textbox avec la souris
				if ((*i)->touche(Mouse::getPosition(*mainWin)))
				{
					textboxActif = (*i); //Ce textbox devient actif
					(*i)->selectionner();  //on l'affiche comme étant sélectionné
					errLabel.insererTexte(""); //on efface le message d'erreur.
					wasATextbox = true;
				}
				else
				{
					//Sinon, le textbox n'est pas actif.
					(*i)->deSelectionner();
				}
			}

			// Si le click de la souris n'était pas sur un textbox, aucun textbox n'est actif.
			if (!wasATextbox)
			{
				textboxActif = nullptr;
			}
		}

		//Un événement de touche de clavier AVEC un textobx actif
		if (event.type == Event::KeyPressed && textboxActif != nullptr)
		{
			if (event.key.code == Keyboard::Return)
			{
				enterActif = true; //Pour s'assurer que enter n'est pas saisie comme caractère

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
				backspaceActif = true;  //Pour s'assurer que backspace n'est pas saisie comme caractère
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

	// Dans tous les cas on netoie ces conditions après chaque boucle.
	enterActif = false;
	backspaceActif = false;
	tabActif = false;
}

/// <summary>
/// Met à jour la scène.
/// </summary>
void SceneTitre::update()
{
	temps += deltaT;
}

/// <summary>
/// Affiche la scène.
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