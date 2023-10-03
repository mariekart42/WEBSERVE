#ifndef SETSERVER_HPP
#define SETSERVER_HPP

#include "connectClients.hpp"

class configParser;

class SetServer
{
    private:
        fdList  _fdList;
        int     _backlog;
        int     setNewSocketFd(int) const;

    public:
        SetServer();
        ~SetServer();

        void    setServer(int, char**);
};

#endif
