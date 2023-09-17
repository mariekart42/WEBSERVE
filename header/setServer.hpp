#ifndef SETSERVER_HPP
#define SETSERVER_HPP

#include "connectClients.hpp"

class SetServer
{
    private:
        static int getNewSocketFd(int);
        static void setNewSocketFd(int);
        fdList _fdList;

    public:
        SetServer();
        ~SetServer();

        void setServer();
};

#endif
