/**
* @file graph.h
* @Copyrigth Ericsson Ltd. © 2021
* @author egon.csaba.osvath@ericsson.com
*/


#ifndef GRAPH
#define GRAPH

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <memory>
#include <climits>
#include <algorithm>
#include <exception>

#include "stats.h"

extern "C" {
#include <graphviz/gvc.h>
}

using namespace std;

extern ofstream fout;

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
    vector<vector<int>> _nexts_primary;
    vector<vector<int>> _nexts_secondary;

    /***
    * Data member used to store the shortest paths between nodes
    */
    std::vector<std::vector<std::vector<int>>> _paths;


    /***
     *   member function used by R_F_W() to generate the contents of _paths data member when shortest distances and _nexts are caluculated by the modified Roy-Floyd-Warshall algorithm
    */
    void get_paths();

    int minDistance(int dist[], bool sptSet[]);

    vector<int> dijkstra(int src, int dest);

public:
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
    Graph<T>& operator=(const Graph<T>& rhs);

    /***
    * Move assignment operator
    */
    Graph<T>& operator=(Graph<T>&& rhs) noexcept;

    /***
    * Roy-Floyd-Warshall algorithm, finding the shortest distances and paths in the current graph object.
    * Modifies, all data members except _nr_of_nodes.
    */
    void R_F_W(bool secondaries = false);
    /***
    * Function returning the number of nodes in a graph.
    */
    int get_nr_of_nodes()
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

    /***
    * Removes all links connected with node given in @param
    * @param node to remove from graph (removes only links connected with the node)
    */
    void remove_node(int);

    /***
    * Removes a single link from the graph. The link is determined using the @params, which determine the nodes the link connects to.
    *
    * Both parameter has to be between 0 (>=) and _nr_of_nodes (<).
    *
    * @param node1 - endpoint one of the link to remove
    * @param node2 - endpoint two of the link to remove
    */
    void remove_link(int,int);

    /***
    * Friend function that given 2 graphs determines the difference in the shortest paths when using MPLS TI_LFA local repair
    * Only those differences will be printed where a path exists.
    * When determined the different path, the function determines with an iteratively, how many labels are required in the graph1 to get the shortest path in graph2.
    * When finished it prints the number of labels needed, and the nodes that help in rerouting.
    *
    * NOTE!: This method should be called after R_F_W() was called on both graphs, because it uses the _paths data member, which is configured in that function.
    *
    * WARNING!: The difference is checked comparing graph2 to graph1, so here the order of the parameters counts, since it will generate a different output.
    *
    */
//    template <typename T2>
//    void friend check_changed_paths_TI_LFA(const Graph<T2> &graph1, const Graph<T2> &graph2, Statistics &stats, const bool &quiet);
    void calculate_secondaries(const Graph<T>&);

    vector<int> remove_link_temporarily(int i, int j){
        vector<int> result;

        for(int k = 0; k<_nr_of_nodes; ++k)
        {
            result.emplace_back(_nexts_primary[i][k]);
        }
        for(int k = 0; k<_nr_of_nodes; ++k)
        {
            result.emplace_back(_nexts_primary[j][k]);
        }
        for(int k = 0; k<_nr_of_nodes; ++k)
        {
            if(_nexts_primary[i][k] == j)
            {
                _nexts_primary[i][k] = -1;
            }
            if(_nexts_primary[j][k] == i)
            {
                _nexts_primary[j][k] = -1;
            }
        }
        return result;
    }

    void restore_link(vector<int> tmp, int i, int j){
        for(int k = 0; k<_nr_of_nodes; ++k){
            _nexts_primary[i][k] = tmp[k];
        }
        for(int k = _nr_of_nodes; k<2*_nr_of_nodes; ++k){
            _nexts_primary[j][k-_nr_of_nodes] = tmp[k];
        }
    }

    void get_paths_between_only_primary(int i, int j, vector<int> &path) const;

    void get_paths_between_secondary(int i, int j, vector<int> &path) const;

    void get_paths_between(int i, int j, vector<int> &path) const;

    void print_same_paths() const;

    void draw(const char *fname) const;

    const std::vector<int> &get_path(int i, int j)
    {
        return _paths[i][j];
    }

    double get_secondary_stats();

    T get_weight(int i, int j);

};

template <typename T>
T Graph<T>::get_weight(int i, int j)
{
    return _weights[i][j];
}


