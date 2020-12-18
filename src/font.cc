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

    static void grRebuildFontsAndAtlas(grFontAtlas& atlas, grArray<grFont*> const& fonts) {
        grFree(atlas.data);
        atlas.data = nullptr;

        grArray<stbtt_packedchar> packed;

        atlas.width = 512;
        atlas.height = 512;
        atlas.data = static_cast<unsigned char*>(grAlloc(atlas.width * atlas.height));
        atlas.bpp = 8;

        stbtt_pack_context packing;
        stbtt_PackBegin(&packing, atlas.data, atlas.width, atlas.height, 0, 1, nullptr);

        // ensure we have a default block of pixels that are a solid white
        stbrp_rect pix;
        pix.w = 1;
        pix.h = 1;
        stbtt_PackFontRangesPackRects(&packing, &pix, 1);
        assert(pix.x == 0);
        assert(pix.y == 0);
        atlas.data[0] = 0xFF;

        for (grFont* font : fonts) {
            if (font == nullptr)
                continue;

            stbtt_fontinfo fontInfo;
            stbtt_InitFont(
                &fontInfo,
                goober_proggy_data,
                stbtt_GetFontOffsetForIndex(goober_proggy_data, 0));

            packed.resize(255);
            stbtt_PackFontRange(
                &packing,
                goober_proggy_data,
                0,
                STBTT_POINT_SIZE(font->fontSize),
                0,
                255,
                packed.data());

            float const widthScalar = 1.f / atlas.width;
            float const heightScalar = 1.f / atlas.height;

            font->glyphs.clear();
            font->glyphs.reserve(packed.size());
            for (int index = 0; index != 255; ++index) {
                stbtt_packedchar const& pchar = packed[index];
                grRect const extent{{pchar.xoff, pchar.yoff}, {pchar.xoff2, pchar.yoff2}};
                grRect const uv{
                    {pchar.x0 * widthScalar, pchar.y0 * heightScalar},
                    {pchar.x1 * widthScalar, pchar.y1 * heightScalar}};
                font->glyphs.push_back({index, pchar.xadvance, extent, uv});
            }
        }

        stbtt_PackEnd(&packing);
    }

    grResult<grFontId> grCreateDefaultFont(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;

        grFont* font = context->fonts.push_back(new (grAlloc(sizeof(grFont))) grFont());
        grFontId const id = font->fontId = context->nextFontId++;
        context->fontAtlas->dirty = true;
        return id;
    }

    grStatus grDestroyFont(grContext* context, grFontId fontId) {
        if (context == nullptr)
            return grStatus::NullArgument;

        for (grFont*& font : context->fonts) {
            if (font->fontId == fontId) {
                font->~grFont();
                grFree(font);
                font = nullptr;
                context->fontAtlas->dirty = true;
                return grStatus::Ok;
            }
        }

        return grStatus::InvalidId;
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

    grVec2 grFontMeasureText(grContext* context, grFontId fontId, grStringView text) {
        grFont const* font = grGetFont(context, fontId);
        if (font == nullptr)
            return {};

        grVec2 size{0, font->lineHeight};

        for (char ch : text) {
            grGlyph const* glyph = grFontGetGlyph(font, ch);
            if (glyph == nullptr)
                continue;

            size.x += glyph->xAdvance;
        }

        return size;
    }

    grFontAtlas const* grGetFontAtlasIfDirtyAlpha8(grContext* context) {
        if (context == nullptr)
            return nullptr;

        if (!context->fontAtlas->dirty)
            return nullptr;

        if (context->fontAtlas->data == nullptr || context->fontAtlas->bpp != 8) {
            grFree(context->fontAtlas->data);
            grRebuildFontsAndAtlas(*context->fontAtlas, context->fonts);
        }

        return context->fontAtlas;
    }

    void grFontAtlasBindTexture(grContext* context, grTextureId textureId) {
        if (context == nullptr)
            return;

        context->fontAtlas->texture = textureId;
        context->fontAtlas->dirty = false;
    }

} // namespace goober
