/**
* @file main.cpp
* @Copyrigth Ericsson Ltd. © 2021-2022
* @author egon.csaba.osvath@ericsson.com
*/

#include <iostream>
#include "include/graph.h"
#include "include/stats.h"
using namespace std;

#include <fstream>

ofstream fout("data/result.out");

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
        fout<<i+1<<' ';
    }
    fout<<endl;
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

    mpls.R_F_W(true);
    mpls.print_graph_nexts();

    bool quiet = true;
    if(argc==3)
    {
        if(std::string(argv[2])=="--quiet=false")
        {
            quiet = false;
        }
    }
    mpls_original.draw(replaceExt(argv[1],"png").c_str());

    Statistics linkStats;
    for(int i=0; i<mpls_original.get_nr_of_nodes(); i++)
    {
        for(int j=0; j<mpls_original.get_nr_of_nodes();j++)
        {
            if(mpls_original.get_weight(i,j) && i!=j)
            {
                Graph<int> clone = mpls_original.clone();
                clone.remove_link(i,j);
                clone.R_F_W();

                fout<<endl<<"Link "<<i+1<<"->"<<j+1<<" removed: "<<endl;

                vector<int> tmp = mpls.remove_link_temporarily(i,j);

                for(int k=0;k<clone.get_nr_of_nodes();k++)
                {
                    for(int l=0; l<clone.get_nr_of_nodes();l++)
                    {
                        if(k!=l)
                        {
                            vector<int> path{};
                            mpls.get_paths_between(k, l, path);
                            fout<<endl;
//                            if(path != clone.get_path(k,l))
                            {
                                print_path(path);
                                print_path(clone.get_path(k,l));
                                fout<<endl;
                            }
                        }
                    }
                }
                mpls.restore_link(tmp,i,j);
            }
        }
    }

//    fout<<"Link outage stats: "<<linkStats.get_stats()<<"%"<<endl<<endl;
//
//    Statistics nodeStats;
//    try{
//        for(int i=0; i<mpls_original.get_nr_of_nodes(); i++)
//        {
//            Graph<int> clone = mpls_original.clone();
//            clone.remove_node(i);
//            clone.R_F_W();
//
////            for()
////
////            int next = mpls.get_nexts_primary(i,j);
////            mpls.set_nexts_primary(i,j,-1);
//
//
//            fout<<endl<<"Node "<<i+1<<" removed: "<<endl;
//
//            for(int k=0;k<clone.get_nr_of_nodes();k++)
//            {
//                for(int l=0; l<clone.get_nr_of_nodes();l++)
//                {
//                    if(k!=i && l!=i && l!=k)
//                    {
//                        vector<int> path{};
//                        mpls.get_paths_between(k, l, path);
//                        if(path != clone.get_path(k,l))
//                        {
//                            print_path(path);
//                            print_path(clone.get_path(k,l));
//                            fout<<endl;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    catch (const std::exception& e)
//    {
//        cout<<e.what();
//    }
//    fout<<"Node outage stats: "<<nodeStats.get_stats()<<'%'<<endl<<endl;

    return 0;
}
