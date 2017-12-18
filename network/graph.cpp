#include "graph.h"
#include "QsLog.h"
#include <QSet>
#include <QHash>
#include <QList>
Graph::Graph(int V,QList<quint32> vertices,int node)
{

    this->V = V;
    adj = new list<iPair> [V];
    mapping.insert(node,0);
    revMapping.insert(0,node);
    QList<quint32>::iterator i;
    int val = 1;
    for (i = vertices.begin(); i != vertices.end(); ++i){
        mapping.insert(*i,val);
        revMapping.insert(val,*i);
        val++;
    }

}

void Graph::addEdge(int u, int v, int w)
{
    u = mapping.value(u);
    v = mapping.value(v);
    adj[u].push_back(make_pair(v, w));
    adj[v].push_back(make_pair(u, w));
}

QSet<int> Graph::primMST(int id)
{
    // Create a priority queue to store vertices that
    // are being preinMST. This is weird syntax in C++.
    // Refer below link for details of this syntax
    // http://geeksquiz.com/implement-min-heap-using-stl/
    priority_queue< iPair, vector <iPair> , greater<iPair> > pq;

    int src = 0; // Taking vertex 0 as source

    // Create a vector for keys and initialize all
    // keys as infinite (INF)
    vector<int> key(V, INF);

    // To store parent array which in turn store MST
    vector<int> parent(V, -1);

    // To keep track of vertices included in MST
    vector<bool> inMST(V, false);

    // Insert source itself in priority queue and initialize
    // its key as 0.
    pq.push(make_pair(0, src));
    key[src] = 0;

    /* Looping till priority queue becomes empty */
    while (!pq.empty())
    {
        // The first vertex in pair is the minimum key
        // vertex, extract it from priority queue.
        // vertex label is stored in second of pair (it
        // has to be done this way to keep the vertices
        // sorted key (key must be first item
        // in pair)
        int u = pq.top().second;
        pq.pop();

        inMST[u] = true;  // Include vertex in MST

        // 'i' is used to get all adjacent vertices of a vertex
        list< pair<int, int> >::iterator i;
        for (i = adj[u].begin(); i != adj[u].end(); ++i)
        {
            // Get vertex label and weight of current adjacent
            // of u.
            int v = (*i).first;
            int weight = (*i).second;

            //  If v is not in MST and weight of (u,v) is smaller
            // than current key of v
            if (inMST[v] == false && key[v] > weight)
            {
                // Updating key of v
                key[v] = weight;
                pq.push(make_pair(key[v], v));
                parent[v] = u;
            }
        }
    }
    QSet<int> neighbor;
    id = mapping.value(id);
    // Print edges of MST using parent array
    for(int i = 1; i < V; ++i){
        if (i==id)
            neighbor.insert(revMapping.value(parent[i]));
        else if (parent[i]==id)
            neighbor.insert(revMapping.value(i));
    }
    return neighbor;
        //QLOG_DEBUG()<<"PRINT "<<parent[i]<<"::"<< i;
}
