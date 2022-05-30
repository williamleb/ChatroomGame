#pragma once
#include <string>
#include <io.h>
#include <winsock2.h>

#pragma comment(lib,"WS2_32")

namespace TP3Client
{	
	/// <summary>
	/// Le client est la couche "connexion" du client entre celui-ci et le serveur.
	/// </summary>
	class Client
	{
		static std::string IP;
		static const int PORT = 8888;
		static const int REPONSE_BUFFER_SIZE = 2000;

		WSADATA wsa;

		SOCKET socketDescription;

		sockaddr_in serveur;

		bool running;

	public:
		Client();
		~Client();

		bool init();
		void run();
		void unload();

		bool sendToServer(const std::string& message);
		void stop();

		bool isRunning();

		static const std::string& getIP();
		static void setIP(const std::string& ip);
	};
}