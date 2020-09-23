goober
======

Immediate-mode GUI experiment.

By Sean Middleditch.

See COPYING.md for license information.

About
=====

goober is a simple GUI library built around the immediate-mode paradigm.

goober aims to offer several core features not commonly found in contemporary
imgui libraries, while also attempting to merge in some of the core
capabilities and strengths of both functional-reactive and
document-object-model GUIs.

Three of the major capabilities goober wishes to offer over the common imgui
approach are:
- advanced automatic layout and positioning, close to the HTML box model
- flexible style engine, similar to a very-stripped-down CSS model
- stateful custom widgets, cleanly separating the app model and presentation

From the functional-reactive world, goober also wishes to offer mechanisms
for authoring component-based GUIs that can react to input or state
changes. The intent is not to build a complete state management layer, but
to offer easy integration for application state change notifications combined
with input event detection. A component which is invalidated would be
"rerendered" using the same imgui-style API as goober uses elsewhere.

Finally, goober is intended to be usable as simple debug GUI layer for an
existing app, but also aims to optimize for being used as the core GUI
API for a larger application. To that end, the intent is treat desktop-
oriented features.

goober would like to remain small and light with minimal dependencies, like
many other imgui libraries. However, given any trade-off that inevitably
must be made between minimalism and offering first-class support for full
desktop-class application capabilities, the full capabilities will be
prioritized.

Design
======

goober is in the _very_ early stages of design.

imgui
-----

goober is primarily an immediate-mode GUI library. To that end, the core
"widgets" of goober are encoded almost entirely as public API functions.
These functions perform a combination of submitting state for later
layout and rendering as well as return active state to the application.
For example, a button will look something vaguely like:

```c++
  if (grButton(ctx, "Click Me")) {
    do_something();
  }
```

The application will not be expected to maintain separate state for the
button, is not expected to perform any cleanup or lifetime management,
and will primarily work with interactive-widgets via function return
values rather than callbacks or events.

Layout
------

A key motivating feature for goober is to offer automatic layout capabilities
on par with many desktop applications and complex editors require. Other
imgui libraries often rely on either explicit positioning from the user or
via extremely rudimentary layout capabilities; both tend to require a lot of
code and tweaking and pain from the developer to get a UI layout to look as
desired, and neither tend to lend themselves well to more dynamic UI needs.

The specific layout algorithms or techniques are not really decided upon at
this time. The author is considering the HTML box model as one candidate.

At the very least, goober is intended to require multiple passes to perform
layout for widgets. In turn, this will limit one common feature of other
imgui libraries: immediate-mode code will not be able to query the size or
position of widgets after submission. Any user code that requires this
information will necessarily have to run via a callback executed after the
layout pass.

In the author's experience, _many_ needs to query imgui libraries for size
or position information are necessitated by the lack of automatic layout
capabilities in the first place. It is possible that goober will eventually
offer means of reading _previous_ layout information for a widget from the
prior frame, which may help some of the remaining use cases for querying
this information, but it is to be seen if this will be sufficient in practice
for most users.

Styling
-------

More capable styling support is another feature desired for goober. The
style capabilities of many other imgui libraries are limited to a fairly
small fixed set of features, such as color, some padding choices, and
image overrides. More complex style changes like frames, border shapes,
animations, text effects and highlights, and so on require either building
completely custom widgets with custom rendering commands or modifying the
core of these libraries.

These contraints in particular do a great harm to composition. Libraries
of add-on widgets exist for other imgui libraries, but modifications to the
core imgui libraries or application-custom widgets then fail to mesh well
with add-on widgets from third-parties. A style system and appropriate
hooks allow applications to unify the look and feel of their custom widgets,
the core goober widgets, and third-party add-on widgets they wish to consume.

goober does not aim to solve _all_ of these problems out of the box. The
core design is intended to allow a "style engine" to be plugged in, so an
application requiring a large degree of customization can do this without
modifying the goober library itself.

The core goober style engine, and the style engine, is intended to
utilize a system very similar to simplistic CSS selectors. A widget declares
a tag for its core type, tags for state, and tags for context, and the
style engine can consume this list of tags and return layout-relevant
style information like padding as well as be responsible for submitting
the actual rendering commands.

The specifics of how tags are defined, consumed, or matches is undecided.
The author intends to favor speed, efficiency, and simplicity as much
as possible. Bit masks and an array mapping tag strings to bit indices,
for example, will be considered.

Portals
-------

goober will be built around a concept so-far dubbed portals.

Portals provide a means to separate trees of widgets into containerized
groups. Portals are isolated in terms of layout and widget state, are
nestable, provide the boundary for any state recording and capture.

A portal will be the building block for the functional-reactive
components of goober. Portals will provide the state management for
pop-up windows and tooltips, and can provide the means to escape the
limitations imposed by goober's layout and style engine for size
calculation.

As a parallel, portals are similar to the concept of windows in the
Dear imgui library, but without any of the implicit assumptions about
having title bars, borders, or so on. A floating window of the Dear
imgui style might be implemented as two portals, one containing the
title bar and other window decorations while an inner portal contains
the window content (this is not necessarily the actual plan for
goober; just an example of what we might do).

TBD
---

More design notes to be added.
