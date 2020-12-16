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

    void grLoadFont(grFont* font, float size) {
        stbtt_fontinfo fontInfo;
        stbtt_InitFont(
            &fontInfo,
            goober_proggy_data,
            stbtt_GetFontOffsetForIndex(goober_proggy_data, 0));

        grArray<stbtt_packedchar> packed;
        packed.resize(255);

        font->alpha.width = 512;
        font->alpha.height = 512;
        font->alpha.data =
            static_cast<unsigned char*>(grAlloc(font->alpha.width * font->alpha.height));
        font->alpha.bpp = 8;

        stbtt_pack_context packing;
        stbtt_PackBegin(
            &packing,
            font->alpha.data,
            font->alpha.width,
            font->alpha.height,
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
        font->alpha.data[0] = 0xFF;

        stbtt_PackFontRange(
            &packing,
            goober_proggy_data,
            0,
            STBTT_POINT_SIZE(size),
            0,
            255,
            packed.data());

        float const widthScalar = 1.f / font->alpha.width;
        float const heightScalar = 1.f / font->alpha.height;

        font->glyphs.reserve(packed.size());
        for (int index = 0; index != 255; ++index) {
            stbtt_packedchar const& pchar = packed[index];
            grRect const extent{{pchar.xoff, pchar.yoff}, {pchar.xoff2, pchar.yoff2}};
            grRect const uv{
                {pchar.x0 * widthScalar, pchar.y0 * heightScalar},
                {pchar.x1 * widthScalar, pchar.y1 * heightScalar}};
            font->glyphs.push_back({index, pchar.xadvance, extent, uv});
        }

        stbtt_PackEnd(&packing);
    }

    grResult<grFont*> grCreateDefaultFont(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;

        grFont* font = new (grAlloc(sizeof(grFont))) grFont;
        grLoadFont(font, 13.f);
        return font;
    }

    grStatus grDestroyFont(grFont* font) {
        if (font == nullptr)
            return grStatus::NullArgument;

        grFree(font->alpha.data);
        grFree(font->rgba.data);

        font->~grFont();
        grFree(font);
        return grStatus::Ok;
    }

    grGlyph const* grFontGetGlyph(grFont* font, int codepoint) {
        if (font == nullptr)
            return nullptr;

        for (grGlyph const& gly : font->glyphs) {
            if (gly.codepoint == codepoint) {
                return &gly;
            }
        }
        return nullptr;
    }

    bool grFontIsDirty(grFont* font) { return font != nullptr ? font->dirty : false; }

    grResult<grFontTex*> grFontGetAlpha8(grFont* font) {
        if (font == nullptr)
            return grStatus::NullArgument;

        if (font->alpha.data == nullptr)
            return grStatus::Empty;

        font->dirty = false;
        return &font->alpha;
    }

    grResult<grFontTex*> grFontGetRGBA32(grFont* font) {
        if (font == nullptr)
            return grStatus::NullArgument;

        if (font->rgba.data == nullptr)
            return grStatus::Empty;

        font->dirty = false;
        return &font->rgba;
    }

} // namespace goober
