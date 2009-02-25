#ifndef NETWORK_H_
#define NETWORK_H_

#include <vector>

#include "string.h"
#include "types.h"

struct NetConfig
{
  typedef void (*EventHandler)(int client, int event, void* arg);

  EventHandler    eventHandler;
  uint            snapshotSize;
  const char*     snapshotMask;
  String          masterServer;
  String          gameType;
  String          mapName;
  bool            password;
  bool            localPlayer;
  uint            maxPlayers;
};

struct GameInfo
{
  String name;
  String host;
  String mapName;
  uint   players;
  uint   maxPlayers;
  bool   password;
};

typedef std::vector<GameInfo> GameList;

struct Network
{
  static IMPORT NetConfig config;
  static IMPORT GameList  games;

  enum Event
  {
    Connect = 0,
    Disconnect = 1,
    Snapshot = 2,
    User = 128,
    UserMax = 255
  };

  enum Mask
  {
    ServerToClient = 0x01,
    ClientToServer = 0x02,
    BothWays = 0x03
  };

  static IMPORT String getLastError();

  // Server functions

  static IMPORT bool startGame(const String& name, bool lanOnly = false);

  static IMPORT void stopGame();

  static IMPORT void sendClientCommand(int client, const String& command);

  static IMPORT void sendSnapshot(int client, const void* snapshot);

  static IMPORT String userInfo(int client, const String& key);

  static IMPORT void sendCustomMessage(int client, const void* buffer,
                                       uint size);

  static IMPORT void disconnect(int client);

  // Client functions

  static IMPORT void updateGames(bool lanOnly = false);

  static IMPORT bool connect(const String& name);

  static IMPORT void disconnect();

  static IMPORT void sendSnapshot(const void* snapshot);

  static IMPORT void sendCustomMessage(const void* buffer, uint size);

protected:

  friend class System;

  static void idle();

  static void shutdown();
};

#endif // !NETWORK_H_

// vim: ts=2 sw=2 et
