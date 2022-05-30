#include "client.h"
#include "gestionnaireDeCommunication.h"

using namespace TP3Client;

/// <summary>
/// L'adresse IP par d�faut � laquelle se connecter.
/// </summary>
std::string Client::IP("127.0.0.1");

/// <summary>
/// Retourne l'adresse IP de la connexion.
/// </summary>
/// <returns>L'adresse IP de la connexion.</returns>
const std::string& Client::getIP()
{
	return IP;
}

/// <summary>
/// Change l'adresse IP de la connexion.
/// </summary>
/// <param name="ip">La nouvelle adresse IP de la connexion.</param>
void Client::setIP(const std::string& ip)
{
	IP = ip;
}

/// <summary>
/// Cr�e un nouveau <see cref="Client"/>.
/// </summary>
Client::Client()
	:socketDescription(INVALID_SOCKET), running(false)
{
}

/// <summary>
/// D�truit un <see cref="Client"/>.
/// </summary>
Client::~Client()
{
	unload();
}

/// <summary>
/// Initialise la connexion entre le client et le serveur.
/// </summary>
/// <returns>true si la connexion s'est initialis�e avec succ�s; false sinon.</returns>
bool Client::init()
{
	// Initialise la librairie Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	// Initialisation du socket.
	socketDescription = socket(AF_INET, SOCK_STREAM, 0);
	if (socketDescription == INVALID_SOCKET)
	{
		return false;
	}

	// Pr�paration des informations du serveur.
	serveur.sin_addr.s_addr = inet_addr(IP.c_str());
	serveur.sin_family = AF_INET;
	serveur.sin_port = htons(PORT);

	// Connexion au serveur.
	if (connect(socketDescription, (sockaddr *)&serveur, sizeof(serveur)) < 0)
	{
		return false;
	}
	
	return true;
}

/// <summary>
/// Ex�cute la connexion entre le client et le serveur (�coute les messages du serveur).
/// Les messages sont envoy�s au gestionnaire de communication.
/// </summary>
void Client::run()
{
	running = true;
	int recv_size = 0;
	char reponse[REPONSE_BUFFER_SIZE];

	// Tant qu'on est connect�...
	do
	{
		// On re�oit les messages du serveur.
		recv_size = recv(socketDescription, reponse, REPONSE_BUFFER_SIZE, 0);
		
		if (recv_size >= 0 && recv_size < REPONSE_BUFFER_SIZE)
		{
			reponse[recv_size] = '\0';
		}

		// On envoit le message au jeu.
		GestionnaireDeCommunication::getInstance()->receiveMessage(reponse);

	} while (recv_size > 0 && running);

	running = false;
}

/// <summary>
/// D�charge l'instance du client.
/// </summary>
void Client::unload()
{
	closesocket(socketDescription);
	WSACleanup();
}

/// <summary>
/// Envoit un message au serveur.
/// La connexion doit avoir �t� initialis�e avant d'appeler cette m�thode.
/// </summary>
/// <param name="message">Le message � envoyer au serveur.</param>
/// <returns>true si le message a bien �t� envoy�; false sinon.</returns>
bool Client::sendToServer(const std::string& message)
{
	// On envoie le messsage au serveur.
	if (send(socketDescription, message.c_str(), (int)strlen(message.c_str()), 0) == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

/// <summary>
/// D�termine si la connexion entre le client et le serveur est pr�sentement en ex�cution.
/// </summary>
/// <returns>true si la connexion est en ex�cution; false sinon.</returns>
bool Client::isRunning()
{
	return running;
}

/// <summary>
/// Arr�te l'ex�cution de la communication entre le client et le serveur.
/// </summary>
void Client::stop()
{
	closesocket(socketDescription);
	running = false;
}