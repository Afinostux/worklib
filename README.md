# worklib
experimental header only 3d game library

The intent is to make a library geared toward 3D RPG and strategy game development
for small teams. Eventually, I hope to add features to assist in procedural generation
of everything from animations to level geometry.

In it's current state, there are already some cool features:

 - Vertices can have any combination of attributes, which are interleaved when stored on the graphics card.
 - Shaders can be loaded from .vert and .frag files
 - `#include` directives are supported in shader files
 - Simple shader animation is supported

so far, this project uses it: https://focks.itch.io/dogfighty

