//
//  Server.h
//  Server
//
//  Created by Alex Anderson on 7/18/15.
//  Copyright (c) 2015 Alexander. All rights reserved.
//

#ifndef __Server__Server__
#define __Server__Server__

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <queue>
#include "ClientInformation.h"

/** @class Server
 *  @brief A simple multithreaded chat server. Handles client connections, login authentication, and it has chat log support.
 *  To start server, initialize it with IP and port you desire, and use the Start() method.
 */
class Server
{
public:
    /**
     *  @brief Server Ctor.
     *
     *  @param port Port that the server will run on.
     *  @param IP Specified IP that the server will run from.
     *
     */
    Server(std::string port, std::string IP);
    /**
     *  @brief Server Ctor. Uses default IP address of the computer running it.
     *
     *  @param port Port that the server will run on.
     */
    Server(std::string port);
    
    /**
     *  @brief Default server Ctor. Uses default IP address of the computer running it.
     *  It uses a default port value of 3810.
     */
    Server();
    
    ~Server();
    
    /**
     *  Starts server and begins accepting connections.
     *
     *  @return 0 if everything went ok, -1 otherwise.
     */
    int Start();
    
    /**
     *  Stops server, closes all connections, and will free any memory associated with it.
     *
     *  @return 0 if everything went ok, -1 otherwise.
     */
    int Stop();
private:
    /**
     *  Struct for bundling up the sender and message. It is primarily used for the outgoing messages queue.
     */
    struct Message
    {
        std::string sender;
        std::string message;
    };
    
    bool StillSending; ///< Trigger to allow server to stop sending messages.
    std::string port; ///< Port server uses to accept incoming data
    std::string IP; ///< IP address server uses. Usually a default value.
    
    /**
     *  Holds all information about the clients.
     *  It is a key/value pair with the key being the clients username and the value being an instance of
     *  the class ClientInformation.
     *
     *  ClientInformation holds:
     *  username
     *  password
     *  ClientFD -Used to commincate to client-
     *  ChatLog -Contains all client messages received/sent-
     */
    std::unordered_map<std::string, ClientInformation> * Clients;
    
    /**
     *  Queue of messages that currently need to be sent out. The private method ProcessOutgoingMessages handles
     *  the sending of these messages.
     */
    std::queue<Message> OutgoingMessages;
    
    int AcceptClientConnections(int sockfd); ///< Accepts clients as they connect
    
    /**
     *  Sends pieces of msg until all messages are sent
     *
     *  @param msg      Message to be sent.
     *  @param ClientFD The recipient of the message.
     *
     *  @return 0 if everything went ok, -1 otherwise.
     */
    int SendEntireMessage(std::string msg, int ClientFD);
    
    /**
     *  Gets index of delimiter in a char array. Needs length param because it is usually sent a buffer 
     *  thats only partially filled with usuable data.
     *
     *  @param array  The array looked through
     *  @param length length of the arrays usable data.
     *
     *  @return Returns index of DELIMITER, -1 if its not found.
     */
    long GetIndexOfDelimiter(char * array, long length);
    
    /**
     *  Receives a single message from the user associated with ClientFD. Primarily used in conjunction with ClientSetUp()
     *
     *  @param ClientFD File Descriptor of the user.
     *  @param message  string reference that will hold the message thats received.
     *
     *  @return 0 if everything went ok, -1 otherwise.
     */
    int ReceiveSingleMessage(int ClientFD, std:: string & message);
    
    /**
     *  Once client is accepted, ClientSetUp() is executed. This method handles login information. Clients will send their
     *  username and password and start sending/receiving data.
     *
     *  @param ClientFD File Descriptor of the user.
     *
     *  @return 0 if everything went ok, -1 otherwise.
     */
    int ClientSetUp(int ClientFD);
    
    /**
     *  Receive multiple messages sent by the user. It will receive messages and put them in the OutgoingMessages queue.
     *  It will continue to receive messages until the server stops or the client disconnects.
     *
     *  @param username Username of the client.
     */
    void ReceiveMessages(std::string username);
    
    /**
     *  Processes all OutgoingMessages. If no messages are in the OutgoingMessages queue, this method will wait for
     *  OutgoingMessages to be populated.
     */
    void ProcessOutgoingMessages();
    
    /**
     *  Handles validation of all commands being sent to the server. It finds incorrect messages and handles them by returning
     *  true/false.
     *
     *  Command        int
     *  msg            1
     *  login          2
     *  disconnect     3
     *
     *  @param commandInt Flag for specific command.
     *  @param command    Command passed to be validated.
     *
     *  @return True if valid, false if invalid.
     */
    bool CommandValidation(int commandInt, std::string command);
    
    /**
     *  Gets all users online and broadcasts message to all users.
     *
     *  Sample Message: "online user1 user2 user3 user4 user5"
     */
    void BroadcastOnlineUsers();
    
    std::string GenerateSalt(long saltLength);
    std::string GenerateEncString(std::string input, std::string salt);
};

#endif /* defined(__Server__Server__) */
