/***************************************************************************
                          network.cc  -  Buffered network I/O
                               -------------------
      copyright            : (C) 2004 by Morten Hustveit
      email                : morten@rashbox.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>
#ifndef WIN32
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock.h>
#endif


#include <espace/api.h>
#include <espace/cvar.h>
#include <espace/network.h>
#include <espace/output.h>
#include <espace/stringlist.h>
#include <espace/system.h>

const int ALL_CLIENTS = -1;

#ifndef WIN32
typedef int SOCKET;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;
#endif

namespace
{
  String lastError = "No error";

  const int masterPort = 7171;
  const int gamePort = 7170;

  String gameName = "";
  SOCKET listenSocket = INVALID_SOCKET;

  struct Socket
  {
    Socket()
      : socket(INVALID_SOCKET),
        inBuffer(""),
        outBuffer("")
    {
    }

    SOCKET socket;

    std::string inBuffer;
    std::string outBuffer;

    time_t lastWrite;
    time_t lastRead;
  };

  enum MessageType
  {
    Bad,           /**< Bad message type. */
    Nop,           /**< No operation. */
    GameState,     /**< Game state. (Unused) */
    ConfigString,  /**< Configurations string. (Unused) */
    BaseLine,      /**< Baseline. (Unused) */
    ServerCommand, /**< Server command. */
    Download,      /**< Data from requested file. (Unused) */
    SnapshotLE,    /**< Little endian snapshot. */
    SnapshotBE,    /**< Big endian (non-Intel/Alpha) snapshot. */
    EOM,           /**< End of message (for UDP). */
    UserInfo,      /**< User console variables. */
    SystemInfo,    /**< System console variables. */
    GameType       /**< Game type (for kicking invalid clients). */
  };

  struct Client
  {
    Client()
      : snapshot(0)
    {
    }

    Socket socket;

    char* snapshot;

    std::list<std::string> messages; // Messages waiting to be sent

    std::map<String, String> userInfo;
  };

  typedef std::map<int, Client> ClientList;

  ClientList clients;
  int nextClient = 0; // ID of next client

  Client localClient;

  enum MasterMode
  {
    ListGames,
    AddGame,
    RemoveGame,
    NOP,
    Quit
  };

  std::list<MasterMode> masterMode;
  Socket master;
  time_t masterReconnect = 0;
  time_t masterBackoff = 2;

  void deinitialize();
  bool connectToMaster();

  char buffer[65536];

  bool bigEndian = false;
}

GameList   Network::games;
NetConfig  Network::config;

String Network::getLastError()
{
  return lastError;
}

bool Network::startGame(const String& name, bool lanOnly)
{
  deinitialize();

  // Test byte order
  uint16_t test = 0xABCD;
  bigEndian = (0xAB == *reinterpret_cast<uint8_t*>(&test));

  if(name.length() == 0 || name.length() > 16)
  {
    lastError = "Game name must be between 1 and 16 characters long";

    return false;
  }

  if(config.gameType.length() == 0 || config.gameType.length() > 16)
  {
    lastError = "Game type must be between 1 and 16 characters long";

    return false;
  }

  if(config.maxPlayers <= 0)
  {
    lastError = "Maximum player count must be greater than zero";

    return false;
  }

  if(!config.eventHandler)
  {
    lastError = "Missing event handler";

    return false;
  }

  if(!config.mapName.length())
    config.mapName = "none";

  if(INVALID_SOCKET == (listenSocket = socket(PF_INET, SOCK_STREAM, 0)))
  {
    lastError = "socket() call failed";

    return false;
  }

  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(gamePort);

  if(SOCKET_ERROR == bind(listenSocket,
                          reinterpret_cast<struct sockaddr*>(&address),
                          sizeof(address)))
  {
    close(listenSocket);

    listenSocket = INVALID_SOCKET;

    lastError = "Failed to bind TCP/IP socket (";
    lastError += strerror(errno);
    lastError += ")";

    return false;
  }

  if(SOCKET_ERROR == listen(listenSocket, 16))
  {
    close(listenSocket);

    listenSocket = INVALID_SOCKET;

    lastError = "Failed to start listening on TCP socket";

    return false;
  }

  if(SOCKET_ERROR == fcntl(listenSocket, F_SETFL, O_NONBLOCK))
  {
    close(listenSocket);

    listenSocket = INVALID_SOCKET;

    lastError = "Failed to make TCP/IP socket non-blocking";

    return false;
  }

  if(!name.length())
  {
    lastError = "Missing game name";

    return false;
  }

  if(name.length() > 16)
  {
    lastError = "Game name too long";

    return false;
  }

  if(!lanOnly && master.socket == INVALID_SOCKET)
  {
    if(!connectToMaster())
    {
      esWarning << "Connection to master server failed: " + lastError << std::endl;
      esWarning << "Clients may only connect by address because of this." << std::endl;

      masterReconnect = INT_MAX;
    }
    else
    {
      std::ostringstream command;

      command << "ADD " << name
              << " " << config.gameType
              << " " << config.mapName
              << (config.localPlayer ? " 1 " : " 0 ")
              << config.maxPlayers
              << (config.password ? " 1\n" : " 0\n");

      master.outBuffer += command.str();

      masterMode.push_back(AddGame);
    }
  }

  gameName = name;

  return true;
}

