/**
* @file main.cpp
* @Copyrigth Ericsson Ltd. © 2021
* @author egon.csaba.osvath@ericsson.com
*/

#include <iostream>
#include "include/graph.h"
#include "include/stats.h"
#include <set>
#include <algorithm>

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

bool paths_are_disjoint(const vector<int>& first, const vector<int>& second)
{
    set<int> first_set(first.begin()+1, first.end()-1);
    set<int> second_set(second.begin()+1, second.end()-1);

    set<int> intersect;
    set_intersection(first_set.begin(), first_set.end(), second_set.begin(), second_set.end(),
                 std::inserter(intersect, intersect.begin()));

    return intersect.empty();
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

    vector<int> stats(5,0);
    int total = 0;

    for(int i=0; i<mpls.get_nr_of_nodes(); ++i)
    {
        for(int j=0; j<mpls.get_nr_of_nodes(); ++j)
        {
            if(i!=j)
            {
                cout<<i+1<<" --> "<<j+1<<endl;
                vector<vector<int>> paths_between_i_j;
                mpls.get_paths_between(i, j, paths_between_i_j, stack<int>(),
                    std::vector<bool>(mpls.get_nr_of_nodes(), false));
                vector<int> labels_needed(paths_between_i_j.size());

                for(int k = 0; k < paths_between_i_j.size(); ++k)
                {
                    labels_needed[k] = mpls.get_nr_of_labels(paths_between_i_j[k]);
                    cout<<"Labels needed: "<<labels_needed[k]<<" path: ";
                    for(const auto l:paths_between_i_j[k])
                    {
                        cout<<l+1<<' ';
                    }
                    cout<<endl;
                }

                vector <pair<pair<int,vector<int>>,pair<int,vector<int>>>> disjoint_paths_i_j;
                for(int k = 0; k < paths_between_i_j.size()-1; ++k)
                {
                    for(int l = k+1; l < paths_between_i_j.size(); ++l)
                    {
                        if(paths_are_disjoint(paths_between_i_j[k], paths_between_i_j[l]))
                        {
                            disjoint_paths_i_j.emplace_back(pair<pair<int,vector<int>>,pair<int,vector<int>>>(
                                    make_pair(labels_needed[k],paths_between_i_j[k]),
                                    make_pair(labels_needed[l],paths_between_i_j[l])));
                        }
                    }
                }

                cout<<endl;

                int best_index = 0, least_labels = -1;
                int index = 0;
                for(const auto &disjoint_path_pair : disjoint_paths_i_j)
                {
                    if(least_labels == -1 ||
                            (disjoint_path_pair.first.first + disjoint_path_pair.second.first) < least_labels)
                    {
                        least_labels = disjoint_path_pair.first.first + disjoint_path_pair.second.first;
                        best_index = index;
                    }
                    ++index;
                }

                cout<<"Best disjoint path pair with a total of "<<least_labels<<" needed:"<<endl;
                cout<<"First: ";
                for(const auto &x : disjoint_paths_i_j[best_index].first.second)
                {
                    cout<<x+1<<' ';
                }
                cout<<endl<<"Second: ";
                for(const auto &x : disjoint_paths_i_j[best_index].second.second)
                {
                    cout<<x+1<<' ';
                }
                cout<<endl<<endl;

                if(least_labels < 4)
                {
                    ++stats[least_labels];
                    ++total;
                }
            }
        }
    }

    cout<<"Percentage of labels needed:"<<endl;
    for(int i=0;i<stats.size();i++)
    {
        cout<<"  "<<i<<" \t\t   ";
    }
    cout<<endl;
    for(int i=0;i<stats.size();i++)
    {
        cout<<"  "<<stats[i]/(double)total*100<<"          ";
    }

    return 0;
}
