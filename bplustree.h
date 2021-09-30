#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include "storage.h"

class Node
{
private:
    int *keys, size;
    bool is_leaf;
    void **children;

public:
    Node(int max_nodes, bool is_leaf);
    ~Node();

    void print_contents();

    friend class BPlusTree;
};

class BPlusTree
{
private:
    int keys_per_node, num_nodes;
    Node *root;

    void insert_non_leaf(Node *node, Node *child, int key);
    int remove_non_leaf(Node *node, Node *child, int key);
    Node *find_parent(Node *node);

public:
    BPlusTree(int block_size);
    ~BPlusTree();

    void insert(int key, record *r);
    int remove(int key);
    void find(int key, std::vector<record *> &records, std::vector<Node *> &index_nodes);
    void find(int start_key, int end_key, std::vector<record *> &records, std::vector<Node *> &index_nodes);
    int get_height();
    void print_info();
};

#endif