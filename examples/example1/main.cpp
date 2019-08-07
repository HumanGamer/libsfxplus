#include <iostream>

#include <sfxplus.h>

int main(int argc, const char** argv)
{
    std::cout << "=== Example1 ===" << std::endl;

    std::cout << "Initializing..." << std::endl;
    if (!sfx_startup())
    {
        std::cerr << "Failed to initialize libsfxplus: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    std::cout << "Creating source..." << std::endl;
    SFX_SOURCE source = sfx_create_source();
    if (sfx_getlasterror() != SFX_NO_ERROR)
    {
        std::cerr << "Error when creating source: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    if (argc < 2)
    {
        std::cout << "Error: No audio file specified" << std::endl;
        std::cout << "Usage: example1 <soundfile>" << std::endl;
        sfx_shutdown();
        return 0;
    }

    const char* inputFile = argv[1];
    std::cout << "Loading Audio File..." << std::endl;
    SFX_AUDIO audio = sfx_load_audio(inputFile);
    if (sfx_getlasterror() != SFX_NO_ERROR)
    {
        std::cerr << "Error when playing sound with source: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    std::cout << "Playing Audio File..." << std::endl;
    sfx_source_play_sound(source, audio);
    if (sfx_getlasterror() != SFX_NO_ERROR)
    {
        std::cerr << "Error when playing sound with source: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    std::cout << "Waiting for playback to finish..." << std::endl;
    sfx_source_wait(source);
    if (sfx_getlasterror() != SFX_NO_ERROR)
    {
        std::cerr << "Error when playing sound with source: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    std::cout << "Cleaning up..." << std::endl;
    sfx_shutdown();
    if (sfx_getlasterror() != SFX_NO_ERROR)
    {
        std::cerr << "Error when playing sound with source: " << sfx_errorstring() << std::endl;
        sfx_shutdown();
        return 1;
    }

    return 0;
}
