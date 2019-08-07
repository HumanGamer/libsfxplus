#include <iostream>

#include <sfxplus.h>

int main(int argc, const char** argv)
{
    std::cout << "=== Example1 ===" << std::endl;

    if (!sfx_startup())
    {
        std::cerr << "Failed to initialize libsfxplus: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    std::cout << "Success: " << sfx_errorstring() << std::endl;

    sfx_shutdown();

    return 0;
}
