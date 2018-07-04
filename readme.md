# Puppup: Basic crossword game engine

Suggests moves for crossword games such as Scrabble and Words with Friends using a [gaddag](https://en.wikipedia.org/wiki/GADDAG).

![screenshot](https://pics.dllu.net/file/dllu-sc/c3f100da1a.png)

# About

A GADDAG is a type of directed acyclic word graph (DAWG) where each word is stored with each of its prefixes, reversed.

![puppup](https://daniel.lawrence.lu/puppy/puppup.png)

**Figure 1**: A small purple dawg and its reverse.

# Build

You need CMake

```
mkdir build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