void Network::sendClientCommand(int client, const String& command)
{
  std::string message = "";

  message += static_cast<char>(ServerCommand);
  message.append(static_cast<const char*>(command),
                 static_cast<const char*>(command) + command.length());

  if(client == ALL_CLIENTS)
  {
    for(ClientList::iterator i = clients.begin(); i != clients.end(); ++i)
    {
      i->second.messages.push_back(message);
    }

    return;
  }

  ClientList::iterator i;

  if(clients.end() == (i = clients.find(client)))
  {
    esWarning << "Network::sendClientCommand: client " << client
              << " not found." << std::endl;

    return;
  }
  else
  {
    i->second.messages.push_back(message);
  }
}

void Network::sendSnapshot(int client, const void* _snapshot)
{
  if(client == ALL_CLIENTS)
  {
    for(ClientList::iterator i = clients.begin(); i != clients.end(); ++i)
    {
      sendSnapshot(i->first, _snapshot);
    }

    return;
  }

  ClientList::iterator i;

  if(clients.end() == (i = clients.find(client)))
  {
    esWarning << "Network::sendSnapshot: client " << client
              << " not found." << std::endl;

    return;
  }

  const char* snapshot = reinterpret_cast<const char*>(_snapshot);

  Client& c = i->second;

  std::string message = "";

  message += static_cast<char>(bigEndian ? SnapshotBE : SnapshotLE);

  uint offset = 0;

  while(offset < config.snapshotSize)
  {
    uint step, length;

    for(step = 0; step < UCHAR_MAX && offset < config.snapshotSize; ++offset)
    {
      if(!(config.snapshotMask[offset] & ServerToClient))
        continue;

      if(c.snapshot[offset] != snapshot[offset])
        break;

      ++step;
    }

    message += static_cast<char>(step);

    uint j = 0;

    for(length = 0; length < UCHAR_MAX && offset + j < config.snapshotSize; ++j)
    {
      if(!(config.snapshotMask[offset + j] & ServerToClient))
        continue;

      if(c.snapshot[offset + j] == snapshot[offset + j])
        break;

      c.snapshot[offset + j] = snapshot[offset + j];

      ++length;
    }

    message += static_cast<char>(length);

    for(; j--; ++offset)
    {
      if(config.snapshotMask[offset] & ServerToClient)
        message += snapshot[offset];
    }
  }

  c.messages.push_back(message);
}

String Network::userInfo(int client, const String& key)
{
  // XXX
  return String::null;
}

void Network::disconnect(int client)
{
  // XXX
}

void Network::updateGames(bool lanOnly)
{
  if(config.gameType.length() == 0 || config.gameType.length() > 16)
  {
    esWarning << "Game type must be between 1 and 16 characters long";

    return;
  }

  if(!lanOnly)
  {
    if(master.socket != INVALID_SOCKET || connectToMaster())
    {
      games.clear();

      master.outBuffer += "LIST\n";

      masterMode.push_back(ListGames);
    }
  }

  // XXX: Broadcast request on LAN
}

