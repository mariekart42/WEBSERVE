#ifndef SETSERVER_HPP
#define SETSERVER_HPP

#include "connectClients.hpp"

class SetServer
{
    private:
        static int getNewServerSocket(int);
        void initServerSocket(int);

    public:
        SetServer();
        ~SetServer();

    std::vector<int> setUpServer();
};

#endif
