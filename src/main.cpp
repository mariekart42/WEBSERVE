#include "../header/main.hpp"

int main(void)
{
    std::cout << "Putin is gay" << std::endl;

//    Config conf;
//    if (conf.startServer)
//    {
        SetServer set;
        ConnectClients connect(set.setUpServer());
        connect.connectClients();
//    }
//    else
//        std::cout << conf.getErrorMsg() << std::endl;
    return 0;
}