bool Network::connect(const String& name)
{
  deinitialize();

  // Test byte order
  uint16_t test = 0xABCD;
  bigEndian = (0xAB == *reinterpret_cast<uint8_t*>(&test));

  if(config.gameType.length() == 0 || config.gameType.length() > 16)
  {
    lastError = "Game type must be between 1 and 16 characters long";

    return false;
  }

  if(!config.eventHandler)
  {
    lastError = "Missing event handler";

    return false;
  }

  struct hostent* host = gethostbyname(name);

  if(!host)
  {
    if(games.empty())
    {
      while(!masterMode.empty() && master.socket != INVALID_SOCKET)
        idle();
    }

    for(GameList::iterator i = games.begin(); i != games.end(); ++i)
    {
      if(i->name == name)
      {
        host = gethostbyname(i->host);

        break;
      }
    }

    if(!host)
    {
      lastError = "Failed to resolve host/invalid game name";

      return false;
    }
  }

  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_port = htons(gamePort);
  memcpy(&address.sin_addr, host->h_addr, host->h_length);

  SOCKET clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  if(INVALID_SOCKET == clientSocket)
  {
    lastError = "Failed to create TCP/IP socket";

    return false;
  }

  if(SOCKET_ERROR == ::connect(clientSocket,
                               reinterpret_cast<struct sockaddr*>(&address),
                               sizeof(address)))
  {
    lastError = "Failed to connect to remote host (";
    lastError += strerror(errno);
    lastError += ")";

    close(clientSocket);

    return false;
  }

  if(SOCKET_ERROR == fcntl(clientSocket, F_SETFL, O_NONBLOCK))
  {
    lastError = "Failed to make TCP/IP socket non-blocking";

    close(clientSocket);

    return false;
  }

  localClient.socket.socket = clientSocket;
  localClient.socket.lastRead = localClient.socket.lastWrite = time(0);

  if(!localClient.snapshot)
  {
    localClient.snapshot = new char[config.snapshotSize];

    memset(localClient.snapshot, 0, config.snapshotSize);
  }

  String vars = CVar::getVars(CVar::UserInfo);

  std::string message = "";

  message += static_cast<char>(UserInfo);

  message.append(static_cast<const char*>(vars),
                 static_cast<const char*>(vars) + vars.length());

  localClient.messages.push_back(message);

  message = "";

  message += static_cast<char>(GameType);

  message.append(
    static_cast<const char*>(config.gameType),
    static_cast<const char*>(config.gameType) + config.gameType.length());

  localClient.messages.push_back(message);

  return true;
}

void Network::disconnect()
{
  close(localClient.socket.socket);

  localClient.socket.socket = INVALID_SOCKET;

  config.eventHandler(0, Disconnect);
}

void Network::sendSnapshot(const void* _snapshot)
{
  if(localClient.socket.socket == INVALID_SOCKET)
  {
    esWarning << "sendSnapshot called with no connection to server."
              << std::endl;

    return;
  }

  const char* snapshot = reinterpret_cast<const char*>(_snapshot);

  Client& c = localClient;

  std::string message = "";

  message += static_cast<char>(bigEndian ? SnapshotBE : SnapshotLE);

  uint offset = 0;

  while(offset < config.snapshotSize)
  {
    uint step, length;

    for(step = 0; step < UCHAR_MAX && offset < config.snapshotSize; ++offset)
    {
      if(!(config.snapshotMask[offset] & ClientToServer))
        continue;

      if(c.snapshot[offset] != snapshot[offset])
        break;

      ++step;
    }

    message += static_cast<char>(step);

    uint j = 0;

    for(length = 0; length < UCHAR_MAX && offset + j < config.snapshotSize; ++j)
    {
      if(!(config.snapshotMask[offset + j] & ClientToServer))
        continue;

      if(c.snapshot[offset + j] == snapshot[offset + j])
        break;

      c.snapshot[offset + j] = snapshot[offset + j];

      ++length;
    }

    message += static_cast<char>(length);

    for(; j--; ++offset)
    {
      if(config.snapshotMask[offset] & ClientToServer)
        message += snapshot[offset];
    }
  }

  c.messages.push_back(message);
}

