#include "serveur.h"
#include <locale.h>

int main()
{
	setlocale(LC_ALL, "");
	TP3Serveur::Serveur serveur;
	return serveur.run();
}
