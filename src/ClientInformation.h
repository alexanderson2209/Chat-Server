//
//  ClientInformation.h
//  Server
//
//  Created by Alex Anderson on 8/2/15.
//  Copyright (c) 2015 Alexander. All rights reserved.
//
#ifndef __Server__ClientInformation__
#define __Server__ClientInformation__

#include <stdio.h>
#include <string>
#include <vector>

/*!
 \class ClientInformation
 
 \brief ClientInformation holds information about a single client.
 
 \author $ Author: Alex Anderson $
 */
class ClientInformation
{
private:
    /**
     *  File Descriptor for client socket communication
     */
    int ClientFD;
    
    /**
     *  Clients username for server authentication
     */
    std::string name;
    
    /**
     *  Clients password for server authentication
     */
    std::string password;
    
    /**
     *  Salt for client password encryption
     */
    std::string salt;
    
    /**
     *  Contains all msg commands sent/received
     */
    std::vector<std::string> * ChatLog;
    
public:
    /**
     *  @brief Ctor for ClientInformation
     *  @param name Client name
     *  @param password Client password
     *  @param ClientFD Client file descriptor
     */
    ClientInformation(int ClientFD, std::string name, std::string password, std::string salt);
    ~ClientInformation();
    
    /**
     *  Online status
     *  <br>true = online
     *  <br>false = offline
     */
    bool online;
    
    /**
     *  Client username getter
     *
     *  @return Client username
     *
     */
    std::string GetName();
    
    /**
     *  Client password getter
     *
     *  @return Client password
     */
    std::string GetPass();
    
    /**
     *  Client salt getter
     *
     *  @return Client password's salt
     */
    std::string GetSalt();
    
    /**
     *  Client file descritor getter
     *
     *  @return Client file descriptor
     */
    int GetClientFD();
    
    /**
     *  Client file descriptor setter
     *
     *  @param ClientFD New client file descriptor
     */
    void SetClientFD(int ClientFD);
    
    /**
     *  Returns a vector containing all sent/received msg commands
     *
     *  @return a vector containing all sent/received msg commands
     */
    std::vector<std::string> GetChatLog();
    
    /**
     *  Sets password for client
     *  <br>Does some authentication where it checks if client knows old password
     *
     *  @param OldPass Password currently stored for client
     *  @param NewPass New password
     *
     *  @return true = successful password change
     */
    bool SetPass(std::string OldPass, std::string NewPass);
    
    /**
     *  Adds new entry to the clients chat log
     *  <br>Example chat log entry: "Sender: Sender Recipient: Recipient Message: Msg"
     *
     *  @param Sender    Who sent the message
     *  @param Recipient Who received the message
     *  @param Msg       Message content that was sent/received
     */
    void AddToChatLog(std::string Sender, std::string Recipient, std::string Msg);
};

#endif /* defined(__Server__ClientInformation__) */
