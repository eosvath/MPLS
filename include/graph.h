/**
* @file graph.h
* @Copyrigth Ericsson Ltd. © 2021-2022
* @author egon.csaba.osvath@ericsson.com
*/


#ifndef GRAPH
#define GRAPH

#include <vector>
#include <stack>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <memory>
#include <climits>

#include "stats.h"

extern "C" {
#include <graphviz/gvc.h>
}

using namespace std;

extern inline void print_path(const vector <int> &path);

template <typename T>
class Graph
{
    /***
    * Data member storing the number of nodes in a graph
    */
    int _nr_of_nodes;

    /***
    * Data member storing the distance (weights) between two adjacent nodes
    */
    vector<vector<T>> _weights;

    /***
    * Data member used to build up the shortest paths between the nodes
    */

    vector<vector<bool>> _is_link;
    vector<vector<int>> _nexts_primary;
    vector<vector<int>> _nexts_secondary;
    vector<vector<int>> _nexts_shortest;

public:

    int nth;

    /***
    *   Constructor getting an input file_name as const char* and reading the _nr_of_nodes and weights.
    *   @param file_name as const char*
    */
    Graph(const char * input_file);
    /***
    * Copy constructor
    */
    Graph(const Graph<T>& );

    /***
    * Move constructor
    */
    Graph(Graph<T>&& rhs) noexcept;

    /***
    * Member function returning a clone of the current object.
    */
    Graph clone() const;

    /***
    * Copy assignment operator
    */
    Graph<T>& operator=(const Graph<T>& rhs) = default;

    /***
    * Move assignment operator
    */
    Graph<T>& operator=(Graph<T>&& rhs) noexcept = default;

    /***
    * Function returning the number of nodes in a graph.
    */
    inline int get_nr_of_nodes()
    {
        return _nr_of_nodes;
    }

    /***
    * Function printing the paths between each node in a human readable format.
    * NOTE!: in memory node numbers are stored indexing from 0. When printed, these numbers are increased with one.
    */
    void print_paths() const;

    /***
    * Function printing the contents of _weigths data member.
    */
    void print_graph_dists() const;

    /***
    * Function printing the contents of _nexts data member.
    */
    void print_graph_nexts() const;

    vector<int> get_primary_path(int i, int j) const;

    vector<int> get_secondary_path(int i, int j) const;

    void draw(const char *fname) const;

    void get_paths_between(int i, int j, vector<vector<int>> &paths, stack<int> &&path, vector<bool> && seen);

    void R_F_W();

    int get_nr_of_labels(const vector<int> & path);
};

template <typename T>
Graph<T>::Graph(const char * input_file)
{
    ifstream fin;
    fin.exceptions ( ifstream::failbit | ifstream::badbit );
    try
    {
        fin.open (input_file);
        fin>>_nr_of_nodes;
        _weights.reserve(_nr_of_nodes);
        _nexts_primary.reserve(_nr_of_nodes);
        _nexts_secondary.reserve(_nr_of_nodes);
        _nexts_shortest.reserve(_nr_of_nodes);
        _is_link.reserve(_nr_of_nodes);
        for(int i=0; i<_nr_of_nodes; ++i)
        {
            _weights[i].reserve(_nr_of_nodes);
            _nexts_primary[i].reserve(_nr_of_nodes);
            _nexts_secondary[i].reserve(_nr_of_nodes);
            _nexts_shortest[i].reserve(_nr_of_nodes);
            _is_link[i].reserve(_nr_of_nodes);
            for(int j=0; j<_nr_of_nodes; ++j)
            {
                fin>>_weights[i][j];
                if(_weights[i][j])
                {
                    _nexts_primary[i][j]=j;
                    _nexts_secondary[i][j]=j;
                    _nexts_shortest[i][j]=j;
                    _is_link[i][j]=true;
                }
                else
                {
                    if(i==j)
                    {
                        _nexts_primary[i][j]=j;
                        _nexts_secondary[i][j]=j;
                        _nexts_shortest[i][j]=j;
                        _is_link[i][j]=true;
                    }
                    else
                    {
                        _nexts_primary[i][j]=-1;
                        _nexts_secondary[i][j]=-1;
                        _nexts_shortest[i][j]=-1;
                        _is_link[i][j]=false;
                    }
                }
            }
        }
        fin.close();
    }
    catch (ifstream::failure e)
    {
        cerr << "Exception opening/reading/closing file\n";
        exit(EXIT_FAILURE);
    }

}

template <typename T>
Graph<T>::Graph(const Graph<T>& rhs):_nr_of_nodes(rhs._nr_of_nodes), _weights(rhs._weights),
        _nexts_primary(rhs._nexts_primary), _nexts_secondary(rhs._nexts_secondary)
        ,_nexts_shortest(rhs._nexts_shortest),_is_link(rhs._is_link)
{
    cout<<"cpy-ctor\n";
}

template <typename T>
Graph<T> Graph<T>::clone() const
{
    return Graph<T>(*this);
}

