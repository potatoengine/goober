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
        ctx->draw.drawRect({10, 10}, {200, 200}, grColors::darkgrey);
        grEndFrame(ctx);

        glViewport(0, 0, width, height);
        glClearColor(0.f, 0.3f, 0.6f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, 0, 1.f);

        for (auto const& cmd : ctx->draw.commands) {
            glBegin(GL_TRIANGLES);
            for (grDrawList::Offset index = cmd.indexStart, end = index + cmd.indexCount;
                 index != end;
                 ++index) {
                auto const& vert = ctx->draw.vertices[ctx->draw.indices[index]];
                glColor4f(
                    vert.rgba.r / 255.0f,
                    vert.rgba.g / 255.0f,
                    vert.rgba.b / 255.0f,
                    vert.rgba.a / 255.0f);
                glVertex2f(vert.pos.x, vert.pos.y);
            }
            glEnd();
        }

        SDL_GL_SwapWindow(window);
    }
    return 0;
}
