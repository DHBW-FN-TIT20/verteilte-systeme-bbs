#ifndef COMMANDIDENTIFIERS_H
#define COMMANDIDENTIFIERS_H

/**
 * @brief Specifies a Enum that describes the different Commands that can be sent from the client to the server.
 */
enum CommandIdentifiers{
  SUBSCRIBE,
  UNSUBSCRIBE,
  PUBLISH,
  LIST_TOPICS,
  GET_TOPIC_STATUS,
};

#endif // COMMANDIDENTIFIERS_H