template <typename T>
Graph<T>::Graph(Graph<T>&& rhs) noexcept : _nr_of_nodes(rhs._nr_of_nodes), _weights(move(rhs._weights)),
        _nexts_primary(move(rhs._nexts_primary)), _nexts_secondary(move(rhs._nexts_secondary))
        ,_nexts_shortest(move(rhs._nexts_shortest)),_is_link(move(rhs._is_link))
{
    cout<<"move-ctor\n";
}

template <typename T>
void Graph<T>::print_graph_dists() const
{
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        for(int j=0; j<_nr_of_nodes ; ++j)
        {
            cout<<_weights[i][j]<<' ';
        }
        cout<<endl;
    }
}

template <class T>
vector<int> Graph<T>::get_primary_path(int i, int j) const
{

}

template <class T>
vector<int> Graph<T>::get_secondary_path(int i, int j) const
{

}

template <class T>
void Graph<T>::print_paths() const
{
    for (int i = 0; i < _nr_of_nodes - 1; i++)
    {
        for (int j = i+1; j < _nr_of_nodes; j++)
        {
            cout<<i+1<<"->"<<j+1<<endl;
            std::vector<int> a{get_primary_path(i,j)};
            for(int k : a)
            {
                cout<<k+1<<' ';
            }
            cout<<endl;

            a = move(get_secondary_path(i,j));

            for(int k : a)
            {
                cout<<k+1<<' ';
            }
            cout<<endl<<endl;
        }
    }
}

template <typename T>
void Graph<T>::print_graph_nexts() const
{
    cout<<"Primary paths interfaces: "<<endl;
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        for(int j=0; j<_nr_of_nodes ; ++j)
        {
            cout<<_nexts_primary[i][j]+1<<' ';
        }
        cout<<endl;
    }
    cout<<endl<<"Secondary paths interfaces: "<<endl;

    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        for(int j=0; j<_nr_of_nodes ; ++j)
        {
            cout<<_nexts_secondary[i][j]+1<<' ';
        }
        cout<<endl;
    }
    cout<<endl;
}

template <typename T>
void Graph<T>::R_F_W()
{
    for (int k = 0; k < _nr_of_nodes; k++)
    {
        for (int i = 0; i < _nr_of_nodes; i++)
        {
            for (int j = 0; j < _nr_of_nodes; j++)
            {
                if(i!=j && i!=k && j!=k)
                {
                    if ((_weights[i][k] + _weights[k][j] < _weights[i][j] ||  !_weights[i][j]) && ( _weights[i][k] && _weights[k][j]))
                    {
                        _weights[i][j] = _weights[i][k] + _weights[k][j];

                        _nexts_shortest[i][j]=_nexts_shortest[i][k];
                    }
                }
            }
        }
    }
}

template<typename T, class C>
C& getProtectedContainer(std::stack<T, C>& s)
{
   struct OpenStack : std::stack<T, C> {
      using std::stack<T, C>::c;
   };
   return static_cast<OpenStack&>(s).c;
}

template <typename T>
void Graph<T>::get_paths_between(int i, int j,vector<vector<int>> &paths,
        stack<int> &&path, vector<bool> && seen)
{
    path.push(i);
    seen[i]=true;

    if(i == j)
    {
        auto& c = getProtectedContainer(path);
        paths.emplace_back(vector<int>(c.begin(), c.end()));
        path.pop();
        return;
    }

    for(int k = 0; k<_nr_of_nodes; ++k)
    {
        if(k!=i)
        {
            if(_is_link[i][k] && (!seen[k]))
            {
                seen[k]=true;
                get_paths_between(k, j, paths, move(path), move(seen));
                seen[k]=false;
            }
        }
    }
    path.pop();
    seen[i]=false;
}

template <typename T>
int Graph<T>::get_nr_of_labels(const vector<int> & path)
{
    int labels_needed = 0;
    int last_label_destination = path[path.size()-1];
    for(int i = path.size()-2; i >= 0; i--)
    {
        if(_nexts_shortest[path[i]][last_label_destination] != path[i+1])
        {
            last_label_destination = path[i+1];
            ++labels_needed;
            ++i;
//            cout<<"Increasing at "<<path[i]+1<<endl;
        }
    }
    return labels_needed;
}

template <typename T>
void Graph<T>::draw(const char *fname) const
{
    Agraph_t *g;
    vector<Agnode_t*> nodes;

    /* set up a graphviz context - but only once even for multiple graphs */
    static GVC_t *gvc;

    gvc = gvContext();

    /* Create a simple digraph */
    g = agopen((char*)"g", Agdirected, 0);
    nodes.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes; ++i)
    {
        nodes[i] = agnode(g, &to_string(i+1)[0], 1);
    }
    for (int i = 0; i < _nr_of_nodes; i++)
        for (int j = 0; j < _nr_of_nodes; j++)
            if(_weights[i][j])
            {
                Agedge_t *e=agedge(g, nodes[i], nodes[j], (char*)"", 1);
                agsafeset(e, "label", &to_string(_weights[i][j])[0], "");
            }

    /* Use the directed graph layout engine */
    gvLayout(gvc, g, (char*)"neato");

    /* Output in .dot format */
    gvRenderFilename(gvc, g, (char*)"png", fname);

    gvFreeLayout(gvc, g);

    agclose(g);

}

#endif // GRAPH
