#include "setServer.hpp"

int main(int ac, char **av)
{
    std::cout << "Putin is gay" << std::endl;

    SetServer set;
    set.setServer(ac, av);
    std::cout << BOLDGREEN << "\nWebserv successfully terminated." << RESET << std::endl;

    return 0;
}