template <typename T>
Graph<T>::Graph(const char * input_file)
{
    ifstream fin;
    fin.exceptions ( ifstream::failbit | ifstream::badbit );
    try
    {
        fin.open (input_file);
        fin>>_nr_of_nodes;
        _weights.resize(_nr_of_nodes);
        _nexts_primary.resize(_nr_of_nodes);
        _nexts_secondary.resize(_nr_of_nodes);
        _paths.resize(_nr_of_nodes);
        for(int i=0; i<_nr_of_nodes; ++i)
        {
            _weights[i].resize(_nr_of_nodes);
            _nexts_primary[i].resize(_nr_of_nodes);
            _nexts_secondary[i].resize(_nr_of_nodes);
            _paths.resize(_nr_of_nodes);
            for(int j=0; j<_nr_of_nodes; ++j)
            {
                fin>>_weights[i][j];
                if(_weights[i][j])
                {
                    _nexts_primary[i][j]=j;
                    _nexts_secondary[i][j]=j;
                }
                else
                {
                    if(i==j)
                    {
                        _nexts_primary[i][j]=j;
                        _nexts_secondary[i][j]=j;
                    }
                    else
                    {
                        _nexts_primary[i][j]=-1;
                        _nexts_secondary[i][j]=-1;
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
Graph<T>::Graph(const Graph<T>& rhs)
{
    //fout<<"cpy-ctor\n";
    _nr_of_nodes = rhs._nr_of_nodes;
    _weights.resize(_nr_of_nodes);
    _nexts_primary.resize(_nr_of_nodes);
    _nexts_secondary.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        _weights[i]=rhs._weights[i];
        _nexts_primary[i]=rhs._nexts_primary[i];
        _nexts_secondary[i]=rhs._nexts_secondary[i];
        _paths[i].resize(rhs._paths[i].size());
        for(int j=0; j<_paths[i].size() ; ++j)
        {
            _paths[i][j]=rhs._paths[i][j];
        }
    }
}

template <typename T>
Graph<T> Graph<T>::clone() const
{
    return move(Graph<T>(*this));
}

template <typename T>
Graph<T>::Graph(Graph<T>&& rhs) noexcept
{
    //fout<<"move-ctor\n";
    _nr_of_nodes = move(rhs._nr_of_nodes);
    _weights.resize(_nr_of_nodes);
    _nexts_primary.resize(_nr_of_nodes);
    _nexts_secondary.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        _weights[i] = move(rhs._weights[i]);
        _nexts_primary[i] = move(rhs._nexts_primary[i]);
        _nexts_secondary[i] = move(rhs._nexts_secondary[i]);
        for(int j=0; j<_paths[i].size() ; ++j)
        {
            _paths[i][j]=rhs._paths[i][j];
        }
    }
}

template <typename T>
void Graph<T>::print_graph_dists() const
{
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        for(int j=0; j<_nr_of_nodes ; ++j)
        {
            fout<<_weights[i][j]<<' ';
        }
        fout<<endl;
    }
}

template <typename T>
void Graph<T>::print_graph_nexts() const
{
    fout<<"Primary outgoing interfaces: "<<endl;
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        for(int j=0; j<_nr_of_nodes ; ++j)
        {
            fout<<_nexts_primary[i][j]+1<<' ';
        }
        fout<<endl;
    }
    fout<<endl<<"Secondary outgoing interfaces: "<<endl;

    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        for(int j=0; j<_nr_of_nodes ; ++j)
        {
            fout<<_nexts_secondary[i][j]+1<<' ';
        }
        fout<<endl;
    }
    fout<<endl;
}


template <typename T>
Graph<T>& Graph<T>:: operator=(const Graph<T>& rhs)
{
//    fout<<"cpy-assignment-op\n";
    _nr_of_nodes = rhs._nr_of_nodes;
    _weights.resize(_nr_of_nodes);
    _nexts_primary.resize(_nr_of_nodes);
    _nexts_secondary.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        _weights[i] = rhs._weights[i];
        _nexts_primary[i] = rhs._nexts[i];
        _nexts_secondary[i] = rhs._nexts[i];
        for(int j=0; j<_paths[i].size() ; ++j)
        {
            _paths[i][j]=rhs._paths[i][j];
        }
    }
    return *this;
}

template <typename T>
Graph<T>& Graph<T>:: operator=(Graph<T>&& rhs) noexcept
{
//    fout<<"move-assignment-op\n";
    _nr_of_nodes = move(rhs._nr_of_nodes);
    _weights.resize(_nr_of_nodes);
    _nexts_primary.resize(_nr_of_nodes);
    _nexts_secondary.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        _weights[i] = move(rhs._weights[i]);
        _nexts_primary[i] = move(rhs._nexts_primary[i]);
        _nexts_secondary[i] = move(rhs._nexts_secondary[i]);
        for(int j=0; j<_paths[i].size() ; ++j)
        {
            _paths[i][j]=rhs._paths[i][j];
        }
    }

    return *this;
}

