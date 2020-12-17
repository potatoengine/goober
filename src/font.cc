// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/font.hh"

#include <limits>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC

#include "stb_rect_pack.h"
#include "stb_truetype.h"

extern "C" {
extern unsigned char const goober_proggy_data[];
extern std::size_t goober_proggy_size;
}

namespace goober {

    static void grRenderFont(grFont& font) {
        stbtt_fontinfo fontInfo;
        stbtt_InitFont(
            &fontInfo,
            goober_proggy_data,
            stbtt_GetFontOffsetForIndex(goober_proggy_data, 0));

        grFree(font.pixels.data);
        font.pixels.data = nullptr;

        grArray<stbtt_packedchar> packed;
        packed.resize(255);

        font.pixels.width = 512;
        font.pixels.height = 512;
        font.pixels.data =
            static_cast<unsigned char*>(grAlloc(font.pixels.width * font.pixels.height));
        font.pixels.bpp = 8;

        stbtt_pack_context packing;
        stbtt_PackBegin(
            &packing,
            font.pixels.data,
            font.pixels.width,
            font.pixels.height,
            0,
            1,
            nullptr);

        // ensure we have a default block of pixels that are a solid white
        stbrp_rect pix;
        pix.w = 1;
        pix.h = 1;
        stbtt_PackFontRangesPackRects(&packing, &pix, 1);
        assert(pix.x == 0);
        assert(pix.y == 0);
        font.pixels.data[0] = 0xFF;

        stbtt_PackFontRange(
            &packing,
            goober_proggy_data,
            0,
            STBTT_POINT_SIZE(font.fontSize),
            0,
            255,
            packed.data());

        float const widthScalar = 1.f / font.pixels.width;
        float const heightScalar = 1.f / font.pixels.height;

        font.glyphs.clear();
        font.glyphs.reserve(packed.size());
        for (int index = 0; index != 255; ++index) {
            stbtt_packedchar const& pchar = packed[index];
            grRect const extent{{pchar.xoff, pchar.yoff}, {pchar.xoff2, pchar.yoff2}};
            grRect const uv{
                {pchar.x0 * widthScalar, pchar.y0 * heightScalar},
                {pchar.x1 * widthScalar, pchar.y1 * heightScalar}};
            font.glyphs.push_back({index, pchar.xadvance, extent, uv});
        }

        stbtt_PackEnd(&packing);
    }

    grResult<grFontId> grCreateDefaultFont(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;

        grFont* font = context->fonts.push_back(new (grAlloc(sizeof(grFont))) grFont());
        grFontId const id = font->fontId = context->nextFontId++;
        return id;
    }

    grStatus grDestroyFont(grFont* font) {
        if (font == nullptr)
            return grStatus::NullArgument;

        grFree(font->pixels.data);

        font->~grFont();
        grFree(font);
        return grStatus::Ok;
    }

    static grFont* grGetFontMutable(grContext* context, grFontId fontId) {
        if (context == nullptr)
            return nullptr;

        for (grFont* font : context->fonts)
            if (font->fontId == fontId)
                return font;

        return nullptr;
    }

    grFont const* grGetFont(grContext* context, grFontId fontId) {
        return grGetFontMutable(context, fontId);
    }

    grGlyph const* grFontGetGlyph(grFont const* font, int codepoint) {
        if (font == nullptr)
            return nullptr;

        for (grGlyph const& gly : font->glyphs) {
            if (gly.codepoint == codepoint) {
                return &gly;
            }
        }
        return nullptr;
    }

    grGlyph const* grFontGetGlyph(grContext* context, grFontId fontId, int codepoint) {
        if (context == nullptr)
            return nullptr;

        return grFontGetGlyph(grGetFont(context, fontId), codepoint);
    }

    bool grFontIsDirty(grContext* context, grFontId fontId) {
        grFont const* font = grGetFont(context, fontId);
        return font != nullptr ? font->dirty : false;
    }

    grFontTex const* grFontGetAlpha8(grContext* context, grFontId fontId) {
        grFont* font = grGetFontMutable(context, fontId);
        if (font == nullptr)
            return nullptr;

        if (font->pixels.data == nullptr || font->dirty || font->pixels.bpp != 8) {
            grFree(font->pixels.data);
            grRenderFont(*font);
        }

        font->dirty = false;
        return &font->pixels;
    }

    grFontTex const* grFontGetRGBA32(grContext* context, grFontId fontId) {
        grFont* font = grGetFontMutable(context, fontId);
        if (font == nullptr)
            return nullptr;

        if (font->pixels.data == nullptr || font->dirty || font->pixels.bpp != 32) {
            grFree(font->pixels.data);
        }

        font->dirty = false;
        return &font->pixels;
    }

} // namespace goober
