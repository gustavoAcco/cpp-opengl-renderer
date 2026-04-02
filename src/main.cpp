#include "core/Application.hpp"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

int main()
{
    try {
        Application app;
        app.run();
    } catch (const std::runtime_error& e) {
        std::fprintf(stderr, "Fatal error: %s\n", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