template <class T>
void Graph<T>::get_paths()
{
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes; ++i)
    {
        _paths[i].resize(_nr_of_nodes);
        for(int j=0; j<_nr_of_nodes; ++j)
        {
            _paths[i][j].resize(0);
            int k=i;
            while(k!=j && _nexts_primary[k][j]>=0)
            {
                _paths[i][j].push_back(k);
                k=_nexts_primary[k][j];
            }
            if(_nexts_primary[k][j]>=0)
            {
                _paths[i][j].push_back(k);
            }
        }
    }
}

template <typename T>
void Graph<T>::R_F_W(bool secondaries)
{
    Graph<int> clone_original(this->clone());

    for (int k = 0; k < _nr_of_nodes; k++)
    {
        for (int i = 0; i < _nr_of_nodes; i++)
        {
            for (int j = 0; j < _nr_of_nodes; j++)
            {
                if(i!=j&& k!=i)
                {
                    if ((_weights[i][k] + _weights[k][j] < _weights[i][j] ||  !_weights[i][j]) && (_weights[i][k] && _weights[k][j]))
                    {
                        _weights[i][j] = _weights[i][k] + _weights[k][j];
                        _nexts_primary[i][j]=_nexts_primary[i][k];
                    }
                }
            }
        }
    }
    if(secondaries)
    {
        calculate_secondaries(clone_original);
    }

    get_paths();
}

template <typename T>
void Graph<T>::calculate_secondaries(const Graph<T>& before_R_F_W)
{
    for (int i = 0; i < _nr_of_nodes; i++)
    {
        for (int j = 0; j < _nr_of_nodes; j++)
        {
            if(i!=j)
            {
                Graph<int> clone(before_R_F_W.clone());
                clone.remove_link(i, _nexts_primary[i][j]);

                vector<int> secondary_to(clone.dijkstra(i,j));
//                fout<<"Dijkstra "<<i+1<<"->"<<j+1<<":";
//                for(auto p : secondary_to)
//                {
//                    fout<<p+1<<" ";
//                }
//                fout<<endl;

                if(!secondary_to.empty() && secondary_to.size() >= 2)
                {
                    int k = 0;
                    while(k < secondary_to.size() && _nexts_primary[secondary_to[k]][j] != secondary_to[k+1])
                    {
                        _nexts_secondary[secondary_to[k]][j] = secondary_to[k+1];
                        k++;
                    }
                }
            }
        }
    }
}

