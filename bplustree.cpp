#include <iostream>

#include "bplustree.h"

using namespace std;

Node::Node(int max_nodes, bool is_leaf)
{
    this->keys = new int[max_nodes];
    this->size = 0;
    this->is_leaf = is_leaf;
    this->children = new void*[max_nodes+1];

    for (int i=0; i<=max_nodes; i++)
    {
        this->children[i] = NULL;
    }
}

Node::~Node()
{
    delete[] keys;
    delete[] children;
}

BPlusTree::BPlusTree(int block_size)
{
    root = NULL;
    keys_per_node = (block_size - sizeof(void*)) / (sizeof(int) + sizeof(void*));
    cout << "Keys per node: " << keys_per_node << endl;
}

BPlusTree::~BPlusTree()
{
    if (root == NULL) return;

    if (!root->is_leaf)
    {
        for (int i=0; i<root->size; i++)
        {   
            delete (Node *)root->children[i];
        }
    }
    delete root;
}

void BPlusTree::insert(int key, record *r)
{
    cout << "Inserting id " << r->tconst << " with value " << r->numVotes << " with address " << r << endl;
    if (root == NULL)
    {
        root = new Node(keys_per_node, true);
        root->keys[0] = key;
        root->children[0] = r;
        root->size++;

        return;
    }

    Node *parent, *ptr, *tmp;
    int i, j;

    parent = NULL;
    ptr = root;

    // Traverse to leaf nodes
    while (!ptr->is_leaf)
    {
        // Find position of key
        for (i=0; i<ptr->size && key >= ptr->keys[i]; i++);
        // Traverse to next level
        parent = ptr;
        ptr = (Node*)ptr->children[i];
    }

    // Directly insert
    if (ptr->size < keys_per_node)
    {
        // Find position to insert key
        for (i=0; i<ptr->size && key >= ptr->keys[i]; i++);
        
        // Shift rest of the keys and pointers
        for (j=ptr->size; j>i; j--) {
            ptr->keys[j] = ptr->keys[j-1];
            ptr->children[j] = ptr->children[j-1];
        }

        // Insert new record
        ptr->keys[i] = key;
        ptr->children[i] = r;
        ptr->size++;
    }
    // Need to split
    else {
        Node *new_node = new Node(keys_per_node, true);
        
        // Fix rightmost pointer of each node
        new_node->children[keys_per_node] = ptr->children[keys_per_node];
        ptr->children[keys_per_node] = new_node;
        
        // Find size of each node
        int left_node, right_node;

        new_node->size = (keys_per_node + 1) / 2;
        ptr->size = keys_per_node - new_node->size;

        // Find position to insert key
        bool inserted = false;
        for (i=keys_per_node-1, j=new_node->size-1; j>=0; i--, j--)
        {
            // Inserting to new node
            if (key >= ptr->keys[i] && !inserted)
            {
                new_node->keys[j] = key;
                new_node->children[j] = r;
                inserted = true;
                j--;
                if (j < 0) break;
            }
            new_node->keys[j] = ptr->keys[i];
            new_node->children[j] = ptr->children[i];
        }

        if (!inserted)
        {
            // Inserting to old node
            for (i=ptr->size-1; i>0 && key < ptr->keys[i-1]; i--)
            {
                ptr->keys[i] = ptr->keys[i-1];
                ptr->children[i] = ptr->children[i-1];
            }
            ptr->keys[i] = key;
            ptr->children[i] = r;
        }

        insert_non_leaf(parent, new_node, new_node->keys[0]);
    }
}

void BPlusTree::insert_non_leaf(Node *node, Node *new_child, int key)
{
    // Need to create new root node
    if (node == NULL)
    {
        Node *new_root = new Node(keys_per_node, false);
        new_root->keys[0] = key;
        new_root->children[0] = root;
        new_root->children[1] = new_child;
        root = new_root;
        new_root->size++;

        return;
    }

    int i, j;

    // Directly insert
    if (node->size < keys_per_node)
    {
        // Find position to insert key
        for (i=0; i<node->size && key >= node->keys[i]; i++);
        
        // Shift rest of the keys and pointers
        for (j=node->size; j>i; j--) {
            node->keys[j] = node->keys[j-1];
            node->children[j+1] = node->children[j];
        }

        // Insert new record
        node->keys[i] = key;
        node->children[i+1] = new_child;
        node->size++;
    }
    
    // Need to split
    else {
        Node *new_node = new Node(keys_per_node, false);
        
        // Find size of each node
        int left_node, right_node;

        new_node->size = keys_per_node / 2;
        node->size = keys_per_node - new_node->size;

        // Find position to insert key
        bool inserted = false;
        for (i=keys_per_node-1, j=new_node->size-1; j>=0; i--, j--)
        {
            // Inserting to new node
            if (key >= node->keys[i] && !inserted)
            {
                new_node->keys[j] = key;
                new_node->children[j+1] = new_child;
                inserted = true;
                j--;
                if (j < 0) break;
            }
            new_node->keys[j] = node->keys[i];
            new_node->children[j+1] = node->children[i+1];
        }
        new_node->children[0] = node->children[i+1];

        if (!inserted)
        {
            // Inserting to old node
            for (i=node->size-1; i>0 && key < node->keys[i-1]; i--)
            {
                node->keys[i] = node->keys[i-1];
                node->children[i+1] = node->children[i];
            }
            node->keys[i] = key;
            node->children[i+1] = new_child;
        }

        insert_non_leaf(find_parent(node), new_node, find_smallest_key(new_node));
    }
}

Node *BPlusTree::find_parent(Node *node)
{
    if (node == NULL) return NULL;

    Node *ptr, *parent;
    int i, key;

    ptr = root;
    parent = NULL;
    key = node->keys[0];

    while (ptr != node)
    {
        // Reached the leaf node but still havent found parent, means no parent
        if (ptr->is_leaf) return NULL;

        parent = ptr;
        for (i=0; i<ptr->size && key >= ptr->keys[i]; i++);
        ptr = (Node *)ptr->children[i];
    }
    return parent;
}

int BPlusTree::find_smallest_key(Node *node)
{
    if (node == NULL) return -1;

    Node *ptr;

    ptr = node;
    while (!ptr->is_leaf)
        ptr = (Node *)ptr->children[0];
    return ptr->keys[0];
}

void BPlusTree::print_tree()
{
    if (root == NULL) return;
    print_tree(root);
}

void BPlusTree::print_tree(Node *root)
{
    if (root == NULL) return;

    if (root->is_leaf)
        cout << "LEAF" << endl;

    cout << "Keys: ";
    for (int i=0; i<root->size; i++)
    {
        cout << '\t' << root->keys[i];
    }
    cout << endl;

    cout << "Children: ";
    for (int i=0; i<root->size; i++)
    {
        cout << '\t' << root->children[i];
    }
    cout << endl;

    if (!root->is_leaf)
    {
        for (int i=0; i<=root->size; i++)
        {
            print_tree((Node*)root->children[i]);
        }
    }
}