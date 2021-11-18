/**
* @file main.cpp
* @Copyrigth Ericsson Ltd. © 2021
* @author egon.csaba.osvath@ericsson.com
*/

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

std::string setSubDir(std::string s, std::string subDir) {

   std::string::size_type i = s.rfind('/', s.length());

   if (i != std::string::npos) {
      s.insert(i+1, subDir);
   }
   return s;
}

inline void print_path(const vector <int> &path){
    for (int i:path)
    {
        cout<<i+1<<' ';
    }
    cout<<endl;
}

int main(int argc, char *argv[])
{
    if(argc!=2 && argc !=3)
    {
        std::cout<<"Usage: MPLS {INPUT_FILE_NAME} [--quiet=false]\n";
        exit(EXIT_FAILURE);
    }

    //Load graph from file
    Graph<int> mpls_original(argv[1]);
    Graph<int> mpls = mpls_original.clone();

    bool quiet = true;
    if(argc==3)
    {
        if(std::string(argv[2])=="--quiet=false")
        {
            quiet = false;
        }
    }
    mpls_original.draw(replaceExt(argv[1],"png").c_str());
    mpls.R_F_W(setSubDir(argv[1],"res/"));

    cout<<mpls.get_secondary_stats()<<"%"<<endl;

    mpls.print_graph_nexts();

    Statistics linkStats;
    for(int i=0; i<mpls_original.get_nr_of_nodes()-1; i++)
    {
        for(int j=i+1; j<mpls_original.get_nr_of_nodes();j++)
        {
            if(mpls.get_weight(i,j))
            {
                Graph<int> clone = mpls.clone();
                clone.remove_link(i,j);

                cout<<endl<<"Link "<<i+1<<"->"<<j+1<<" removed: "<<endl;

                for(int k=0;k<clone.get_nr_of_nodes()-1;k++)
                {
                    for(int l=k+1; l<clone.get_nr_of_nodes();l++)
                    {
                        vector<int> path{};
                        clone.get_paths_between(k, l, path);
                        if(path.empty())
                        {
                            linkStats.add(true);
                            cout<<"NO PATH FOUND between: "<<k+1<<"->"<<l+1<<" when link: "<<i+1<<"->"<<j+1<<" removed!";
                        }
                        else
                        {
                            linkStats.add(false);
                            print_path(path);
                        }
                    }
                }
            }
        }
    }

    cout<<"Link outage stats: "<<linkStats.get_stats()<<"%"<<endl<<endl;

    Statistics nodeStats;
    for(int i=0;i<mpls_original.get_nr_of_nodes();i++)
    {
        Graph<int> clone = mpls.clone();
        clone.remove_node(i);

        cout<<endl<<"Node "<<i+1<<" removed: "<<endl;

        for(int k=0; k < clone.get_nr_of_nodes()-1; k++)
        {
            for(int l=k+1; l < clone.get_nr_of_nodes() ;l++)
            {
                if(k!=i && l!=i)
                {
                    vector<int> path{};
                    clone.get_paths_between(k,l,path);
                    if(path.empty())
                    {
                        nodeStats.add(true);
                        cout<<"NO PATH FOUND between: "<<k+1<<"->"<<l+1<<" when node "<<i+1<<" removed!";
                    }
                    else
                    {
                        nodeStats.add(false);
                        print_path(path);
                    }
                }
            }
        }
    }

    cout<<"Node outage stats: "<<nodeStats.get_stats()<<'%'<<endl<<endl;

    return 0;
}