// A utility function to find the
// vertex with minimum distance
// value, from the set of vertices
// not yet included in shortest
// path tree
template <typename T>
int Graph<T>::minDistance(int dist[], bool sptSet[])
{

    // Initialize min value
    int min = INT_MAX, min_index;

    for (int v = 0; v < _nr_of_nodes; v++)
    {
        if (sptSet[v] == false && dist[v] < min)
        {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

template <typename T>
vector<int> Graph<T>::dijkstra(int src, int dest)
{

    // The output array. dist[i]
    // will hold the shortest
    // distane from src to i
    int dist[_nr_of_nodes];

    // sptSet[i] will true if vertex
    // i is included / in shortest
    // path tree or shortest distance
    // from src to i is finalized
    bool sptSet[_nr_of_nodes];

    // Parent array to store
    // shortest path tree
    int parent[_nr_of_nodes];

    // Initialize all distances as
    // INFINITE and stpSet[] as false
    for (int i = 0; i < _nr_of_nodes; i++)
    {
        parent[i] = -1;
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    // Distance of source vertex
    // from itself is always 0
    dist[src] = 0;

    // Find shortest path
    // for all vertices
    for (int count = 0; count < _nr_of_nodes - 1; count++)
    {
        // Pick the minimum distance
        // vertex from the set of
        // vertices not yet processed.
        // u is always equal to src
        // in first iteration.
        int u = minDistance(dist, sptSet);

        // Mark the picked vertex
        // as processed
        sptSet[u] = true;

        // Update dist value of the
        // adjacent vertices of the
        // picked vertex.
        for (int v = 0; v < _nr_of_nodes; v++)

            // Update dist[v] only if is
            // not in sptSet, there is
            // an edge from u to v, and
            // total weight of path from
            // src to v through u is smaller
            // than current value of
            // dist[v]
            if (!sptSet[v] && _weights[u][v] &&
                dist[u] + _weights[u][v] < dist[v])
            {
                parent[v] = u;
                dist[v] = dist[u] + _weights[u][v];
            }
    }
    vector<int> res{};
    int end = dest;
    while(end!=src && end!=-1)
    {
        res.insert(res.begin(), end);
        end = parent[end];
    }
    if(end!=-1)
    {
        res.insert(res.begin(), src);
    }
    else
    {
        res.clear();
    }
    return res;
}


template <class T>
void Graph<T>::get_paths_between_only_primary(int i, int j, vector<int> &path) const
{
    path.push_back(i);
    if(i!=j)
    {
        get_paths_between_only_primary(_nexts_primary[i][j], j, path);
    }
}

template <class T>
void Graph<T>::get_paths_between_secondary(int i, int j, vector<int> &path) const
{
    int top = -1;
    if(path.size())
    {
        top = path.back();
    }
    path.push_back(i);
//    fout<<i+1<<" ";
    if(i!=j)
    {
        if(_nexts_primary[i][j] == top || _nexts_primary[i][j] == -1)
        {
            get_paths_between_secondary(_nexts_secondary[i][j],j,path);
        }
        else
        {
            get_paths_between(_nexts_primary[i][j],j,path);
        }

//        if(_nexts_secondary[_nexts_secondary[i][j]][j] != i)
//        {
//            get_paths_between_secondary(_nexts_secondary[i][j], j, path);
//        }
//        else
//        {
//            get_paths_between(_nexts_secondary[i][j], j, path);
//        }
    }
}

template <class T>
void Graph<T>::get_paths_between(int i, int j, vector<int> &path) const
{
    int top = -1;
    if(path.size())
    {
        top = path.back();
    }

    path.push_back(i);
//    fout<<i+1<<" ";
    if(i!=j)
    {
        if(_nexts_primary[i][j]!=-1)
        {
            get_paths_between(_nexts_primary[i][j], j, path);
        }
        else
        {
            get_paths_between_secondary(_nexts_secondary[i][j],j,path);

//            if(_nexts_secondary[_nexts_secondary[i][j]][j] != i)
//            {
//                get_paths_between_secondary(_nexts_secondary[i][j], j, path);
//            }
//            else
//            {
//                get_paths_between(_nexts_secondary[i][j], j, path);
//            }
        }
    }
}

template <class T>
void Graph<T>::print_paths() const
{
    for (int i = 0; i < _nr_of_nodes - 1; i++)
    {
        for (int j = i+1; j < _nr_of_nodes; j++)
        {
            fout<<i+1<<"->"<<j+1<<endl;
            std::vector<int> a{};
            get_paths_between_only_primary(i,j,a);

            for(int k : a)
            {
                fout<<k+1<<' ';
            }
            fout<<endl;

            a.clear();
            get_paths_between_secondary(i,j,a);

            for(int k : a)
            {
                fout<<k+1<<' ';
            }
            fout<<endl<<endl;
        }
    }
}

template <class T>
void Graph<T>::print_same_paths() const
{
    for (int i = 0; i < _nr_of_nodes - 1; i++)
    {
        for (int j = i+1; j < _nr_of_nodes; j++)
        {
            std::vector<int> a{};
            get_paths_between_only_primary(i,j,a);

            std::vector<int> b{};
            get_paths_between_secondary(i,j,b);

            if(a==b)
            {
                for(int k : b)
                {
                    fout<<k+1<<' ';
                }
                fout<<endl<<endl;
            }
        }
    }
}

template <typename T>
void Graph<T>::remove_node(int node)
{
    if(node>=0 && node<_nr_of_nodes)
    {
        for(int i=0; i<_nr_of_nodes; ++i)
        {
            for(int j=0; j<_nr_of_nodes; ++j)
            {
                if(i!=j)
                {
                    if(_nexts_primary[i][j] == node)
                    {
                        _weights[i][j]=_weights[j][i]=0;
                        _nexts_primary[i][j] = -1;
                    }
                }
            }
        }
    }
    else
    {
        std::ostringstream oss;
        oss <<"Invalid node index to remove: "<< node;
        throw runtime_error(oss.str());
    }
}

template <typename T>
void Graph<T>::remove_link(int node1, int node2)
{
    if(node1>=0 && node1<_nr_of_nodes && node2>=0 && node2<_nr_of_nodes)
    {
        _weights[node2][node1]=_weights[node1][node2]=0;
        for(int i=0;i<_nr_of_nodes;i++)
        {
            if(_nexts_primary[node1][i] == node2)
            {
                _nexts_primary[node1][i] = -1;
            }
            if(_nexts_primary[node2][i] == node1)
            {
                _nexts_primary[node2][i] = -1;
            }
        }
    }
    else
    {
        std::ostringstream oss;
        oss <<"Invalid node index in link to remove: "<< node1<<"->"<<node2<< " nr. of nodes: "<<_nr_of_nodes;
        throw runtime_error(oss.str());
    }
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


