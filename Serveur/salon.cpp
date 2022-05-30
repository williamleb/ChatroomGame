#include "salon.h"
#include "joueur.h"
#include "jeu.h"
#include <thread>

using namespace TP3Serveur;

/// <summary>
/// Crée un nouveau <see cref="Salon"/>.
/// </summary>
Salon::Salon()
	:joueurLock(false)
{
	tempsDernierEnvoiPosition = 0;
	tempsSalon = 0;
}

/// <summary>
/// Retourne le nombre de joueurs dans le salon.
/// </summary>
/// <returns></returns>
size_t Salon::getNumberOfPlayer() const
{
	return tousLesJoueurs.size();
}

/// <summary>
/// Détermine si le salon est plein.
/// </summary>
/// <returns>true si le selon est plein; false sinon.</returns>
bool Salon::isFull() const
{
	return tousLesJoueurs.size() == SALON_MAX_JOUEURS;
}

/// <summary>
/// Détermine si le salon est vide.
/// </summary>
/// <returns>true si le selon est vide; false sinon.</returns>
bool Salon::isEmpty() const
{
	return tousLesJoueurs.empty();
}

/// <summary>
/// Met à jour le salon.
/// </summary>
/// <param name="deltaT">La différence de temps en microsecondes depuis la dernière mise à jour.</param>
void Salon::update(const long long int deltaT)
{
	tempsSalon += deltaT;

	while (joueurLock == true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	joueurLock = true;

	for (std::list<Joueur*>::iterator i = joueursARetirer.begin(); i != joueursARetirer.end(); ++i)
	{
		tousLesJoueurs.remove(*i);
		*i = nullptr;
	}
	joueursARetirer.clear();

	// Si ça fait assez longtemps, on envoie la position de tous les joueurs à tous les clients.
	if ((tempsSalon - tempsDernierEnvoiPosition) * 0.001f >= MS_POUR_ENVOYER_POSITION) // TODO: En ms
	{
		tempsDernierEnvoiPosition = tempsSalon;
		
		for (Joueur* joueur : tousLesJoueurs)
		{
			if (joueur != nullptr && joueur->aInitialise())
			{
				Jeu::getInstance()->sendPosition(joueur, joueur->getPosition(), tempsSalon);
			}
		}
	}

	// Met à jour tous les joueurs.
	for (Joueur* joueur : tousLesJoueurs)
	{
		std::pair<float, float> oldPos = joueur->getPosition();
		joueur->update(deltaT);
		if (enCollisionnAvecUnAutreJoueur(joueur))
		{
			joueur->setPosition(oldPos);
		}
	}

	joueurLock = false;
}

/// <summary>
/// Ajoute un joueur au salon.
/// </summary>
/// <param name="joueur">Le joueur à ajouter au salon.</param>
void Salon::ajouterJoueur(Joueur* joueur)
{
	// On détermine une position au hasard dans le selon (pour qu'il n'entre pas en collision avec les autres joueurs).
	do
	{
		int posX = rand() % (LARGEUR_SALON - Joueur::LARGEUR_JOUEUR);
		int posY = rand() % (HAUTEUR_SALON - Joueur::HAUTEUR_JOUEUR);
		std::pair<float, float> position(posX, posY);
		joueur->setPosition(position);
	} while (enCollisionnAvecUnAutreJoueur(joueur));

	tousLesJoueurs.push_back(joueur);
}

/// <summary>
/// Détermine si un joueur est en collision avec n'importe quel autre joueur du salon.
/// </summary>
/// <param name="joueur">Le joueur avec lequel déterminer s'il y a collision.</param>
/// <returns>true si le joueur est en collision avec un autre joueur; false sinon.</returns>
bool Salon::enCollisionnAvecUnAutreJoueur(const Joueur* joueur)
{
	// Si le joueur entre en collision avec n'importe quel joueur sauf lui, on retourne vrai.
	for (Joueur* other : tousLesJoueurs)
	{
		if (other != joueur)
		{
			if (other->enCollisionAvec(*joueur))
			{
				return true;
			}
		}
	}

	return false;
}

/// <summary>
/// Retire un joueur du salon.
/// </summary>
/// <param name="joueur">Le joueur à retirer.</param>
void Salon::retirerJoueur(Joueur* joueur)
{
	joueursARetirer.push_back(joueur);

	// On enlève le salon s'il est vide.
	//if (isEmpty())
	//{
	//	Jeu::getInstance()->retirerSalon(this);
	//}
}

/// <summary>
/// Envoie un message à tous les joueurs du salon sauf un.
/// </summary>
/// <param name="message">Le message à envoyer.</param>
/// <param name="joueurANePasEnvoyer">Le joueur à ne pas envoyer le message.</param>
void Salon::broadcast(const std::string& message, const Joueur* joueurANePasEnvoyer)
{

	// Pour tous les joueurs du salon...
	for (Joueur* joueur : tousLesJoueurs)
	{
		// Si ce n'est pas le joueur à ne pas notifier...
		if (joueur != joueurANePasEnvoyer)
		{
			// On envoie le message.
			if (!joueur->sendToClient(message))
			{
				retirerJoueur(joueur);
				Jeu::getInstance()->sendDeconnexion(joueur);
			}
		}
	}
}

/// <summary>
/// Retourne un nouveau nom non utilisé dans le salon.
/// </summary>
/// <returns>Un nouveau nom non utilisé dans le salon.</returns>
std::string Salon::getNouveauNom() const
{
	return ajusterNom("Joueur");
}

/// <summary>
/// Ajuste un nom pour qu'il soit correct.
/// </summary>
/// <param name="ancienNom">Le nom à ajuster.</param>
/// <returns>Le nom ajusté.</returns>
std::string Salon::ajusterNom(const std::string& ancienNom) const
{
	std::string nouveauNom = ancienNom;

	// Tant que le nom n'est pas correct, on l'ajuste.
	while (nouveauNom != ajusterNomPrivate(nouveauNom))
	{
		nouveauNom = ajusterNomPrivate(nouveauNom);
	}

	return nouveauNom;
}

/// <summary>
/// Ajuste un nom une fois.
/// </summary>
/// <param name="ancienNom">Le nom à ajuster.</param>
/// <returns>Le nom ajusté.</returns>
std::string Salon::ajusterNomPrivate(const std::string& ancienNom) const
{
	// Le nom doit faire un maximum de 10 caractères.
	std::string nouveauNom = ancienNom.substr(0, 10);

	// On parcourt tous les joueurs du salon pour s'assurer que les noms ne sont pas pareil.
	bool nomDejaPris = false;
	for (Joueur* joueur : tousLesJoueurs)
	{
		if (nouveauNom == joueur->getNom())
		{
			nomDejaPris = true;
		}
	}

	// Si le nom est déjà pris...
	if (nomDejaPris)
	{
		char newChar = (rand() % (126 - 33)) + 33;
		if (nouveauNom.length() == 10)
		{
			// S'il a la taille maximale, on remplace un caractère au hasard dans le nom.
			unsigned int position = rand() % 10;
			nouveauNom[position] = newChar;
		}
		else
		{
			// Sinon, on lui ajoute un caractère au hasard.
			nouveauNom.push_back(newChar);
		}
	}

	return nouveauNom;
}

/// <summary>
/// Retourne le temps du salon en microsecondes.
/// </summary>
/// <returns>Le temps du salon en microsecondes.</returns>
unsigned long long int Salon::getTimeInMicroseconds() const
{
	return tempsSalon;
}

/// <summary>
/// Envoie un message de connexion de tous les joueurs à un joueur spécifique.
/// </summary>
/// <param name="joueur">Le joueur à envoyer les messages de connexion.</param>
void Salon::envoyerTousLesJoueursConnectes(Joueur* joueur)
{
	while (joueurLock == true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	joueurLock = true;

	for (Joueur* other : tousLesJoueurs)
	{
		if (other != joueur)
		{
			Jeu::getInstance()->sendConnexionTo(joueur, other);
		}
	}

	joueurLock = false;
}

/// <summary>
/// Détermine si un joueur est dans le salon.
/// </summary>
/// <param name="joueur">Le joueur à vérifier s'il est dans le salon.</param>
/// <returns>true si le joueur est dans le salon; false sinon.</returns>
bool Salon::estDansSalon(const Joueur* joueur)
{
	while (joueurLock == true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	joueurLock = true;

	for (Joueur* other : tousLesJoueurs)
	{
		if (other == joueur)
		{
			joueurLock = false;
			return true;
		}
	}

	joueurLock = false;
	return false;
}

/// <summary>
/// Ferme la connexion des tous les joueurs du salon.
/// </summary>
void Salon::fermerToutesConnexions()
{
	while (joueurLock == true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	joueurLock = true;

	while (!tousLesJoueurs.empty())
	{
		joueurLock = false;
		tousLesJoueurs.back()->fermerConnexion();
		joueurLock = true;
	}

	joueurLock = false;
}