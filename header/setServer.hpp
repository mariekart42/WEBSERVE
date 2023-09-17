#ifndef SETSERVER_HPP
#define SETSERVER_HPP

#include "connectClients.hpp"

class configParser;

class SetServer
{
    private:
        int setNewSocketFd(int) const;
        fdList _fdList;
        int _backlog;

    public:
        SetServer();
        ~SetServer();

        void setServer(int, char**);
};

#endif
