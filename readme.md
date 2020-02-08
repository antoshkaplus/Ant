
# Ant 

C++ homemade library with different algorithm and data structures implementations

## Project structure

- app     - different implementation projects. there we can benchmark different implementations of the same algo/data struct or try something new
- bin     - every executable goes there
- build-* - generated solutions for different IDEs
- data    - different test data
- include - include of ant library and gtest
- src     - sources of ant library
- lib     - ant binary file goes there. also other library files
- test    - unit tests for ant library and gtest sources

## Installation

In library directory:
- $mkdir build
- $cd build
- $cmake -DCMAKE_BUILD_TYPE=Release ..
- $make
- $make install

# TODO
* core/ implement IndexSet - map that stores indexes. Use bool array for storage.
    maybe store array of keys too. Maybe abandon remove operation.
* core/ rename DecreaseClustering to DecreaseClusters. Maybe Reduce suits better.
    add new method ReindexClusters that would remove offsets between cluster indices.
* a map that lets user to keep different type, probably some kind of serialization involved
* core/range/ - sentinel should not take up space, maybe use one sentinel for all the ranges

# GTEST FILTER
--gtest_filter=POSTIVE_PATTERNS[-NEGATIVE_PATTERNS]