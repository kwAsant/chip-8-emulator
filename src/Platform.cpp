/**
 * @file Platform.cpp
 * @brief Platform abstraction layer for window and texture management.
 * @author Jody Gyekye
 * @date June 2025
 * @version 1.0
 */

#include "Platform.hpp"
#include <SDL.h>    // include SDL2 for window and input management

#include <unordered_map>

std::unordered_map<SDL_Keycode, uint8_t> keyMap = {
    {SDLK_x, 0}, {SDLK_1, 1}, {SDLK_2, 2}, {SDLK_3, 3}, {SDLK_q, 4}, {SDLK_w, 5}, 
    {SDLK_e, 6}, {SDLK_a, 7}, {SDLK_s, 8}, {SDLK_d, 9}, {SDLK_z, 0xA}, {SDLK_c, 0xB}, 
    {SDLK_4, 0xC}, {SDLK_r, 0xD}, {SDLK_f, 0xE}, {SDLK_v, 0xF}};

// constructor
Platform::Platform(char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2 video subsystem

    // create window, renderer, and texture
    window = SDL_CreateWindow(title,
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            windowWidth,
                            windowHeight,
                            SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                textureWidth,
                                textureHeight);
}
    
// destructor - runs automatically when object goes out of scope (is destroyed)
// its job is to clean up resources
Platform::~Platform()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys)
{
    bool quit = false;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            quit = true;
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            }
            else
            {
                auto it = keyMap.find(event.key.keysym.sym);
                if (it != keyMap.end())
                {
                    // Set key state based on event type
                    keys[it->second] = (event.type == SDL_KEYDOWN) ? 1 : 0;
                }
            }
        }
    }
    return quit;
}