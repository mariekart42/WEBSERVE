#include "../header/main.hpp"

int main(int ac, char **av)
{
    std::cout << "Putin is gay" << std::endl;

    if (ac == 2)
    {
        int port = atoi(av[1]);
        std::cout << "PORT: " << port << std::endl;

    // DIS IN A LOOP IF MULTIPLE SERVER ARE REQUIRED
        SetServer obj(port);
        obj.setUpServer();

    }
    else
        exitWithError("./a.out <PORT>");


}
