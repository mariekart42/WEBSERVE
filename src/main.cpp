#include "setServer.hpp"

int main(int ac, char **av)
{
    std::cout << "Putin is gay" << std::endl;

    SetServer set;
    if (!set.setServer(ac, av))
        return 1;

    std::cout << BOLDGREEN << "\nWebserv successfully terminated." << RESET << std::endl;

    return 0;
}
