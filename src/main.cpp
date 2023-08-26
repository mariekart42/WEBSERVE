#include "../header/main.hpp"

int main(int ac, char **av)
{
    std::cout << "Putin is gay" << std::endl;

    if (ac == 2)
    {
        int port = atoi(av[1]);
        std::cout << "PORT: " << port << std::endl;

        // implement config parser here:
        //  - array OR vector<int>  of Ports ---> eg. portArray[size=3] = {2020, 3030, 4040};

        SetServer obj(port);
        obj.setUpServer();

    }
    else
        exitWithError("./a.out <PORT>");
}
