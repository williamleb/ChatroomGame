#include "commandBuffer.h"

using namespace TP3Client;

/// <summary>
/// Détruit un <see cref="CommandBuffer"/>.
/// </summary>
CommandBuffer::~CommandBuffer()
{
	for (OtherCommand_Base* command : commands)
	{
		delete command;
	}
}

/// <summary>
/// Accède aux commandes.
/// </summary>
/// <returns>La liste de commandes.</returns>
const std::deque<OtherCommand_Base*>& CommandBuffer::getCommands() const
{
	return commands;
}

/// <summary>
/// Ajoute une nouvelle commande à la fin de la liste.
/// </summary>
/// <param name="command">La commande à ajouter.</param>
void CommandBuffer::storeCommand(OtherCommand_Base* command)
{
	commands.push_back(command);
}

/// <summary>
/// Ajoute une nouvelle commande à la fin de la liste et l'exécute.
/// </summary>
/// <param name="command">La commande à ajouter et à exécuter.</param>
/// <param name="player">Le joueur avec lequel exécuter la commande.</param>
void CommandBuffer::storeAndExecute(OtherCommand_Base* command, Other* player)
{
	storeCommand(command);
	command->execute(player);
}

/// <summary>
/// Retire la première commande de la liste.
/// </summary>
void CommandBuffer::removeCommand()
{
	delete commands.front();
	commands.pop_front();
}