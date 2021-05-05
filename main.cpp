#include <iostream>
#include "include/graph.h"
#include "include/stats.h"
using namespace std;

std::string replaceExt(std::string s, std::string newExt) {

   std::string::size_type i = s.rfind('.', s.length());

   if (i != std::string::npos) {
      s.replace(i+1, newExt.length(), newExt);
   }
   return s;
}

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        std::cout<<"Usage: MPLS {INPUT_FILE_NAME}\n";
        exit(EXIT_FAILURE);
    }

    //Load graph from file
    Graph<int> mpls_original(argv[1]);
    Graph<int> mpls = mpls_original.clone();

    Statistics stats;
    stats.reset();
    bool quiet = true;

    mpls_original.draw(replaceExt(argv[1],"png").c_str());

    for(int i=0;i<mpls_original.get_nr_of_nodes();++i)
    {
        //Copy graph loaded from file
//        Graph<int> mpls = mpls_original.clone();
        Graph<int> mpls_removed = mpls_original.clone();
        //Try to remove a node from copied graph
        try{
            mpls_removed.remove_node(i);
        }
        catch(const char* msg)
        {
            std::cout<<"Error! "<<msg<<'\n';
        }
        //Determine the shortest distances and paths
        mpls.R_F_W();
        mpls_removed.R_F_W();

        //Print the shortest paths calculated in both graphs, in a human readable format
        if(!quiet)
        {
            mpls.print_paths();
            std::cout<<std::endl;
            mpls_removed.print_paths();
        }
        try{
            check_changed_paths_TI_LFA<int>(mpls, mpls_removed, stats, quiet);
        }
        catch(const char* msg)
        {
            std::cout<<"Error! "<<msg<<'\n';
        }
    }

    stats.print_stats();

    stats.reset();
    for(int i=0;i<mpls_original.get_nr_of_nodes()-1;++i)
    {
        for(int j=i+1;j<mpls_original.get_nr_of_nodes();++j)
        {
            //Copy graph loaded from file
  //          Graph<int> mpls = mpls_original.clone();
            Graph<int> mpls_removed = mpls_original.clone();
            //Try to remove a node from copied graph
            try{
                mpls_removed.remove_link(i,j);
            }
            catch(const char* msg)
            {
                std::cout<<"Error! "<<msg<<'\n';
            }
            //Determine the shortest distances and paths
            mpls.R_F_W();
            mpls_removed.R_F_W();

            //Print the shortest paths calculated in both graphs, in a human readable format
            if(!quiet)
            {
                mpls.print_paths();
                std::cout<<std::endl;
                mpls_removed.print_paths();
            }
            try{

                check_changed_paths_TI_LFA<int>(mpls, mpls_removed, stats, quiet);
            }
            catch(const char* msg)
            {
                std::cout<<"Error! "<<msg<<'\n';
            }
        }
    }
    stats.print_stats();

    return 0;
}
