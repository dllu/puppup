# Puppup: Basic crossword game engine

Suggests moves for crossword games such as [Scrabble](https://en.wikipedia.org/wiki/Scrabble) and Words with Friends using a [gaddag](https://en.wikipedia.org/wiki/GADDAG).

![screenshot](https://pics.dllu.net/file/dllu-sc/1485531d49.png)

# About

A GADDAG is a type of directed acyclic word graph (DAWG) where each word is stored with each of its prefixes, reversed.

![puppup](https://daniel.lawrence.lu/puppy/puppup.png)

**Figure 1**: A small purple dawg and its reverse.

# Build

You need CMake and a reasonably new GCC or Clang compiler supporting C++14.

```
mkdir build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

# Running

Puppup outputs [ANSI colour codes](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors), so be sure to use a terminal that supports this.
You need to supply your own dictionary (a text file with one word per line).

```
./puppup word_list.txt
```

The word list is not provided with puppup (see: [Who owns Scrabble's word list?](http://www.slate.com/articles/life/gaming/2014/09/major_scrabble_brouhaha_can_you_copyright_a_list_of_words.html)).
