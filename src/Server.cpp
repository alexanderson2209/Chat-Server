//
//  Server.cpp
//  Server
//
//  Created by Alex Anderson on 7/18/15.
//  Copyright (c) 2015 Alexander. All rights reserved.
//

/*
 TODO:
 Fix Bugs
 */

#include <openssl/rand.h>

#include "Server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <stdlib.h>
#include <algorithm>
#include <unistd.h>

#define MAXDATASIZE 100
#define DELIMETER "\r\n"
#define GlobalSalt "A08hfH888--9990][--=.asef24kin2488932npppmu2bflslu3ufb12w1s12e234t"

Server::Server(std::string port, std::string IP)
{
    Server::StillSending = true;
    Server::port = port;
    Server::IP = IP;
    Clients = new std::unordered_map<std::string, ClientInformation>;
}

Server::Server(std::string port)
{
    Server::StillSending = true;
    Server::port = port;
    Clients = new std::unordered_map<std::string, ClientInformation>;
}

Server::Server()
{
    Server::StillSending = true;
    Server::port = "3810";
    Clients = new std::unordered_map<std::string, ClientInformation>;
}

Server::~Server()
{
    delete Clients;
}

int Server::Start()
{
    struct addrinfo hints, *res;
    int sockfd;
    int status;
    socklen_t addr_size;
    struct sockaddr_storage their_addr;
    int yes = 1;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Sets which IP version you use. AF_UNSPEC is for either
    

    hints.ai_socktype = SOCK_STREAM; //This socket is going to be a stream type(SOCK_STREAM is TCP socket, UDP is SOCK_DGRAM)
    
    if (IP.size() == 0)
    {
        hints.ai_flags = AI_PASSIVE; // use my IP
        if ((status = getaddrinfo(NULL, port.c_str(), &hints, &res)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
            return 2;
        }
    }
    else
    {
        if ((status = getaddrinfo(IP.c_str(), port.c_str(), &hints, &res)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
            return 2;
        }
    }
    
    
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
        {
            perror("server: socket");
        }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    
    
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("Socket failed to bind to port.");
    }
    
    if (listen(sockfd, 15) == -1)
    {
        perror("Could not listen on port.");
    }
    
    addr_size = sizeof their_addr;

    int (Server::*AcceptClientConnectionsPointer) (int);
    AcceptClientConnectionsPointer = &Server::AcceptClientConnections;
    
    std::thread t1(&Server::AcceptClientConnections, this, sockfd);
    t1.detach();
    
    std::thread t2(&Server::ProcessOutgoingMessages, this);
    t2.detach();
    
    return 0;
}

int Server::AcceptClientConnections(int sockfd)
{
    while (1)
    {
        int ClientFD;
        struct sockaddr_storage their_addr;
        socklen_t addr_size;
        
        addr_size = sizeof their_addr;
        
        ClientFD = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        if (ClientFD == -1) {
            perror("accept");
            continue;
        }
        
        std::thread t2(&Server::ClientSetUp, this, ClientFD);
        t2.detach();
    }
    return 0;
}

int Server::SendEntireMessage(std::string msg, int ClientFD)
{
    msg.append(DELIMETER);
    long MsgRemaining;
    
    bool LoopControl = false;
    while (!LoopControl)
    {
        long SentAmount = send(ClientFD, msg.c_str(), msg.length(), 0);
        
        MsgRemaining = msg.length() - SentAmount;
        
        if (MsgRemaining == 0)
        {
            //message done sending
            LoopControl = true;
        }
        else if (MsgRemaining < 0)
        {
            //error occured
            return -1;
        }
        else
        {
            msg = msg.substr(MsgRemaining, msg.npos);
        }
    }
    return 0;
}

int Server::ClientSetUp(int ClientFD)
{
    SendEntireMessage("connected 0", ClientFD);
    
    std::string message;
    
    if ((ReceiveSingleMessage(ClientFD, message) == 1))
    {
        close(ClientFD);
        return -1;
    }
    
    if (CommandValidation(2, message))
    {
        message = message.substr(6, message.npos);
        long UserIndex = message.find(" ");
        
        std::string user = message.substr(0, UserIndex);
        std::string password = message.substr(UserIndex + 1, message.npos);
        
        //Authenticate user and password
        std::unordered_map<std::string, ClientInformation>::iterator it;
        
        if ((it = Clients->find(user)) == Clients->end())
        {
            std::string salt = GenerateSalt(password.length());
            Clients->insert({user, ClientInformation(ClientFD, user, GenerateEncString(password, salt), salt)});
            SendEntireMessage("authenticated 0", ClientFD);
        }
        else
        {
            std::string LookAtMe1 = GenerateEncString(password, it->second.GetSalt());
            std::string LookAtMe2 = it->second.GetPass();
            if (GenerateEncString(password, it->second.GetSalt()) != it->second.GetPass())
            {
                SendEntireMessage("authenticated 1", ClientFD);
                ClientSetUp(ClientFD);
                return 1;
            }
            else if (it->second.online == true)
            {
                SendEntireMessage("authenticated 1", ClientFD);
                SendEntireMessage("error 4 User already logged in", ClientFD);
                ClientSetUp(ClientFD);
                return 1;
            }
            it->second.online = true;
            it->second.SetClientFD(ClientFD);
            SendEntireMessage("authenticated 0", ClientFD);
        }
        
        BroadcastOnlineUsers();
        ReceiveMessages(user);
    }
    else
    {
        SendEntireMessage("error 2 Unexpected Command. Expected login command.", ClientFD);
        ClientSetUp(ClientFD);
        return 1;
    }
    
    return 0;
}

void Server::BroadcastOnlineUsers()
{
    //Get login of all clients online and populate clients
    std::vector<std::string> OnlineClients;
    std::string OnlineClientsMessage = "online";
    
    for (auto it =Clients->begin(); it != Clients->end(); it++)
    {
        if (it->second.online == true)
        {
            OnlineClients.push_back(it->second.GetName());
            OnlineClientsMessage.append(" " + it->second.GetName());
        }
    }
    
    for (std::vector<std::string>::iterator it = OnlineClients.begin(); it != OnlineClients.end(); ++it)
    {
        SendEntireMessage(OnlineClientsMessage, Clients->at(*it).GetClientFD());
    }
}

long Server::GetIndexOfDelimiter(char * array, long length)
{
    char * match = std::strstr(array, DELIMETER);
    
    if (match == NULL)
    {
        return -1;
    }
    else
    {
        return match - array;
    }
}

int Server::ReceiveSingleMessage(int ClientFD, std:: string & message)
{
    while (true)
    {
        char buffer[MAXDATASIZE];
        long ReceivedAmount;
        ReceivedAmount = recv(ClientFD, buffer, MAXDATASIZE, 0);
        
        if (ReceivedAmount == 0)
        {
            return 1;
        }
        
        long index;
        if ((index = GetIndexOfDelimiter(buffer, ReceivedAmount)) == -1)
        {
            std::string temp(buffer, ReceivedAmount);
            message.append(temp);
            continue;
        }
        else
        {
            message.append(buffer, index);
            return 0;
        }
    }
}

void Server::ReceiveMessages(std::string username)
{
    while (true)
    {
        char buffer[MAXDATASIZE];
        std::string PartialMessage;
        long ReceivedAmount = recv(Clients->at(username).GetClientFD(), buffer, MAXDATASIZE, 0);
        
        if (ReceivedAmount == 0 || ReceivedAmount == -1)
        {
            Clients->at(username).online = false;
            close(Clients->at(username).GetClientFD());
            BroadcastOnlineUsers();
            break;
        }
        
        long index;
        
        while ((index = GetIndexOfDelimiter(buffer, ReceivedAmount)) == -1)
        {
            PartialMessage.append(buffer, ReceivedAmount);
            ReceivedAmount = recv(Clients->at(username).GetClientFD(), buffer, MAXDATASIZE, 0);
            
            if (ReceivedAmount == 0 || ReceivedAmount == -1)
            {
                Clients->at(username).online = false;
                close(Clients->at(username).GetClientFD());
                BroadcastOnlineUsers();
                break;
            }
        }
        
        if (!Clients->at(username).online)
        {
            break;
        }
        
        PartialMessage.append(buffer, index);
        
        if (CommandValidation(1, PartialMessage))
        {
            Server::Message msg;
            msg.sender = username;
            msg.message = PartialMessage;
            OutgoingMessages.push(msg);
        }
        else if (CommandValidation(3, PartialMessage))
        {
            Clients->at(username).online = false;
            close(Clients->at(username).GetClientFD());
            BroadcastOnlineUsers();
            break;
        }
        else
        {
           SendEntireMessage("error 2 Unexpected Command. Expected msg or disconnect command.", Clients->at(username).GetClientFD());
        }
    }
}

void Server::ProcessOutgoingMessages()
{
    while(StillSending)
    {
        if (OutgoingMessages.size() > 0)
        {
            Message msg = OutgoingMessages.front();
            OutgoingMessages.pop();
            
            msg.message = msg.message.substr(4, msg.message.npos);
            
            std::string recipient = msg.message.substr(0, msg.message.find(" "));
            
            std::string message = msg.message.substr(msg.message.find(" ") + 1, msg.message.npos);
            
            if (Clients->find(recipient) == Clients->end())
            {
                SendEntireMessage("error 3 Recipient does not exist", Clients->at(msg.sender).GetClientFD());
            }
            else if (Clients->at(recipient).online == false)
            {
                SendEntireMessage("error 1 Recipient Offline", Clients->at(msg.sender).GetClientFD());
            }
            else
            {
                SendEntireMessage("msg " + msg.sender + " " + message, Clients->at(recipient).GetClientFD());
            }
        }
    }
}
/*
 Validates commands sent to/from server
 
 Command        int
 msg            1
 login          2
 disconnect     3
 */
bool Server::CommandValidation(int commandInt, std::string command)
{
    /** @bug Clients can login with "login user " which isnt valid */
    //Checks that msg and login command have 2 spaces
    if (commandInt == 1 || commandInt == 2)
    {
        std::string sub = " ";
        int count = 0;
        
        size_t pos = command.find(sub, 0);
        while (pos != command.npos) {
            count++;
            pos = command.find(sub, pos+1);
        }
        
        if (count < 2) return false;
    }
    
    //checks command for valid starting substring and valid minimum size
    switch (commandInt) {
        case 1:
            if (!(command.substr(0, 4) == "msg ")) return false;
            if (command.length() < 7) return false;
            return true;
        case 2:
            if (!(command.substr(0, 6) == "login ")) return false;
            if (command.length() < 8) return false;
            return true;
        case 3:
            if (!(command.substr(0, 10) == "disconnect")) return false;
            if (command.length() != 10) return false;
            return true;
        default:
            return false;
    }
}

std::string Server::GenerateSalt(long saltLength)
{
    static const char charPool[] =
    "01346789*-="
    "bdefgijklmnpqrstuvwxz"
    "ABCEFGHJKLNOPQRTUVWXY";
    
    saltLength = saltLength * 3 - (saltLength / 2);
    
    std::string salt;
    
    for (int i = 0; i < saltLength; i++)
    {
        salt = salt + charPool[rand() % (sizeof(charPool) - 1)];
    }
    
    return salt;
}