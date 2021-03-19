#ifndef GRAPH
#define GRAPH

#include <vector>
#include <fstream>
#include <iostream>
#include <utility>
#include <memory>

template <typename T>
class Graph{
    int _nr_of_nodes;
    std::vector<std::vector<T>> _weights;
    std::vector<std::vector<int>> _nexts;
    std::vector<std::vector<std::vector<int>>> _paths;
    Graph();
    void get_paths();
public:
    Graph(const char *);
    Graph(const Graph<T>& );
    Graph(Graph<T>&& rhs) noexcept;
    Graph clone() const;
    Graph<T>& operator=(const Graph<T>& rhs);
    Graph<T>& operator=(Graph<T>&& rhs) noexcept;
    void R_F_W();
    void print_paths();
    void print_graph_dists() const;
    void print_graph_nexts() const;
    void remove_node(int);
    void remove_link(int,int);
    template <typename T2>
    void friend check_changed_paths(const Graph<T2> &graph1, const Graph<T2> &graph2);

};

template <typename T>
Graph<T>::Graph()
{
    std::cout<<"empty-ctor\n";
    _nr_of_nodes = 0;
}

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
    std::cout<<"cpy-ctor\n";
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
    std::cout<<"move-ctor\n";
    _nr_of_nodes = std::move(rhs._nr_of_nodes);
    _weights.resize(_nr_of_nodes);
    _nexts.resize(_nr_of_nodes);
    _paths.resize(_nr_of_nodes);
    for(int i=0; i<_nr_of_nodes ; ++i)
    {
        _weights[i] = std::move(rhs._weights[i]);
        _nexts[i] = std::move(rhs._nexts[i]);

        _paths[i].resize(rhs._paths[i].size());
        for(int j=0;j<_paths[i].size();++j)
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
Graph<T>& Graph<T>:: operator=(const Graph<T>& rhs) {
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
        for(int j=0;j<_paths[i][j].size();++j)
        {
            _paths[i][j]=rhs._paths[i][j];
        }
    }
    return *this;
}

template <typename T>
Graph<T>& Graph<T>:: operator=(Graph<T>&& rhs) noexcept{
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
        for(int j=0; j< rhs._paths[i].size();++j)
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
void Graph<T>::print_paths()
{
    for(int i=0; i<_paths.size(); ++i)
    {
        for(int j=i+1; j<_paths.size(); ++j)
        {
            std::cout<<i+1<<".->"<<j+1<<". : ";
            for(int k=0;k<_paths[i][j].size();++k)
            {
                std::cout<<_paths[i][j][k]+1<<' ';
            }
            std::cout<<std::endl;
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
    else{
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
    else{
        throw "Invalid node index in link to remove!";
    }
}

template <typename T>
void check_changed_paths(const Graph<T> &graph1, const Graph<T> &graph2){
    for(int i=0; i<graph1._nr_of_nodes; ++i)
    {
        for(int j=i+1; j<graph2._nr_of_nodes; ++j)
        {
            if(graph1._paths[i][j].size() && graph2._paths[i][j].size())
            {
                if(graph1._paths[i][j] != graph2._paths[i][j])
                {
                    std::cout<<std::endl<<"i = "<<i+1<<", j = "<<j+1<<std::endl;
                    //copy new route
                    std::vector<int> differing_path = graph2._paths[i][j];
                    int k=0, rerouting_node, reroute_destination_node;
                    //delete common part with the old part from the beginning, remember last common node from beginning of new path
                    for(; graph1._paths[i][j][k] == graph2._paths[i][j][k] ; rerouting_node = differing_path[0], differing_path.erase(differing_path.begin()), k++);
                    //delete common part with the old part from the end, remember first common node from end of new path
                    for(k=1; graph1._paths[i][j][graph1._paths[i][j].size()-k] == graph2._paths[i][j][graph2._paths[i][j].size()-k] && differing_path.size() > 0; reroute_destination_node = differing_path[differing_path.size()], differing_path.erase(differing_path.end()-1),++k);
                    //std::cout<<rerouting_node<<std::endl;
                    for(auto it:differing_path)
                    {
                        std::cout<<it+1<<' ';
                    }
                    //std::cout<<std::endl<<reroute_destination_node<<std::endl;
                    std::cout<<std::endl;
                    k=0;
                    differing_path.insert(differing_path.begin(),rerouting_node);
                    int counter=0, index = differing_path.size();
                    while(index >= 0)
                    {
                        if( graph1._paths[differing_path[index]][reroute_destination_node] == graph2._paths[differing_path[index]][reroute_destination_node])
                        {
                            index--;
                            continue;
                        }
                        else{
                            if(index+1<differing_path.size())
                            {
                                std::cout<<differing_path[index+1]+1<<". reroutes to "<<reroute_destination_node+1<<std::endl;
                                reroute_destination_node = differing_path[index+1];
                                ++counter;
                            }
                            else{
                                throw "This theoretically should never happen!";
                            }
                        }
                    }
                    std::cout<<differing_path[index+1]+1<<". reroutes to "<<reroute_destination_node+1<<std::endl;
                    std::cout<<"Number of labels required: "<<counter<<std::endl;
                }
            }
        }
    }
}

#endif // GRAPH


