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

    mpls.print_paths();

    return 0;
}
