#ifndef GRAPH_H
#define GRAPH_H

#include<bits/stdc++.h>
#include <QSet>
#include <QHash>
#include <QList>
using namespace std;
# define INF 0x3f3f3f3f
typedef pair<int, int> iPair;

class Graph
{
    int V;    // No. of vertices
    QHash<int,int> mapping;
    QHash<int,int> revMapping;
    // In a weighted graph, we need to store vertex
    // and weight pair for every edge
    list< pair<int, int> > *adj;
public:
    Graph(int V,QList<quint32> vertices,int node);  // Constructor

    // function to add an edge to graph
    void addEdge(int u, int v, int w);

    // Print MST using Prim's algorithm
    QSet<int> primMST(int);
};

#endif // GRAPH_H
