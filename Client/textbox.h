#pragma once

/* 
	Fichier créé par François Paradis dans le cadre du cours Programmation Jeu Vidéo III 
	Modifications par William Lebel.
*/

#include <SFML/Graphics.hpp>

using namespace sf;


namespace TP3Client
{
	/// <summary>
	/// Textbox de base, pleine de flaws, mais qui fera la job pour ce TP
	/// Notez que on peut l'utiliser aussi comme texte d'information.  Idéalement,
	/// On aurait une structure TexteDeBase comme superclasse avec Label et Textbox
	/// Comme sous-classe.  Ceci dit je suis très serré dans le temps, alors je livre
	/// vite quelque chose qui marche.
	/// </summary>
	class Textbox
	{
	//DOC DES MÉTHODES DANS LE .CPP
	public:
		Textbox();
		~Textbox();
		void init(const int longueur, const int limite, const Vector2f position, const Font& font, Textbox* next = nullptr);
		void initInfo(const Vector2f position, const Font& font, const bool estErreur);
		void insererTexte(std::string leTexte);
		void ajouterChar(const char leChar);
		void retirerChar();
		void dessiner(RenderWindow* const window);
		void selectionner();
		void deSelectionner();
		bool touche(const Vector2i& position);
		String getTexte() const;
		Textbox* getNext();
		void setPosition(const Vector2f& position);


	private:
		void ajustementsVisuels();		
		
		/// <summary>
		/// Limite de caractères que notre textbox tolère.  Mettez 1 de limite par 20 pixels de largeur de votre boite
		/// Ainsi si vous entrez que des caractères larges (comme W par exemple), tout va entrer
		/// </summary>
		int limite;
				
		/// <summary>
		/// Graphiquement, la textbox est un simple rectangle.
		/// </summary>
		RectangleShape boite;
		
		/// <summary>
		/// Le texte à afficher.
		/// </summary>
		Text texte;
		
		/// <summary>
		/// Un pointeur sur le prochain textbox à sélectionner.
		/// </summary>
		Textbox* next;

		bool isLabel;
		
	};
}

