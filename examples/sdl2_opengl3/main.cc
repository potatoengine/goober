// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/core.hh"
#include "goober/draw.hh"
#include "goober/font.hh"
#include "goober/widgets.hh"

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL_opengl.h>
#include <GL/gl.h>

static constexpr char vertexSource[] =
    "#version 330 core\n"
    "layout(location = 0) in vec2 in_pos;\n"
    "layout(location = 1) in vec2 in_uv;\n"
    "layout(location = 2) in vec4 in_rgba;\n"
    "out vec4 attr_rgba;\n"
    "out vec2 attr_uv;\n"
    "void main() {\n"
    "    gl_Position = vec4(in_pos.x / 400.0f - 1, (600.0f - in_pos.y) / 300.0f - 1, 0, 1);\n"
    "    attr_rgba = in_rgba;\n"
    "    attr_uv = in_uv;\n"
    "}\n";
static constexpr int vertexLength = sizeof(vertexSource);

static constexpr char fragmentSource[] =
    "#version 330 core\n"
    "out vec4 out_rgba;\n"
    "in vec4 attr_rgba;\n"
    "in vec2 attr_uv;\n"
    "uniform sampler2D in_tex;\n"
    "void main() {\n"
    //"   vec2 uv = vec2(attr_uv.x, 1.0 - attr_uv.y);\n"
    "   vec2 uv = attr_uv;\n"
    "   float sample = texture(in_tex, uv).r;\n"
    "   out_rgba = attr_rgba * vec4(sample, sample, sample, sample);\n"
    "}\n";
static constexpr int fragmentLength = sizeof(fragmentSource);

static constexpr int vboSize = 4 * 1024 * 1024;
static constexpr int iboSize = 1024 * 1024;

int main(int argc, char* argv[]) {
    Uint32 width = 800;
    Uint32 height = 600;
    SDL_Window* window =
        SDL_CreateWindow("OpenGL Test", 100, 100, width, height, SDL_WINDOW_OPENGL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GLContext glCtx = SDL_GL_CreateContext(window);

    glewInit();

    auto [rs, ctx] = grCreateContext();
    auto [rs2, font] = grCreateDefaultFont(ctx);

    GLuint fontTexture = 0;
    glGenTextures(1, &fontTexture);

    GLuint fontSampler = 0;
    glGenSamplers(1, &fontSampler);
    glSamplerParameteri(fontSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(fontSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(fontSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(fontSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vboSize, nullptr, GL_STREAM_DRAW);

    GLuint ibo = 0;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboSize, nullptr, GL_STREAM_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(grDrawList::Vertex),
        (void*)offsetof(grDrawList::Vertex, pos));
    glEnableVertexArrayAttrib(vao, 0);

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(grDrawList::Vertex),
        (void*)offsetof(grDrawList::Vertex, uv));
    glEnableVertexArrayAttrib(vao, 1);

    glVertexAttribPointer(
        2,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(grDrawList::Vertex),
        (void*)offsetof(grDrawList::Vertex, rgba));
    glEnableVertexArrayAttrib(vao, 2);

    GLuint program = glCreateProgram();
    {
        GLchar const* source = vertexSource;
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &source, &vertexLength);
        glAttachShader(program, vertexShader);

        source = fragmentSource;
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &source, &fragmentLength);
        glAttachShader(program, fragmentShader);

        glCompileShader(vertexShader);
        glCompileShader(fragmentShader);
        glLinkProgram(program);
    }
    GLint texLoc = glGetUniformLocation(program, "in_tex");

    if (grFontAtlas const* atlas = grGetFontAtlasIfDirtyAlpha8(ctx)) {
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        GLenum const format = atlas->bpp == 8 ? GL_RED : GL_RGBA;
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            atlas->width,
            atlas->height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            atlas->data);
        grFontAtlasBindTexture(ctx, fontTexture);
    }

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

        grBeginPortal(ctx, "Test", {{0, 0}, {400, 600}});
        grText(ctx, "hello world!", {40, 40}, grColors::white);
        if (grButton(ctx, "exit", {240, 240}, grColors::darkgrey))
            running = false;
        grImage(ctx, 0, {{200, 300}, {300, 400}}, {{0, 1}, {1, 0}}, grColors::white);

        grEndPortal(ctx);

        grBeginPortal(ctx, "Layout", {{400, 0}, {600, 600}});

        grAddBlockElement(ctx, {20, 20}, grColors::red);
        grBeginBlockElement(ctx, {40, 30}, grColors::blue);
        {
            grAddBlockElement(ctx, {30, 20}, grColors::green);
            grAddInlineElement(ctx, {10, 10}, grColors::cyan);
            grAddInlineElement(ctx, {10, 12}, grColors::magenta);
            grAddInlineElement(ctx, {10, 10}, grColors::cyan);
            grAddInlineElement(ctx, {10, 12}, grColors::magenta);
            grAddInlineElement(ctx, {10, 10}, grColors::cyan);
            grAddInlineElement(ctx, {5, 20}, grColors::yellow);
            grAddInlineElement(ctx, {10, 12}, grColors::magenta);
            grAddInlineElement(ctx, {10, 10}, grColors::cyan);
            grAddInlineElement(ctx, {10, 12}, grColors::magenta);
            grAddInlineElement(ctx, {10, 10}, grColors::cyan);
            grAddInlineElement(ctx, {10, 12}, grColors::magenta);
            grAddInlineElement(ctx, {10, 10}, grColors::cyan);
            grAddInlineElement(ctx, {10, 12}, grColors::magenta);
            grAddInlineElement(ctx, {5, 20}, grColors::yellow);
            grAddInlineElement(ctx, {10, 10}, grColors::cyan);
            grAddInlineElement(ctx, {10, 12}, grColors::magenta);
            grAddInlineElement(ctx, {10, 10}, grColors::cyan);
            grAddInlineElement(ctx, {10, 12}, grColors::magenta);
        }
        grEndElement(ctx);
        grAddBlockElement(ctx, {60, 10}, grColors::black);

        grEndPortal(ctx);

        grRenderElements(ctx);
        grEndFrame(ctx);

        glViewport(0, 0, width, height);
        glClearColor(0.3f, 0.3f, 0.3f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glUseProgram(program);
        glUniform1i(texLoc, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindSampler(0, fontSampler);
        glEnable(GL_SCISSOR_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);

        for (grPortal* port : ctx->portals) {
            grDrawList const& draw = *port->draw;
            if (draw.commands.empty())
                continue;

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(
                GL_ARRAY_BUFFER,
                0,
                draw.vertices.size() * sizeof(grDrawList::Vertex),
                draw.vertices.data());

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferSubData(
                GL_ELEMENT_ARRAY_BUFFER,
                0,
                draw.indices.size() * sizeof(uint16_t),
                draw.indices.data());

            for (grDrawList::Command const& cmd : draw.commands) {
                glBindTexture(
                    GL_TEXTURE_2D,
                    cmd.textureId == 0 ? ctx->fontAtlas->texture : cmd.textureId);
                glBindSampler(0, fontSampler);
                glDrawElements(
                    GL_TRIANGLES,
                    cmd.indexCount,
                    GL_UNSIGNED_SHORT,
                    (void*)(cmd.indexStart * sizeof(uint16_t)));
            }
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        SDL_GL_SwapWindow(window);
    }
    return 0;
}
