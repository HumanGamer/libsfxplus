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

    std::cout << "Initialized!" << std::endl;

    SFX_SOURCE source = sfx_create_source();
    if (sfx_getlasterror() != SFX_NO_ERROR)
    {
        std::cerr << "Error when creating source: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    if (argc < 2)
    {
        std::cout << "No audio file specified..." << std::endl;
        sfx_shutdown();
        return 0;
    }

    const char* inputFile = argv[1];
    SFX_AUDIO audio = sfx_load_audio(inputFile);
    if (sfx_getlasterror() != SFX_NO_ERROR)
    {
        std::cerr << "Error when playing sound with source: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    sfx_source_play_sound(source, audio);
    if (sfx_getlasterror() != SFX_NO_ERROR)
    {
        std::cerr << "Error when playing sound with source: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    sfx_source_wait(source);

    sfx_shutdown();

    return 0;
}
