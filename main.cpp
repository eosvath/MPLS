#include <iostream>
#include "graph.h"
using namespace std;

int main()
{
    //Load graph from file
    Graph<int> mpls("data/graph.in");

    mpls.draw("graph.png");

    //Copy graph loaded from file
    Graph<int> mpls_removed = mpls.clone();
    //Try to remove a node from copied graph
    try{
        mpls_removed.remove_node(2);
    }
    catch(const char* msg)
    {
        std::cout<<"Error! "<<msg<<'\n';
    }
    //Determine the shortest distances and paths in both graphs
    mpls.R_F_W();
    mpls_removed.R_F_W();

    //Print the shortest paths calculated in both graphs, in a human readable format
    mpls.print_paths();
    std::cout<<std::endl;
    mpls_removed.print_paths();

    /***
    * Try to check for differences in shortest paths, and determine the differing subpaths in graph2 from graph1,
    * then iteratively determine how many labels are needed to do the rerouting,
    * then print where the labels are assigned with what destination.
    */
    try{
        check_changed_paths<int>(mpls, mpls_removed);
    }
    catch(const char* msg)
    {
        std::cout<<"Error! "<<msg<<'\n';
    }

    return 0;
}
