#include "stdafx.h"
#include "Netplay/GameClient.h"
#include "Netplay/ClientConnectionData.h"
#include "Netplay/GameClientConnection.h"
#include "Shared/MessageManager.h"
#include "Shared/Emulator.h"
#include "Shared/NotificationManager.h"
#include "Utilities/Socket.h"

GameClient::GameClient(Emulator* emu)
{
	_emu = emu;
	_stop = false;
	_connected = false;
}

GameClient::~GameClient()
{
}

bool GameClient::Connected()
{
	return _connected;
}

void GameClient::Connect(ClientConnectionData &connectionData)
{
	_emu->GetNotificationManager()->RegisterNotificationListener(shared_from_this());

	_stop = false;
	unique_ptr<Socket> socket(new Socket());
	if(socket->Connect(connectionData.Host.c_str(), connectionData.Port)) {
		_connection.reset(new GameClientConnection(_emu, std::move(socket), connectionData));
		_connected = true;
		_clientThread.reset(new thread(&GameClient::Exec, this));
	} else {
		MessageManager::DisplayMessage("NetPlay", "CouldNotConnect");
		_connected = false;
	}
}

void GameClient::Disconnect()
{
	_stop = true;
	_connected = false;
	if(_clientThread) {
		_clientThread->join();
		_clientThread.reset();
	}
}

void GameClient::Exec()
{
	if(_connected) {
		while(!_stop) {
			if(!_connection->ConnectionError()) {
				_connection->ProcessMessages();
				_connection->SendInput();
			} else {
				_connected = false;
				_connection->Shutdown();
				break;
			}
			std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1));
		}
	}
}

void GameClient::ProcessNotification(ConsoleNotificationType type, void* parameter)
{
	if(type == ConsoleNotificationType::GameLoaded &&
		std::this_thread::get_id() != _clientThread->get_id() && 
		!_emu->IsEmulationThread()
	) {
		//Disconnect if the client tried to manually load a game
		//A deadlock occurs if this is called from the emulation thread while a network message is being processed
		Disconnect();
	}
	
	if(_connection) {
		_connection->ProcessNotification(type, parameter);
	}
}

void GameClient::SelectController(uint8_t port)
{
	if(_connection) {
		_connection->SelectController(port);
	}
}

uint8_t GameClient::GetAvailableControllers()
{
	return _connection ? _connection->GetAvailableControllers() : 0;
}

uint8_t GameClient::GetControllerPort()
{
	return _connection ? _connection->GetControllerPort() : GameConnection::SpectatorPort;
}