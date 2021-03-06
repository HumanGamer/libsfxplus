#include <iostream>

#include <sfxplus/sfxplus.h>

#define ERROR_CHECK(msg) \
    if (sfx_error() != SFX_NO_ERROR) \
    { \
        std::cerr << msg << ": " << sfx_error_string() << std::endl; \
        sfx_shutdown(); \
        return 1; \
    }

int main(int argc, const char** argv)
{
    std::cout << "=== Sfx+ Streaming Example ===" << std::endl;

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
        std::cout << "Usage: example1 <soundfile>" << std::endl;
        sfx_shutdown();
        return 0;
    }

    const char* inputFile = argv[1];

    std::cout << "Creating source..." << std::endl;
    SFX_SOURCE source = sfx_source_create(true);
    ERROR_CHECK("Error when creating source");

    std::cout << "Opening audio stream..." << std::endl;
    SFX_STREAM stream = sfx_stream_open(source, inputFile);
    ERROR_CHECK("Error when opening audio stream");

    std::cout << "Playing Audio..." << std::endl;
    sfx_source_play(source);
    ERROR_CHECK("Error when playing audio");

    std::cout << "Waiting for playback to finish..." << std::endl;
    sfx_source_wait(source);
    ERROR_CHECK("Error when waiting for playback to finish");

    std::cout << "Closing stream..." << std::endl;
    sfx_stream_close(stream);
    ERROR_CHECK("Error when closing stream");

    std::cout << "Unloading source" << std::endl;
    sfx_source_destroy(source);
    ERROR_CHECK("Error when unloading source");

    std::cout << "Cleaning up..." << std::endl;
    sfx_shutdown();
    ERROR_CHECK("Error when cleaning up");

    return 0;
}
