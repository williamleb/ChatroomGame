  #pragma once
#include <io.h>
#include <winsock2.h>
#include <queue>
#include <thread>

#pragma comment(lib,"WS2_32")

namespace TP3Serveur
{
	class Joueur;
	
	/// <summary>
	/// Le serveur qui écoute les connexions.
	/// </summary>
	class Serveur
	{
		static const int PORT = 8888;

		WSADATA wsa;

		SOCKET socketDescription;
		SOCKET newSocket;

		sockaddr_in serveur;
		sockaddr_in client;

		std::queue<std::thread*> allConnexions;
		std::queue<Joueur*> allPlayers;
		
	public:		
		~Serveur();

		bool init();
		int run();
		void unload();

		void readConsole();

		
	};
}