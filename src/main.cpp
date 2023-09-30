#include "../header/main.hpp"

int main(int ac, char **av)
{
    std::cout << "Putin is gay" << std::endl;

//    Config conf;
//    if (conf.startServer)
//    {


        SetServer set;
        set.setServer(ac, av);
        std::cout << BOLDGREEN << "\nWebserv successfully terminated." << RESET << std::endl;
//    }
//    else
//        std::cout << conf.getErrorMsg() << std::endl;
    return 0;
}
 