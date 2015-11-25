//
//  ClientInformation.cpp
//  Server
//
//  Created by Alex Anderson on 8/2/15.
//  Copyright (c) 2015 Alexander. All rights reserved.
//

#include "ClientInformation.h"

ClientInformation::ClientInformation(int ClientFD,std::string name, std::string password, std::string salt)
{
    this->ClientFD = ClientFD;
    this->name = name;
    this->password = password;
    this->salt = salt;
    online = true;
    
    ChatLog = new std::vector<std::string>();
}
ClientInformation::~ClientInformation()
{
}

std::string ClientInformation::GetName()
{
    return name;
}

std::string ClientInformation::GetPass()
{
    return password;
}

std::string ClientInformation::GetSalt()
{
    return salt;
}

int ClientInformation::GetClientFD()
{
    return ClientFD;
}

void ClientInformation::SetClientFD(int ClientFD)
{
    this->ClientFD = ClientFD;
}

std::vector<std::string> ClientInformation::GetChatLog()
{
    std::vector<std::string> TempChatLog = *ChatLog;
    return TempChatLog;
}

bool ClientInformation::SetPass(std::string OldPass, std::string NewPass)
{
    if (OldPass == password)
    {
        password = NewPass;
        return true;
    }
    return false;
}

void ClientInformation::AddToChatLog(std::string sender, std::string recipient, std::string msg)
{
    std::string SingleLog = "Sender: " + sender + " Recipient: " + recipient + " Message: " + msg;
    
    ChatLog->push_back(SingleLog);
}