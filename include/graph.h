#ifndef GRAPH
#define GRAPH

#include <vector>
#include <fstream>
#include <iostream>
#include <utility>
#include <memory>

#include "stats.h"

extern "C" {
#include <graphviz/gvc.h>
}
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
    std::vector<std::vector<T>> _weights;

    /***
    * Data member used to build up the shortest paths between the nodes
    */
    std::vector<std::vector<int>> _nexts;
    /***
    * Ddata member used to store the shortest paths between nodes
    */
    std::vector<std::vector<std::vector<int>>> _paths;

    //Graph();
    /***
     *   member function used by R_F_W() to generate the contents of _paths data member when shortest distances and _nexts are caluculated by the modified Roy-Floyd-Warshall algorithm
    */
    void get_paths();
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
    void R_F_W();
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
    template <typename T2>
    void friend check_changed_paths_TI_LFA(const Graph<T2> &graph1, const Graph<T2> &graph2, Statistics &stats, const bool &quiet);

    void draw(const char *fname) const;

};

template <typename T>
Graph<T>::Graph(const char * input_file)
{

    std::ifstream fin(input_file);
    fin>>_nr_of_nodes;
    _weights.resize(_nr_of_nodes);
    _nexts.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes; ++i)
    {
        _weights[i].resize(_nr_of_nodes);
        _nexts[i].resize(_nr_of_nodes);
        _paths[i].resize(_nr_of_nodes);
        for(int j=0; j<_nr_of_nodes; ++j)
        {
            fin>>_weights[i][j];
            if(_weights[i][j])
            {
                _nexts[i][j]=j;
            }
            else
            {
                if(i==j)
                {
                    _nexts[i][j]=j;
                }
                else
                {
                    _nexts[i][j]=-1;
                }
            }
        }
    }
}

template <typename T>
Graph<T>::Graph(const Graph<T>& rhs)
{
    //std::cout<<"cpy-ctor\n";
    _nr_of_nodes = rhs._nr_of_nodes;
    _weights.resize(_nr_of_nodes);
    _nexts.resize(_nr_of_nodes);
    _paths.resize(rhs._paths.size());
    for(int i=0; i<_nr_of_nodes ; ++i)
    {

        _weights[i]=rhs._weights[i];
        _nexts[i]=rhs._nexts[i];
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
    return std::move(Graph<T>(*this));
}

template <typename T>
Graph<T>::Graph(Graph<T>&& rhs) noexcept
{
    //std::cout<<"move-ctor\n";
    _nr_of_nodes = std::move(rhs._nr_of_nodes);
    _weights.resize(_nr_of_nodes);
    _nexts.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        _weights[i] = std::move(rhs._weights[i]);
        _nexts[i] = std::move(rhs._nexts[i]);

        _paths[i].resize(rhs._paths[i].size());
        for(int j=0; j<_paths[i].size(); ++j)
        {
            _paths[i][j]=std::move(rhs._paths[i][j]);
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
            std::cout<<_weights[i][j]<<' ';
        }
        std::cout<<std::endl;
    }
}

template <typename T>
void Graph<T>::print_graph_nexts() const
{
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        for(int j=0; j<_nr_of_nodes ; ++j)
        {
            std::cout<<_nexts[i][j]<<' ';
        }
        std::cout<<std::endl;
    }
}


template <typename T>
Graph<T>& Graph<T>:: operator=(const Graph<T>& rhs)
{
    std::cout<<"cpy-assignment-op\n";
    _nr_of_nodes = rhs._nr_of_nodes;
    _weights.resize(_nr_of_nodes);
    _nexts.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        _weights[i] = rhs._weights[i];
        _nexts[i] = rhs._nexts[i];
        _paths[i].resize(rhs._paths[i].size());
        for(int j=0; j<_paths[i][j].size(); ++j)
        {
            _paths[i][j]=rhs._paths[i][j];
        }
    }
    return *this;
}

template <typename T>
Graph<T>& Graph<T>:: operator=(Graph<T>&& rhs) noexcept
{
    std::cout<<"move-assignment-op\n";
    _nr_of_nodes = std::move(rhs._nr_of_nodes);
    _weights.resize(_nr_of_nodes);
    _nexts.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        _weights[i] = std::move(rhs._weights[i]);
        _nexts[i] = std::move(rhs._nexts[i]);
        _paths[i].resize(rhs._paths[i].size());
        for(int j=0; j< rhs._paths[i].size(); ++j)
        {
            _paths[i][j] = std::move(rhs._paths[i][j]);
        }
    }

    return *this;
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
                if(i!=j)
                {
                    if ((_weights[i][k] + _weights[k][j] < _weights[i][j] ||  !_weights[i][j]) && ( _weights[i][k] && _weights[k][j]))
                    {
                        _weights[i][j] = _weights[i][k] + _weights[k][j];
                        _nexts[i][j]=_nexts[i][k];
                    }
                }
            }
        }
    }
    get_paths();
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
            while(k!=j && _nexts[k][j]>=0)
            {
                _paths[i][j].push_back(k);
                k=_nexts[k][j];
            }
            if(_nexts[k][j]>=0)
            {
                _paths[i][j].push_back(k);
            }
        }
    }
}

