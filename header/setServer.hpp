#ifndef SETSERVER_HPP
#define SETSERVER_HPP

#include "connectClients.hpp"

class SetServer
{
    private:
        int setNewSocketFd(int);
        fdList _fdList;
        int _backlog;

    public:
        SetServer();
        ~SetServer();

        void setServer();
};

#endif
