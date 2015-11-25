////
////  main.cpp
////  Server
////
////  Created by Alex Anderson on 7/12/15.
////  Copyright (c) 2015 Alexander. All rights reserved.
////
//
///*
// ** showip.c -- show IP addresses for a host given on the command line
// */
//
//#include <stdio.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
//
////int main(int argc, char *argv[])
//{
//    /*
//     Basic form of a server in c++
//     getaddrinfo();
//     socket();
//     bind();
//     listen();
//     accept();
//     */
//    
//    
//    //getaddrinfo() portion
//    
//    /*
//     hints--partially filled addrinfo. Usually fed to getaddrinfo()
//     res--Finished product filled out by getaddrinfo()
//     p--temporary addrinfo used for iteration
//     */
//    struct addrinfo hints, *res;
//    
//    int status; //Used for error checking. If status != 0, uh oh
//    int sockfd; //General socket used to bind to a local port usually.
//    socklen_t addr_size; //Size of address
//    
//    //*****
//    struct sockaddr_storage their_addr; //I dont know yet
//    //*****
//    
//    memset(&hints, 0, sizeof hints); //Sets memory to all 0's
//    hints.ai_family = AF_UNSPEC; // Sets which IP version you use. AF_UNSPEC is for either
//    hints.ai_socktype = SOCK_STREAM; //This socket is going to be a stream type(SOCK_STREAM is TCP socket, UDP is SOCK_DGRAM)
//    hints.ai_flags = AI_PASSIVE; // use my IP
//    
//    /*
//     This step does some nifty error reporting alongside filling out the addrinfo res.
//     It uses addrinfo hints as a template.
//     argv[1] is the first argument in this demo. (It is the address "192.167.0.1 www.google.com etc)
//     the NULL portion is the port if needed
//     */
//    if ((status = getaddrinfo(NULL, "3810", &hints, &res)) != 0) {
//        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
//        return 2;
//    }
//    
//    //end getaddrinfo() portion
//    
//    
//    
//    
//    
//    //socket(), bind(), listen() and accept() portion
//    
//    //Makes a socket descriptor with the specified settings
//    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
//    
//    //Binds a socket to a specific address.
//    //ai_addr contains information about your address, namely, port and IP address
//    bind(sockfd, res->ai_addr, res->ai_addrlen);
//    
//    //listens for incoming connections on the port specified in the bind() call
//    listen(sockfd, 5);
//    
//    //address size of incoming connection. IPV6 for future proofing
//    addr_size = sizeof their_addr;
//    
//    //Specific socket descriptor for the client/server connection
//    int clientSockFD;
//    
//    while(1)
//    {
//        clientSockFD = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
//        if (clientSockFD == -1) {
//            perror("accept");
//            continue;
//        }
//        if (send(clientSockFD, "Hello, world!", 13, 0) == -1)
//            perror("send");
//    }
//    //accepts incoming client connection. New socket discriptor is clientSockFD
//    
//    //end socket(), bind(), listen() and accept() portion
//    
//    return 0;
//}
//
//
