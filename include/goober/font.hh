// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_FONT_HH_)
#define GOOBER_FONT_HH_
#pragma once

#include "core.hh"

inline namespace goober {
    // ------------------------------------------------------
    //  * forward declarations *
    // ------------------------------------------------------

    struct grFont;

    // ------------------------------------------------------
    //  * fonts *
    // ------------------------------------------------------

    struct grGlyph {
        int glyph = -1;
        grVec2 size;
        grRect texcoord;
    };

    struct grFontTex {
        unsigned char* data = nullptr;
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int bpp = 0;
    };

    struct grFont {
        grString name;
        void* texture = nullptr;
        grArray<grGlyph> glyphs;
        grContext* context = nullptr;
        grFontTex alpha;
        grFontTex rgba;
        bool dirty = true;
    };

    // ------------------------------------------------------
    //  * core public interfaces *
    // ------------------------------------------------------

    GOOBER_API grResult<grFont*> grCreateDefaultFont(grContext* context);
    GOOBER_API grStatus grDestroyFont(grFont* font);

    GOOBER_API grGlyph const* grFontGetGlyph(grFont* font, int glyph);

    GOOBER_API bool grFontIsDirty(grFont* font);
    GOOBER_API grResult<grFontTex*> grFontGetAlpha8(grFont* font);
    GOOBER_API grResult<grFontTex*> grFontGetRGBA32(grFont* font);

} // namespace goober

#endif // defined(GOOBER_FONT_HH_)
