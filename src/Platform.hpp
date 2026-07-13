#pragma once
#include <cstdint>

/**
 * @file Platform.hpp
 * @brief Header file that contains platform-specific definitions and utilities.
 * @author Jody Gyekye
 * @date June 2025
 * @version 1.0
 */

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class Platform
{
public:
    Platform(char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();

    void Update(void const *buffer, int pitch);
    bool ProcessInput(uint8_t *keys);

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
};