#include <iostream>

#include <sfxplus.h>

int main(int argc, const char** argv)
{
    std::cout << "=== Example1 ===" << std::endl;

    if (!sfx_startup())
    {
        int error = sfx_getlasterror();
        const char* errstr = sfx_errorstring(error);
        std::cerr << "Failed to initialize libsfxplus: " << errstr << std::endl;
        sfx_shutdown();
        return 1;
    }

    int error = sfx_getlasterror();
    const char* errstr = sfx_errorstring(error);
    std::cout << "Success: " << errstr << std::endl;

    sfx_shutdown();

    return 0;
}
