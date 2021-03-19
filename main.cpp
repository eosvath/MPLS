#include <iostream>
#include "graph.h"
using namespace std;

int main()
{
    Graph<int> mpls("data/graph.in");
    Graph<int> mpls_removed = mpls.clone();
    try{
        mpls_removed.remove_node(2);
    }
    catch(const char* msg)
    {
        std::cout<<"Error! "<<msg<<'\n';
    }

    mpls.R_F_W();
    mpls_removed.R_F_W();

    mpls.print_paths();
    std::cout<<std::endl;
    mpls_removed.print_paths();

    try{
        check_changed_paths<int>(mpls, mpls_removed);
    }
    catch(const char* msg)
    {
        std::cout<<"Error! "<<msg<<'\n';
    }

    return 0;
}
