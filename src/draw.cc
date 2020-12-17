// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/draw.hh"
#include "goober/font.hh"

inline namespace goober {

    void grDrawList::drawRect(grRect rect, grColor color) {
        Offset const vertex = static_cast<Offset>(vertices.size());
        Offset const index = static_cast<Offset>(indices.size());

        Command& cmd = commands.empty() ? commands.push_back({index, 0}) : commands.back();

        vertices.push_back({rect.minimum, {}, color});
        vertices.push_back({{rect.maximum.x, rect.minimum.y}, {}, color});
        vertices.push_back({rect.maximum, {}, color});
        vertices.push_back({{rect.minimum.x, rect.maximum.y}, {}, color});

        indices.push_back(vertex + 0);
        indices.push_back(vertex + 1);
        indices.push_back(vertex + 2);
        indices.push_back(vertex + 2);
        indices.push_back(vertex + 3);
        indices.push_back(vertex + 0);

        cmd.indexCount += 6;
    }

    void grDrawList::drawRect(grRect rect, grRect texCoord, grColor color) {
        Offset const vertex = static_cast<Offset>(vertices.size());
        Offset const index = static_cast<Offset>(indices.size());

        Command& cmd = commands.empty() ? commands.push_back({index, 0}) : commands.back();

        vertices.push_back({rect.minimum, texCoord.minimum, color});
        vertices.push_back(
            {{rect.maximum.x, rect.minimum.y}, {texCoord.maximum.x, texCoord.minimum.y}, color});
        vertices.push_back({rect.maximum, texCoord.maximum, color});
        vertices.push_back(
            {{rect.minimum.x, rect.maximum.y}, {texCoord.minimum.x, texCoord.maximum.y}, color});

        indices.push_back(vertex + 0);
        indices.push_back(vertex + 1);
        indices.push_back(vertex + 2);
        indices.push_back(vertex + 2);
        indices.push_back(vertex + 3);
        indices.push_back(vertex + 0);

        cmd.indexCount += 6;
    }

    void grDrawList::drawText(grFont const* font, grVec2 ul, grColor color, grStringView text) {
        if (font == nullptr)
            return;

        grVec2 pos = ul;

        for (char ch : text) {
            grGlyph const* glyph = grFontGetGlyph(font, ch);
            if (glyph == nullptr)
                continue;

            Offset const vertex = static_cast<Offset>(vertices.size());
            Offset const index = static_cast<Offset>(indices.size());

            Command& cmd = commands.empty() ? commands.push_back({index, 0}) : commands.back();

            drawRect(
                {pos + glyph->extent.minimum, pos + glyph->extent.maximum},
                glyph->texCoord,
                color);

            pos.x += glyph->xAdvance;
        }
    }

} // namespace goober
