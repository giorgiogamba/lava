#include "Application.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
    
    Lava::Application App;
    
    try
    {
        App.Run();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return 0;
};