template <class T>
void Graph<T>::print_paths() const
{
    for(int i=0; i<_paths.size(); ++i)
    {
        for(int j=0; j<_paths.size(); ++j)
        {
            if(i!=j)
            {
                std::cout<<i+1<<".->"<<j+1<<". : ";
                for(int k=0; k<_paths[i][j].size(); ++k)
                {
                    std::cout<<_paths[i][j][k]+1<<' ';
                }
                std::cout<<std::endl;
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
            _weights[i][node]=_weights[node][i]=0;
            _nexts[i][node] = -1;
            _nexts[node][i] = -1;
        }
    }
    else
    {
        throw "Invalid node index to remove!";
    }
}

template <typename T>
void Graph<T>::remove_link(int node1, int node2)
{
    if(node1>=0 && node1<_nr_of_nodes && node2>=0 && node2<_nr_of_nodes)
    {
        _weights[node2][node1]=_weights[node1][node2]=0;
        _nexts[node1][node2] = -1;
        _nexts[node2][node1] = -1;
    }
    else
    {
        throw "Invalid node index in link to remove!";
    }
}

template <typename T>
void check_changed_paths_TI_LFA(const Graph<T> &graph1, const Graph<T> &graph2, Statistics &stats, const bool &quiet)
{
    for(int i=0; i<graph2._nr_of_nodes; ++i)
    {
        for(int j=0; j<graph2._nr_of_nodes; ++j)
        {
            if(i!=j)
            {
                if(graph1._paths[i][j].size() && graph2._paths[i][j].size())
                {
                    if(graph1._paths[i][j] != graph2._paths[i][j])
                    {
                        /*
                        for(auto it:graph1._paths[i][j])
                        {
                            std::cout<<it+1<<' ';
                        }
                        std::cout<<std::endl;
                        for(auto it:graph2._paths[i][j])
                        {
                            std::cout<<it+1<<' ';
                        }
                        std::cout<<std::endl;
                        */
                        std::vector<int> differing_path = graph1._paths[i][j];
                        int path_index = 0;
                        int last_ok = differing_path[path_index];
                        while (graph2._paths[differing_path[path_index]][differing_path[path_index+1]].size()==2)
                        {
                            last_ok = differing_path[++path_index];
                        }

                        for (int k = 0; k < graph2._paths[last_ok][j].size(); ++k)
                        {
                            if( ++path_index < differing_path.size())
                            {
                                differing_path[path_index] = graph2._paths[last_ok][j][k];
                            }
                            else
                            {
                                differing_path.emplace_back(graph2._paths[last_ok][j][k]);
                            }
                        }
                        int counter = 0;
                        std::vector<int> sub_path = graph2._paths[last_ok][j];

                        if(!quiet)
                        {
                            std::cout<<std::endl<<"i = "<<i+1<<", j = "<<j+1<<std::endl;
                            for(auto it:sub_path)
                            {
                                std::cout<<it+1<<' ';
                            }
                            std::cout<<std::endl;
                        }
                        int last_dest = sub_path[sub_path.size()-1];
                        bool null_segment = false;

                        for (int k = sub_path.size()-2; k >= 0; --k)
                        {
                            if(graph1._paths[sub_path[k]][last_dest] != graph2._paths[sub_path[k]][last_dest])
                            {
                                if(k == 0 && counter == 0)
                                {
                                    if(!quiet)
                                        std::cout<<"0 segment! Sending packet to "<<last_dest+1<<" from node "<<sub_path[0]+1<<" via node "<<sub_path[1]+1<<std::endl;
                                    null_segment = true;
                                    break;
                                }
                                else
                                {
                                    if(k!=0)
                                    {
                                        if(!quiet)
                                            std::cout<<sub_path[k+1]+1<<". reroutes to "<<last_dest+1<<"."<<std::endl;
                                        ++counter;
                                        last_dest = sub_path[k+1];
                                    }
                                }
                            }
                        }
                        if(!quiet)
                        {
                            if(!null_segment)
                            {
                                std::cout<<sub_path[0]+1<<". reroutes to "<<last_dest+1<<"."<<std::endl;
                            }
                            std::cout<<"Number of labels required: "<<counter<<std::endl;
                        }
                        stats.increase_labels_needed(counter);
                    }
                }
            }
        }
    }
}

template <typename T>
void Graph<T>::draw(const char *fname) const
{
    Agraph_t *g;
    std::vector<Agnode_t*> nodes;

    /* set up a graphviz context - but only once even for multiple graphs */
    static GVC_t *gvc;

  	gvc = gvContext();

    /* Create a simple digraph */
    g = agopen((char*)"g", Agdirected, 0);
    nodes.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes; ++i)
    {
        nodes[i] = agnode(g, &std::to_string(i+1)[0], 1);
    }
    for (int i = 0; i < _nr_of_nodes; i++)
        for (int j = 0; j < _nr_of_nodes; j++)
            if(_weights[i][j])
            {
                Agedge_t *e=agedge(g, nodes[i], nodes[j], (char*)"", 1);
                agsafeset(e, "label", &std::to_string(_weights[i][j])[0], "");
            }

   /* Use the directed graph layout engine */
    gvLayout(gvc, g, (char*)"neato");

    /* Output in .dot format */
    gvRenderFilename(gvc, g, (char*)"png", fname);

    gvFreeLayout(gvc, g);

    agclose(g);

}

#endif // GRAPH


