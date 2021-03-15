#include <iostream>
#include "graph.h"
using namespace std;

int main()
{
    Graph<int> mpls("data/graph.in");
    Graph<int> mpls_removed = mpls.clone();
    mpls_removed.remove_node(2);

    mpls.R_F_W();
    mpls_removed.R_F_W();

    mpls.print_paths();
    std::cout<<std::endl;
    mpls_removed.print_graph_nexts();
    std::cout<<std::endl;
    mpls_removed.print_paths();


    return 0;
}
