// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_WIDGETS_HH_)
#define GOOBER_WIDGETS_HH_
#pragma once

#include "core.hh"

#define GOOBER_API

inline namespace goober {
    // ------------------------------------------------------
    //  * standard goober widgets *
    // ------------------------------------------------------

    GOOBER_API bool grButton(grContext* context, grStringView label, grVec2 pos, grColor rgba);

} // namespace goober

#endif // defined(GOOBER_WIDGETS_HH_)
