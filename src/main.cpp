#include "Server.h"
#include <iostream>

int main(int argc, char *argv[])
{
    Server * serv = new Server("3810");

    serv->Start();
    
    while(true)
    {
        
    }
    
}