#include <iostream>

#include <sfxplus.h>

int main(int argc, const char** argv)
{
    std::cout << "=== Example1 ===" << std::endl;

    if (!sfx_init())
    {
        std::cerr << "Failed to initialize libsfxplus" << std::endl;
        return 1;
    }

    std::cout << "Success." << std::endl;

    if (!sfx_unload())
    {
        std::cerr << "Failed to unload libsfxplus" << std::endl;
        return 2;
    }

    return 0;
}
