# MPLS
Ericsson Project Laboratory - MPLS routing algorithms with TI-LFA
### Required package: 
##### graphviz
Install with command:
`sudo apt install graphviz-dev`
### Usage
This is a CodeBlocks project. Just import the .cbp file, into the IDE.
Place the input graph's file (simple text file, see examples in data folder, and documentation in `graph.h` about it's structure) with `.in` extension to the data folder. 
Give the input graph's filename in main.cpp.
The program will analyze, and calculate statistics about the number of labels required for local repair for node protection, after that with link protection, and print statistics to the console.