void Network::idle()
{
  time_t now = time(0);

  if(masterReconnect && now >= masterReconnect)
  {
    if(connectToMaster())
    {
      masterReconnect = 0;
    }
    else
    {
      esWarning << "Reconnect to master server failed.  Retrying in "
                << masterBackoff << " seconds." << std::endl;

      masterReconnect = now + masterBackoff;

      masterBackoff *= 2;
    }
  }

  fd_set readSet, writeSet;
  FD_ZERO(&readSet);
  FD_ZERO(&writeSet);
#ifndef WIN32
  int maxfd = 0;
#endif

  if(master.socket != INVALID_SOCKET)
  {
    // Keep master server connection alive
    if(now - master.lastWrite > 250 && master.outBuffer.empty())
    {
      master.outBuffer += "NOP\r\n";

      masterMode.push_back(NOP);
    }

#ifndef WIN32
    maxfd = master.socket;
#endif

    FD_SET(master.socket, &readSet);

    if(!master.outBuffer.empty())
      FD_SET(master.socket, &writeSet);
  }

  if(localClient.socket.socket != INVALID_SOCKET)
  {
#ifndef WIN32
    if(localClient.socket.socket > maxfd)
      maxfd = localClient.socket.socket;
#endif

    FD_SET(localClient.socket.socket, &readSet);

    for(std::list<std::string>::iterator i = localClient.messages.begin();
        i != localClient.messages.end(); ++i)
    {
      if(i->length() > 0xFFFF)
      {
        esWarning << "Message length too large (" << i->length()
                  << " bytes, max is 65535)." << std::endl;

        continue;
      }

      localClient.socket.outBuffer += static_cast<char>(i->length() >> 8);
      localClient.socket.outBuffer += static_cast<char>(i->length());
      localClient.socket.outBuffer += *i;
    }

    localClient.messages.clear();

    if(!localClient.socket.outBuffer.empty())
      FD_SET(localClient.socket.socket, &writeSet);
  }

  for(ClientList::iterator i = clients.begin(); i != clients.end(); ++i)
  {
    Client& client = i->second;

#ifndef WIN32
    if(client.socket.socket > maxfd)
      maxfd = client.socket.socket;
#endif

    FD_SET(client.socket.socket, &readSet);

    for(std::list<std::string>::iterator j = client.messages.begin();
        j != client.messages.end(); ++j)
    {
      if(j->length() > 0xFFFF)
      {
        esWarning << "Message length too long (" << j->length()
                  << " bytes, max is 65535)." << std::endl;

        continue;
      }

      client.socket.outBuffer += static_cast<char>(j->length() >> 8);
      client.socket.outBuffer += static_cast<char>(j->length());
      client.socket.outBuffer += *j;
    }

    client.messages.clear();

    if(!client.socket.outBuffer.empty())
      FD_SET(client.socket.socket, &writeSet);
  }

  if(listenSocket != INVALID_SOCKET)
  {
    FD_SET(listenSocket, &readSet);

#ifndef WIN32
    if(listenSocket > maxfd)
      maxfd = listenSocket;
#endif
  }

#ifndef WIN32
  if(maxfd == 0)
    return;
#endif

  struct timeval timeout;

  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

#ifndef WIN32
  int count = select(maxfd + 1, &readSet, &writeSet, 0, &timeout);
#else
  int count = select(0, &readSet, &writeSet, 0, &timeout);
#endif

  if(count == 0)
    return;

  if(count == -1)
  {
    esWarning << "select failed: " << strerror(errno) << "." << std::endl;

    return;
  }

  if(listenSocket != INVALID_SOCKET && FD_ISSET(listenSocket, &readSet))
  {
    SOCKET clientSocket = accept(listenSocket, 0, 0);

    if(clientSocket != INVALID_SOCKET)
    {
      if(clients.size() >= config.maxPlayers - (config.localPlayer ? 1 : 0))
      {
        close(clientSocket);
      }
      else if(SOCKET_ERROR == fcntl(clientSocket, F_SETFL, O_NONBLOCK))
      {
        esWarning << "Failed to set client socket to non-blocking." << std::endl;

        close(clientSocket);
      }
      else
      {
        Client client;

        client.socket.socket = clientSocket;
        client.snapshot = new char[config.snapshotSize];
        memset(client.snapshot, 0, config.snapshotSize);

        String vars = CVar::getVars(CVar::SystemInfo);

        std::string message = "";

        message += static_cast<char>(SystemInfo);

        message.append(static_cast<const char*>(vars),
                       static_cast<const char*>(vars) + vars.length());

        client.messages.push_back(message);

        clients[nextClient] = client;

        config.eventHandler(nextClient, Connect);

        std::ostringstream command;

        command << "ADD " << gameName
                << " " << config.gameType
                << " " << config.mapName
                << " " << ((config.localPlayer ? 1 : 0) + clients.size())
                << " " << config.maxPlayers
                << (config.password ? " 1\n" : " 0\n");

        master.outBuffer += command.str();

        masterMode.push_back(AddGame);

        ++nextClient;
      }
    }
  }

  if(master.socket != INVALID_SOCKET)
  {
    if(FD_ISSET(master.socket, &readSet))
    {
      int amount = read(master.socket, buffer, sizeof(buffer));

      if(amount <= 0)
      {
        if(amount < 0)
          esWarning << "Socket read error: " << strerror(errno)
                    << ", disconnecting." << std::endl;

        esWarning << "Disconnected from master server.  Reconnecting in "
                  << masterBackoff << " seconds." << std::endl;

        masterReconnect = now + masterBackoff;

        close(master.socket);

        master.socket = INVALID_SOCKET;

        masterBackoff *= 2;

        return;
      }

      master.inBuffer.append(buffer, buffer + amount);

      master.lastRead = now;

      masterBackoff = 2;

      for(;;)
      {
        std::string::size_type pos = master.inBuffer.find_first_of("\r\n");

        if(pos == master.inBuffer.npos)
          break;

        std::string line = master.inBuffer.substr(0, pos);

        master.inBuffer.erase(0, pos);

        std::string::size_type epos = master.inBuffer.find_first_not_of("\r\n");

        if(epos == master.inBuffer.npos)
          master.inBuffer = "";
        else
          master.inBuffer.erase(0, epos);

        if(masterMode.empty())
        {
          esWarning << "Received unexpected data from master server: \""
                    << line << "\"." << std::endl;
        }

        switch(masterMode.front())
        {
        case ListGames:

          if(line == ".")
          {
            masterMode.pop_front();
          }
          else
          {
            std::basic_istringstream<char> istream(line);

            std::string name, type, mapName, password, host;
            uint players, maxPlayers;

            istream >> name >> type >> mapName >> players >> maxPlayers >> password >> host;

            if(config.gameType != type.c_str())
              break;

            GameInfo info;

            info.name = name.c_str();
            info.host = host.c_str();
            info.mapName = mapName.c_str();
            info.players = players;
            info.maxPlayers = maxPlayers;
            info.password = (password == "true");

            games.push_back(info);
          }

          break;

        case AddGame:
        case RemoveGame:
        case NOP:
        case Quit:

          if(line.length() > 4 && line.substr(0, 4) == "ERR ")
          {
            esWarning << "Error from master server: " << line.substr(4)
                      << std::endl;
          }

          masterMode.pop_front();

          break;
        }
      }
    }

    if(FD_ISSET(master.socket, &writeSet))
    {
      int amount = write(master.socket, master.outBuffer.data(),
                         master.outBuffer.size());

      if(amount < 0)
      {
        esWarning << "Socket write error: " << strerror(errno)
                  << ", disconnecting." << std::endl
                  << "Disconnected from master server.  Reconnecting in "
                  << masterBackoff << " seconds." << std::endl;

        masterReconnect = now + masterBackoff;

        close(master.socket);

        master.socket = INVALID_SOCKET;

        masterBackoff *= 2;

        return;
      }

      master.outBuffer.erase(0, amount);

      master.lastWrite = now;
    }
  }

  if(localClient.socket.socket != INVALID_SOCKET)
  {
    Socket& socket = localClient.socket;

    if(FD_ISSET(socket.socket, &readSet))
    {
      int amount = read(socket.socket, buffer, sizeof(buffer));

      if(amount <= 0)
      {
        if(amount < 0)
          esWarning << "Socket read error: " << strerror(errno)
                    << ", disconnecting." << std::endl;

        close(socket.socket);

        socket.socket = INVALID_SOCKET;

        config.eventHandler(0, Disconnect);

        return;
      }

      socket.inBuffer.append(buffer, buffer + amount);

      socket.lastRead = now;

      while(socket.inBuffer.size() >= 2)
      {
        uint size = static_cast<uint8_t>(socket.inBuffer[0]) << 8
                  | static_cast<uint8_t>(socket.inBuffer[1]);

        if(socket.inBuffer.size() < size + 2)
          break;

        std::string message = socket.inBuffer.substr(2, size);

        socket.inBuffer.erase(0, size + 2);

        switch(message[0])
        {
        case Nop:

          break;

        case ServerCommand:

          API::appendCommand(message.substr(1).c_str());

          break;

        case SnapshotLE:
        case SnapshotBE:

          {
            bool swap = ((message[0] == SnapshotBE) != bigEndian);
            uint in = 1;
            uint offset = 0;

            while(in < message.length())
            {
              uint step = static_cast<unsigned char>(message[in++]);
              uint length = static_cast<unsigned char>(message[in++]);

              for(; step && offset < config.snapshotSize; ++offset)
              {
                if(config.snapshotMask[offset] & ServerToClient)
                  --step;
              }

              for(; length && offset < config.snapshotSize; ++offset)
              {
                if(config.snapshotMask[offset] & ServerToClient)
                {
                  --length;

                  if(!swap)
                  {
                    localClient.snapshot[offset] = message[in++];
                  }
                  else // swap
                  {
                    localClient.snapshot[(offset & ~3) + 3 - (offset & 3)]
                      = message[in++];
                  }
                }
              }

              if(step || length)
                esWarning << "Invalid snapshot received." << std::endl;
            }

            config.eventHandler(0, Snapshot, localClient.snapshot);
          }

          break;

        case SystemInfo:

          esDebug() << "Got SystemInfo." << std::endl;

          break;

        default:

          esDebug() << "Message type " << int(message[0]) << " received."
                    << std::endl;
        }
      }
    }

    if(FD_ISSET(socket.socket, &writeSet))
    {
      int amount = write(socket.socket, localClient.socket.outBuffer.data(),
                         localClient.socket.outBuffer.size());

      if(amount < 0)
      {
        esWarning << "Socket write error: " << strerror(errno)
                  << ", disconnecting." << std::endl;

        close(socket.socket);

        localClient.socket.socket = INVALID_SOCKET;

        config.eventHandler(0, Disconnect);

        return;
      }

      localClient.socket.outBuffer.erase(0, amount);

      localClient.socket.lastWrite = now;
    }
  }

  for(ClientList::iterator i = clients.begin(); i != clients.end(); ++i)
  {
    Client& client = i->second;

    Socket& socket = client.socket;

    if(FD_ISSET(socket.socket, &readSet))
    {
      int amount = read(socket.socket, buffer, sizeof(buffer));

      if(amount <= 0)
      {
        if(amount < 0)
          esInfo << "Socket read error on client " << i->first << ": "
                 << strerror(errno) << ", disconnected." << std::endl;

        close(socket.socket);

        clients.erase(i);

        config.eventHandler(i->first, Disconnect);

        continue;
      }

      socket.inBuffer.append(buffer, buffer + amount);

      socket.lastRead = now;

      while(socket.inBuffer.size() >= 2)
      {
        uint size = static_cast<uint8_t>(socket.inBuffer[0]) << 8
                  | static_cast<uint8_t>(socket.inBuffer[1]);

        if(socket.inBuffer.size() < size + 2)
          break;

        std::string message = socket.inBuffer.substr(2, size);

        socket.inBuffer.erase(0, size + 2);

        switch(message[0])
        {
        case Nop:

          break;

        case SnapshotLE:
        case SnapshotBE:

          {
            bool swap = ((message[0] == SnapshotBE) != bigEndian);
            uint in = 1;
            uint offset = 0;

            while(in < message.length())
            {
              uint step, length;

              for(step = static_cast<unsigned char>(message[in++]);
                  step && offset < config.snapshotSize; ++offset)
              {
                if(config.snapshotMask[offset] & ClientToServer)
                  --step;
              }

              for(length = static_cast<unsigned char>(message[in++]);
                  length && offset < config.snapshotSize; ++offset)
              {
                if(config.snapshotMask[offset] & ClientToServer)
                {
                  --length;

                  if(!swap)
                  {
                    client.snapshot[offset] = message[in++];
                  }
                  else // swap
                  {
                    client.snapshot[(offset & ~3) + 3 - (offset & 3)]
                      = message[in++];
                  }
                }
              }

              if(step || length)
                esWarning << "Invalid snapshot received." << std::endl;
            }

            config.eventHandler(i->first, Snapshot, client.snapshot);
          }

          break;

        case UserInfo:

          esDebug() << "Got UserInfo." << std::endl;

          break;

        default:

          esDebug() << "Message type " << int(message[0]) << " received."
                    << std::endl;
        }
      }
    }

    if(FD_ISSET(socket.socket, &writeSet))
    {
      int amount = write(socket.socket, socket.outBuffer.data(),
                         socket.outBuffer.size());

      if(amount < 0)
      {
        esInfo << "Socket write error on client " << i->first << ": "
               << strerror(errno) << ", disconnected." << std::endl;

        close(socket.socket);

        clients.erase(i);

        config.eventHandler(i->first, Disconnect);

        continue;
      }

      socket.outBuffer.erase(0, amount);

      socket.lastWrite = now;
    }
  }
}

