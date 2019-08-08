#include <iostream>

#include <sfxplus.h>

#define ERROR_CHECK(msg) \
    if (sfx_error() != SFX_NO_ERROR) \
    { \
        std::cerr << msg << ": " << sfx_error_string() << std::endl; \
        sfx_shutdown(); \
        return 1; \
    }

int main(int argc, const char** argv)
{
    std::cout << "=== Sfx+ Playing Example ===" << std::endl;

    std::cout << "Initializing..." << std::endl;
    if (!sfx_startup())
    {
        std::cerr << "Failed to initialize libsfxplus: " << sfx_error_string() << std::endl;
        sfx_shutdown();
        return 1;
    }

    if (argc < 2)
    {
        std::cout << "Error: No audio file specified" << std::endl;
        std::cout << "Usage: playing <soundfile>" << std::endl;
        sfx_shutdown();
        return 0;
    }

    const char* inputFile = argv[1];

    std::cout << "Creating source..." << std::endl;
    SFX_SOURCE source = sfx_source_create();
    ERROR_CHECK("Error when creating source");

    std::cout << "Loading Audio File..." << std::endl;
    SFX_AUDIO audio = sfx_audio_load(inputFile);
    ERROR_CHECK("Error when loading sound");

    std::cout << "Playing Audio File..." << std::endl;
    sfx_source_play_sound(source, audio);
    ERROR_CHECK("Error when playing sound with source");

    std::cout << "Waiting for playback to finish..." << std::endl;
    sfx_source_wait(source);
    ERROR_CHECK("Error when waiting for playback to finish");

    std::cout << "Unloading audio" << std::endl;
    sfx_audio_destroy(audio);
    ERROR_CHECK("Error when unloading audio");

    std::cout << "Unloading source" << std::endl;
    sfx_source_destroy(source);
    ERROR_CHECK("Error when unloading source");

    std::cout << "Cleaning up..." << std::endl;
    sfx_shutdown();
    ERROR_CHECK("Error when cleaning up");

    return 0;
}
