#include "Application.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
    
    Lve::Application App;
    
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
