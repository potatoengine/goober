// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/draw.hh"
#include "goober/font.hh"

inline namespace goober {

    static grDrawList::Command& pushCommand(
        grArray<grDrawList::Command>& commands,
        grTextureId textureId) {
        if (!commands.empty()) {
            grDrawList::Command& cmd = commands.back();
            if (cmd.indexCount == 0 || cmd.textureId == 0) {
                cmd.textureId = textureId;
                return cmd;
            }

            if (textureId == 0 || cmd.textureId == textureId)
                return cmd;
        }

        grDrawList::Command& cmd = commands.push_back({});
        cmd.textureId = textureId;
        return cmd;
    }

    void grDrawList::drawRect(grRect rect, grColor color) {
        Offset const vertex = static_cast<Offset>(vertices.size());
        Offset const index = static_cast<Offset>(indices.size());

        Command& cmd = pushCommand(commands, 0);

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

    void grDrawList::drawRect(grTextureId textureId, grRect rect, grRect texCoord, grColor color) {
        Offset const vertex = static_cast<Offset>(vertices.size());
        Offset const index = static_cast<Offset>(indices.size());

        Command& cmd = pushCommand(commands, textureId);

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

    void grDrawList::drawText(
        grFont const* font,
        grTextureId textureId,
        grVec2 pos,
        grColor color,
        grStringView text) {
        if (font == nullptr)
            return;

        pos.y += font->lineHeight;

        for (char ch : text) {
            grGlyph const* glyph = grFontGetGlyph(font, ch);
            if (glyph == nullptr)
                continue;

            Offset const vertex = static_cast<Offset>(vertices.size());
            Offset const index = static_cast<Offset>(indices.size());

            Command& cmd = commands.empty() ? commands.push_back({index, 0}) : commands.back();

            drawRect(
                textureId,
                {pos + glyph->extent.minimum, pos + glyph->extent.maximum},
                glyph->texCoord,
                color);

            pos.x += glyph->xAdvance;
        }
    }

} // namespace goober
