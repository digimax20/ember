/*
 Copyright (C) 2002  Martin Pollard (Xmp)

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include "ServerServiceSignals.h"

#include "framework/Service.h"

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>

#include <Atlas/Message/Element.h>


#include <boost/filesystem/path.hpp>
#include <memory>

namespace Eris {
class Avatar;

class Connection;

class View;

class Lobby;

class Account;

class Entity;

class TransferInfo;

class Session;
}

namespace Ember {

class ConfigService;

class OOGChat;

class NonConnectedState;

/**
 * @brief Ember Server Service
 *
 * Handles most interaction with the server.
 *
 * @author Martin Pollard (Xmp)
 *
 * @see Ember::Service
 * @see Ember::MetaserverService
 * @see Ember::ConsoleObject
 */
class ServerService : public Service, public ServerServiceSignals {
public:

	/**
	 * @brief Ctor.
	 */
	explicit ServerService(Eris::Session& session);

	/**
	 * @brief Dtor.
	 */
	~ServerService() override;

	void setupLocalServerObservation(ConfigService& configService);

	bool start() override;

	void stop() override;

	/**
	 * @brief Connects to a remote host.
	 * @param host The hostname of the remote host.
	 * @param port The port on the remote host.
	 * @return True if connection was successful.
	 */
	bool connect(const std::string& host, short port = 6767);

	/**
	 * @brief Connects to a local socket.
	 * @return True if connection was successful.
	 */
	bool connectLocal();

	/**
	 * @brief Checks if there's a local socket available.
	 * @return True if a local socket exists.
	 */
	bool hasLocalSocket();

	void disconnect();

	/**
	 * @brief Takes a transferred character.
	 * @param transferInfo Information needed about the transfer.
	 */
	void takeTransferredCharacter(const Eris::TransferInfo& transferInfo);

	/**
	 * @brief Logs out the user.
	 *
	 * If the user has an avatar, that will be logged out. If the user doesn't have an avatar yet but is logged in through the account, the account will be logged out.
	 */
	bool logout();

	/**
	 * @brief Takes the character with the specified id.
	 *
	 * @param id The id of the character to take.
	 */
	void takeCharacter(const std::string& id);

	/**
	 * @brief Creates a new character on the server.
	 * @param name The name of the new character.
	 * @param sex The sex. In most cases either "male" or "female".
	 * @param type The type. This should correspond to one of the available types as sent from the server earlier.
	 * @param description A short description of the character.
	 * @param spawnName The name of the spawn area to spawn in. This should have been sent from the server earlier. Make sure that the type specified is available for the spawn point. If an empty string is specified the earlier spawn mechanism on the server will be used, which isn't guaranteed to always work.
	 * @param extraProperties Any additional properties we want to set. Note that the server might reject any property, depending on rules and access rules.
	 */
	bool createCharacter(const std::string& name, const std::string& sex, const std::string& type, const std::string& description, const std::string& spawnName, const Atlas::Message::MapType& extraProperties = Atlas::Message::MapType());

	Eris::Avatar* getAvatar() const;

	/**
	 * @brief Gets the underlying connection.
	 * @returns A connection instance, or null if we're currently not connected
	 */
	Eris::Connection* getConnection() const;

	/**
	 * @brief Gets the used account, if available.
	 * @return An account, or null if none has been created.
	 */
	Eris::Account* getAccount() const;

	sigc::signal<void> EventLocalSocketChanged;

private:

	void gotConnection(Eris::Connection* connection);

	void gotAvatar(Eris::Avatar* avatar);

	void gotAccount(Eris::Account* account);

	void destroyedAccount();

	void destroyedAvatar();

	Eris::Session& mSession;

	Eris::Connection* mConnection;
	Eris::Account* mAccount;
	Eris::Avatar* mAvatar;

	/**
	 * @brief Contains the class that controls Out of Game Chat
	 */
	OOGChat* mOOGChat;

	std::unique_ptr<NonConnectedState> mNonConnectedState;

	boost::filesystem::path mLocalSocketPath;

};

} // namespace Ember

#endif
