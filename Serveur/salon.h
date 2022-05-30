#pragma once

#include <list>

namespace TP3Serveur
{
	class Joueur;
	
	/// <summary>
	/// Un salon peut accueilir un certain nombre de joueurs.
	/// Il sait comment faire une mise-à-jour logique.
	/// </summary>
	class Salon
	{
	public:
		// Ces constantes doivent être égales à celles présentes dans la classe TP3Client::SceneJeu.
		static const int LARGEUR_SALON = 1280;
		static const int HAUTEUR_SALON = 700;

	private:
		static const int MS_POUR_ENVOYER_POSITION = 25;

		std::list<Joueur*> tousLesJoueurs;
		std::list<Joueur*> joueursARetirer;

		long long int tempsDernierEnvoiPosition;
		long long int tempsSalon;

		bool joueurLock;

	public:		
		Salon();

		static const int SALON_MAX_JOUEURS = 6;

		size_t getNumberOfPlayer() const;
		bool isFull() const;
		bool isEmpty() const;

		void update(const long long int deltaT);

		void ajouterJoueur(Joueur* joueur);
		void retirerJoueur(Joueur* joueur);

		void broadcast(const std::string& message, const Joueur* joueurANePasEnvoyer = nullptr);
		void envoyerTousLesJoueursConnectes(Joueur* joueur);
		void fermerToutesConnexions();

		std::string getNouveauNom() const;
		std::string ajusterNom(const std::string& ancienNom) const;

		unsigned long long int getTimeInMicroseconds() const;

		bool enCollisionnAvecUnAutreJoueur(const Joueur* joueur);

		bool estDansSalon(const Joueur* joueur);

	private:
		std::string ajusterNomPrivate(const std::string& ancienNom) const;
	};
}