// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_FONT_HH_)
#define GOOBER_FONT_HH_
#pragma once

#include "core.hh"

inline namespace goober {
    // ------------------------------------------------------
    //  * fonts *
    // ------------------------------------------------------

    struct grGlyph {
        int codepoint = -1;
        float xAdvance = 0.f;
        grRect extent;
        grRect texCoord;
    };

    struct grFontTex {
        unsigned char* data = nullptr;
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int bpp = 0;
    };

    struct grFont {
        grFontId fontId = 0;
        grTextureId texture = 0;
        grString name;
        grArray<grGlyph> glyphs;
        grContext* context = nullptr;
        grFontTex pixels;
        float fontSize = 12.f;
        float lineHeight = 12.f;
        bool dirty = true;
    };

    // ------------------------------------------------------
    //  * core public interfaces *
    // ------------------------------------------------------

    GOOBER_API grResult<grFontId> grCreateDefaultFont(grContext* context);
    GOOBER_API grStatus grDestroyFont(grContext* context, grFontId fontId);

    GOOBER_API grFont const* grGetFont(grContext* context, grFontId fontId);

    GOOBER_API grGlyph const* grFontGetGlyph(grFont const* font, int codepoint);
    GOOBER_API grGlyph const* grFontGetGlyph(grContext* context, grFontId fontId, int codepoint);

    GOOBER_API grVec2 grFontMeasureText(grContext* context, grFontId fontId, grStringView text);

    GOOBER_API bool grFontIsDirty(grContext* context, grFontId fontId);
    GOOBER_API grFontTex const* grFontGetAlpha8(grContext* context, grFontId fontId);
    GOOBER_API grFontTex const* grFontGetRGBA32(grContext* context, grFontId fontId);

} // namespace goober

#endif // defined(GOOBER_FONT_HH_)
