// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/core.hh"

#include <SDL.h>
#include <SDL_main.h>
#include <SDL_opengl.h>
#include <GL/gl.h>

int main(int argc, char* argv[]) {
    Uint32 width = 800;
    Uint32 height = 600;
    SDL_Window* window =
        SDL_CreateWindow("OpenGL Test", 100, 100, width, height, SDL_WINDOW_OPENGL);
    SDL_GLContext glCtx = SDL_GL_CreateContext(window);

    grAllocator allocator;
    auto [rs, ctx] = grCreateContext(allocator);

    bool running = true;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        grBeginFrame(ctx, 0.f);
        grEndFrame(ctx);

        glViewport(0, 0, width, height);
        glClearColor(1.f, 0.f, 1.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }
    return 0;
}
