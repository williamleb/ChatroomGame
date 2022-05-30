/*
	Auteur : William Lebel

	Fichier non utilisé.
*/
#pragma once
#include "commands.h"
#include <deque>

namespace TP3Client
{
	class CommandBuffer
	{
		std::deque<OtherCommand_Base*> commands;

	public:		
		~CommandBuffer();

		const std::deque<OtherCommand_Base*>& getCommands() const;

		void storeCommand(OtherCommand_Base* command);
		void storeAndExecute(OtherCommand_Base* command, Other* player);
		void removeCommand();
	};
}