void Network::shutdown()
{
  if(clients.empty())
    return;

  sendClientCommand(ALL_CLIENTS, "disconnect");

  float time = System::time();

  while(System::time() < time + 0.5 && !clients.empty())
    Network::idle();
}

namespace
{
  void deinitialize()
  {
    if(listenSocket != INVALID_SOCKET)
    {
      close(listenSocket);

      listenSocket = INVALID_SOCKET;
    }

    if(localClient.socket.socket != INVALID_SOCKET)
    {
      close(localClient.socket.socket);

      localClient.socket = Socket();

      memset(localClient.snapshot, 0, Network::config.snapshotSize);

      localClient.messages.clear();
      localClient.userInfo.clear();
    }

    for(ClientList::iterator i = clients.begin();
        i != clients.end(); ++i)
    {
      close(i->second.socket.socket);
    }

    clients.clear();
  }

  bool connectToMaster()
  {
    if(Network::config.masterServer.isNull())
    {
      lastError = "Master server name unset.";

      return false;
    }

    struct hostent* host = gethostbyname(Network::config.masterServer);

    if(!host)
    {
      lastError = "Failed to resolve host \"";
      lastError += Network::config.masterServer;
      lastError += "\"";

      return false;
    }

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(masterPort);
    memcpy(&address.sin_addr, host->h_addr, host->h_length);

    master.socket = socket(PF_INET, SOCK_STREAM, 0);

    if(master.socket == INVALID_SOCKET)
      return false;

    if(SOCKET_ERROR == connect(master.socket,
                               reinterpret_cast<struct sockaddr*>(&address),
                               sizeof(address)))
    {
      lastError = strerror(errno);

      close(master.socket);

      master.socket = INVALID_SOCKET;

      return false;
    }

    if(SOCKET_ERROR == fcntl(master.socket, F_SETFL, O_NONBLOCK))
    {
      lastError = "Failed to make socket non-blocking (";
      lastError += strerror(errno);
      lastError += ")";

      close(master.socket);

      master.socket = INVALID_SOCKET;

      return false;
    }

    master.inBuffer = "";
    master.outBuffer = "";
    master.lastRead = master.lastWrite = time(0);
    masterMode.clear();

    return true;
  }
}

// vim: ts=2 sw=2 et
