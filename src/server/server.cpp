#include "server.h"
#include "../shared/command.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>

using namespace std;

void Server::handleApproachingClient(int clientSocket)
{
    char buffer[1024];

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0)
    {
        cerr << "Error receiving data from client" << endl;
        close(clientSocket);
        return;
    }
    // Process the received data
    // TODO remove for Production
    string receivedData(buffer, bytesRead);
    cout << "Received from client: " << receivedData << std::endl;

    // deserialize the received data
    // try catch to catch the exception if the string is not a valid json
    Command command;
    try
    {
        command = Command::deserialize(receivedData);
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        // TODO Return internal server error
    }
    
    Response response;

    switch (command.commandIdentifier)
    {
    case CommandIdentifiers::subscribe:
        response = this->handleSubsscribeRequest(clientSocket, command.getCommandArgument(CommunicationParameters::topicName));
        break;
    case CommandIdentifiers::unsubscribe:
        response = this->handleUnsubscribeRequest(clientSocket, command.getCommandArgument(CommunicationParameters::topicName));
        break;
    case CommandIdentifiers::listTopics:
        response = this->handleListTopics();
        break;
    case CommandIdentifiers::getTopicStatus:
        response = this->handleGetTopicStatus(command.getCommandArgument(CommunicationParameters::topicName)); 
        break;
    default:
        break;
    }

    // serialize the response
    string serializedResponse = response.serialize();

    // send the response to the client
    int bytesSent = send(clientSocket, serializedResponse.c_str(), serializedResponse.length(), 0);
    if (bytesSent != serializedResponse.length())
    {
        cerr << "Error sending data to client" << endl;
        close(clientSocket);
        return;
    }

    // close the connection
    close(clientSocket);
}

void Server::startServer(int port, int topicTimeout)
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        return;
    }

    int backlog = 5; // Maximum number of pending connections
    if (listen(serverSocket, backlog) < 0)
    {
        std::cerr << "Error listening for connections" << std::endl;
        return;
    }

    std::vector<std::thread> clientThreads;

    while (true)
    {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0)
        {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        // Create a new thread to handle the approaching client connection
        std::thread threadObj(&Server::handleApproachingClient, this, clientSocket);
        clientThreads.push_back(std::move(threadObj));
    }

    // Code wont be executed

    // //Join all the client threads
    // for (auto &threadObj : clientThreads)
    // {
    //   threadObj.join();
    // }

    // close(serverSocket);
}

Server::Server(int port, int topicTimeout)
{
    cout << "Starting server on port " << port << " with topic timeout of " << topicTimeout << " seconds." << endl;

    // Start the Server in a while loop
    startServer(port, topicTimeout);
}

Server::~Server()
{
    cout << "Stopping server" << endl;
}

Response Server::handleSubsscribeRequest(int socket, string topicName)
{
    // TODO
}

Response Server::handleUnsubscribeRequest(int socket, string topicName)
{
    // TODO
}

Response Server::handleListTopics()
{
    // TODO
}

Response Server::handleGetTopicStatus(string topicName)
{
    // TODO
}