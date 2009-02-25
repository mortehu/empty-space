#ifndef API_H_
#define API_H_

#include "types.h"
#include "string.h"

/**
 * Namespace for dealing with game commands.
 * \author Morten Hustveit
 * \author Carl Henrik Holth Lunde
 */
struct API
{
  typedef void (*Command)(void);
  typedef bool (*CommandHandler)(void);
  typedef void (*ClientCommandHandler)(int clientNum);

  /**
   * Enable a default set of game commands.
   *
   * The following commands are enabled: bind, bindlist, clear, cmdlist, echo,
   * exec, print, quit, set, seta, sets, setu, toggle, toggleconsole, unbind,
   * unbindall, vstr.
   *
   * Calling this function is not critical to using the rest of the functions in
   * this class.
   */
  static IMPORT void initialize();

  /**
   * Binds a name to a command.
   *
   * After this function is called, the command may be called from the console
   * or bound to a key.  The same command may be bound to several names.  The
   * command can call @ref argv(0) to get the name by which it was called.
   *
   * \param name The name to bind to the command.
   * \param command A pointer to the function that processes the command.
   *
   * \see Input::bind(), argc(), argv(), args()
   */
  static IMPORT void setCommand(const char* name, Command command);

  /**
   * Adds a function that can process commands.
   *
   * This function is different from @ref setCommand() in that the handler can
   * process as many commands as it likes.  For example, you might want to
   * create a function that processes all functions starting with "ui_", and it
   * might be inconvenient, or even impossible, to call setCommand() for each of
   * the possible commands.
   *
   * The command handler must return a bool and take zero arguments.  The
   * return value must be true if, and only if, the command was accepted.
   *
   * \param handler A pointer to the function that can process commands.
   */
  static IMPORT void addCommandHandler(CommandHandler handler);

  /**
   * Sets the function that will process client commands.
   *
   * This function sets what command is to be called when a client command is
   * received (possibly over the network).  Since a host can have multiple
   * players, the index of the player is provided as a parameter.
   *
   * The signature of the handler must be "void handler(int clientNum)".
   *
   * \param handler A pointer to the function that will process client commands.
   */
  static IMPORT void setClientCommandHandler(ClientCommandHandler handler);

  /**
   * Adds a command to the back of the execution queue.
   *
   * \param command The command to be added.
   *
   * \see flushCommands(), executeCommand()
   */
  static IMPORT void appendCommand(const char* command);

  /**
   * Inserts a command in the front of the execution queue.
   *
   * \param command The command to be added.
   *
   * \see flushCommands(), executeCommand()
   */
  static IMPORT void prependCommand(const char* command);

  /**
   * Execute all commands in the execution queue.
   *
   * The execution is halted when a "wait" command is encountered.
   */
  static IMPORT void flushCommands();

  /**
   * Executes a command immediately.
   *
   * \param command The command to be executed.
   *
   * \see addCommand()
   */
  static IMPORT void executeCommand(const char* command);

  /**
   * Execute a client command.
   *
   * The command given may possibly need to travel over a network to reach the
   * computer hosting the given player.
   *
   * \param command The command to be executed.
   * \param clientNum The index of the player for which to execute the command.
   */
  static IMPORT void executeClientCommand(const char* command, int clientNum);

  // Used by command handlers:

  /**
   * Get the amount of arguments for the current (possibly client) command.
   *
   * This function is only useful to command handlers.
   *
   * \see argv(), args()
   */
  static IMPORT uint argc();

  /**
   * Get the contents of the specified argument.
   *
   * Argument 0 is the name through which the command was invoked.  This
   * function is only useful to command handlers.
   *
   * \see argc(), args()
   */
  static IMPORT String argv(uint index);

  /**
   * Get the exact string the command was called with.
   *
   * The string includes all parameters and whitespace.
   *
   * \see argc(), argv()
   */
  static IMPORT String args();
};

#endif // !API_H_

// vim: ts=2 sw=2 et
