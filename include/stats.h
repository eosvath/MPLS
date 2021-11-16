#ifndef STATS_H_INCLUDED
#define STATS_H_INCLUDED

#include <iostream>

class LabelStatistics
{
    int total;
    int number_of_labels_needed[6];

    public:
    void reset()
    {
        total = 0;
        for (int i=0 ;i<6;++i)
        {
            number_of_labels_needed[i]=0;
        }
    }
    void increase_labels_needed(int nr_of_labels_needed)
    {
        ++total;
        if(nr_of_labels_needed>4)
            ++number_of_labels_needed[5];
        else
            ++number_of_labels_needed[nr_of_labels_needed];

    }
    void print_stats()
    {
        std::cout<<"\nTotal times rerouting was needed: "<<total<<std::endl;
        std::cout<<"Number of labels required: ";
        for (int i=0; i<5 ;i++)
        {
            std::cout<<i<<" ";
        }
        std::cout<<"4<";
        std::cout<<"\nTimes this many were required: ";
        for (int i=0; i<6 ;i++)
        {
            std::cout<<number_of_labels_needed[i]<<" ";
        }
        std::cout<<"\nPercentage this many were required: ";
        for (int i=0; i<6 ;i++)
        {
            std::cout<<((double)number_of_labels_needed[i])/total*100<<"% ";
        }
        std::cout<<std::endl;
    }
};

class Statistics
{
    int total_true=0, out_of=0;
public:
    void add(bool to_be_counted)
    {
        if(to_be_counted)
            total_true++;
        out_of++;
    }
    double get_stats()
    {
        return static_cast<double>(total_true)/out_of*100;
    }
};

#endif // STATS_H_INCLUDED
