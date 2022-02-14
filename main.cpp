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
    Graph<int> mpls(argv[1]);

    mpls.R_F_W();

    for(int i=0; i<mpls.get_nr_of_nodes()-1; ++i)
    {
        for(int j=i+1; j<mpls.get_nr_of_nodes(); ++j)
        {
            cout<<i+1<<" --> "<<j+1<<endl;
            vector<vector<int>> paths_between_i_j(8);
            mpls.get_paths_between(i, j, paths_between_i_j, stack<int>(),
                std::vector<bool>(mpls.get_nr_of_nodes(), false));
            vector<int> labels_needed(paths_between_i_j.size());
            for(int k = 0; k < paths_between_i_j.size(); ++k)
            {
                if(paths_between_i_j[k].size() > 0)
                {
                    labels_needed[k] = mpls.get_nr_of_labels(paths_between_i_j[k]);
                    cout<<"Labels needed: "<<labels_needed[k]<<" path: ";
                    for(const auto l:paths_between_i_j[k])
                    {
                        cout<<l+1<<' ';
                    }
                    cout<<endl;
                }
            }
            cout<<endl<<endl;
        }
    }

    return 0;
}
