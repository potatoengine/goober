// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/core.hh"
#include "goober/widgets.hh"

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

        {
            int x = 0, y = 0;
            Uint32 const buttons = SDL_GetMouseState(&x, &y);
            ctx->mousePos = {static_cast<float>(x), static_cast<float>(y)};
            ctx->mouseButtons = {};
            if (0 != (buttons & SDL_BUTTON_LEFT))
                ctx->mouseButtons = ctx->mouseButtons | grButtonMask::Left;
            if (0 != (buttons & SDL_BUTTON_RIGHT))
                ctx->mouseButtons = ctx->mouseButtons | grButtonMask::Right;
            if (0 != (buttons & SDL_BUTTON_MIDDLE))
                ctx->mouseButtons = ctx->mouseButtons | grButtonMask::Middle;
        }

        grBeginFrame(ctx, 0.f);

        if (grButton(ctx, "exit", {240, 240, 300, 280}, grColors::darkgrey))
            running = false;

        grEndFrame(ctx);

        if (grIsMouseOver(ctx, {20, 20, 190, 190}) && grIsMouseDown(ctx, grButtonMask::Left))
            running = false;

        glViewport(0, 0, width, height);
        glClearColor(0.f, 0.3f, 0.6f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, 0, 1.f);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        for (auto const& cmd : ctx->draw.commands) {
            glVertexPointer(
                2,
                GL_FLOAT,
                sizeof(grDrawList::Vertex),
                &ctx->draw.vertices.front().pos);
            glColorPointer(
                4,
                GL_UNSIGNED_BYTE,
                sizeof(grDrawList::Vertex),
                &ctx->draw.vertices.front().rgba);
            glDrawElements(
                GL_TRIANGLES,
                cmd.indexCount,
                GL_UNSIGNED_SHORT,
                &ctx->draw.indices[cmd.indexStart]);
        }
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

        SDL_GL_SwapWindow(window);
    }
    return 0;
}
