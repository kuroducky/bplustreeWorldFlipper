#include <iostream>

#include "bplustree.h"

using namespace std;

Node::Node(int max_nodes, bool is_leaf)
{
    this->max_nodes = max_nodes;
    this->keys = new int[max_nodes];
    this->size = 0;
    this->is_leaf = is_leaf;
    this->children = new void *[max_nodes + 1];

    for (int i = 0; i <= max_nodes; i++)
    {
        this->children[i] = NULL;
    }
}

Node::~Node()
{
    delete[] keys;
    delete[] children;
}

void Node::print_contents()
{
    cout << "Keys: ";
    for (int i = 0; i < size; i++)
    {
        cout << keys[i] << " ";
    }
    cout << endl;
}

BPlusTree::BPlusTree(int block_size)
{
    keys_per_node = (block_size - sizeof(void *)) / (sizeof(int) + sizeof(void *));
    num_nodes = 0;
    root = NULL;
}

BPlusTree::~BPlusTree()
{
    if (root == NULL)
        return;

    if (!root->is_leaf)
    {
        for (int i = 0; i < root->size; i++)
        {
            delete (Node *)root->children[i];
        }
    }
    delete root;
}

// BPlusTree removal
int BPlusTree::remove(int key)
{
    int total_count=0, count = 0;

    // if B+ tree is non-existent, nothing can be deleted
    if (root == NULL)
    {
        cout << "Tree is empty. Nothing to delete" << endl;
        return -1;
    }

    Node *ptr, *parent;
    int i, j, leftSibling, rightSibling;

    // Traversing to the leaf nodes to that might contain the key
    ptr = root;
    parent = NULL;
    while (!ptr->is_leaf)
    {
        for (i = 0; i < ptr->size && key > ptr->keys[i]; i++);
        parent = ptr;
        leftSibling = i - 1;
        rightSibling = i + 1;
        ptr = (Node *)ptr->children[i];
    }

    //Once we reached the leaf nodes, we search for the key if it exists
    bool found = false;
    int pos;
    //Finding the correct key in leaf nodes
    for (pos = 0; pos < ptr->size; pos++)
    {
        if (ptr->keys[pos] == key)
        {
            found = true;
            break;
        }
    }
    //Find one more time !
    if (!found)
    {
        ptr = (Node *)ptr->children[keys_per_node];
        for (pos = 0; pos < ptr->size; pos++)
        {
            if (ptr->keys[pos] == key)
            {
                found = true;
                break;
            }
        }
    }
    //If key is not found in the correct leaf nodes
    if (!found)
    {
        cout << "Key not found" << endl;
        return -1;
    }

    //Performing deletion
    cout << "Deleting record " << ((record *)ptr->children[pos])->tconst << endl;
    for (int i = pos; i < ptr->size - 1; i++)
    {
        ptr->keys[i] = ptr->keys[i + 1];
        ptr->children[i] = ptr->children[i + 1];
    }
    ptr->size--;

    //Ensure that there is no underflow ie, root node must have at least 1 value and
    //all other nodes must have at least (m-1)/2 values
    if (ptr->size >= (keys_per_node + 1) / 2) //no underflow
    {
        return 0;
    }
    cout << "Underflow in leaf node" << endl;

    //If underflow occurs, perform shifting
    // Check if left sibling exists
    if (leftSibling >= 0)
    {
        //Performing the transfer
        Node *leftNode = (Node *)parent->children[leftSibling];
        if (leftNode->size >= (keys_per_node + 1) / 2 + 1)
        {
            for (i = ptr->size; i > 0; i--)
            {
                ptr->keys[i] = ptr->keys[i - 1];
                ptr->children[i] = ptr->children[i - 1];
            }
            //shift pointer to next leaf
            ptr->size++;

            //transfer
            ptr->keys[0] = leftNode->keys[leftNode->size - 1];
            ptr->children[0] = leftNode->children[leftNode->size - 1];
            leftNode->size--;

            //update parent
            parent->keys[leftSibling] = ptr->keys[0];
            cout << "Transferred " << ptr->keys[0] << " from left sibling of leaf node\n";
            return 0;
        }
    }

    if (rightSibling <= parent->size) //check if right sibling exist
    {
        Node *rightNode = (Node *)parent->children[rightSibling];
        //check if it is possible to transfer
        if (rightNode->size >= (keys_per_node + 1) / 2 + 1)
        {
            //shift pointer to next leaf
            ptr->size++;

            //transfer
            ptr->keys[ptr->size - 1] = rightNode->keys[0];
            ptr->children[ptr->size - 1] = rightNode->children[0];
            //shift pointer of rightsibling
            rightNode->size--;

            //shift content of right sibling
            for (int i = 0; i < rightNode->size; i++)
            {
                rightNode->keys[i] = rightNode->keys[i + 1];
                rightNode->children[i] = rightNode->children[i + 1];
            }
            //update parent
            parent->keys[rightSibling - 1] = rightNode->keys[0];
            cout << "Transferred " << ptr->keys[ptr->size - 1] << " from right sibling of leaf node\n";
            return 0;
        }
    }

    //must merge and delete a node
    if (leftSibling >= 0) //if left sibling exist
    {
        Node *leftNode = (Node *)parent->children[leftSibling];
        // transfer all keys to leftsibling and then transfer pointer to next leaf node
        for (i = leftNode->size, j = 0; j < ptr->size; i++, j++)
        {
            leftNode->keys[i] = ptr->keys[j];
            leftNode->children[i] = ptr->children[j];
        }
        leftNode->size += ptr->size;
        leftNode->children[keys_per_node] = ptr->children[keys_per_node];
        cout << "Merging two leaf nodes" << endl;
        count = remove_non_leaf(parent->keys[leftSibling], parent, ptr, count); // delete parent node key
        delete ptr;
    }
    else if (rightSibling <= parent->size) //if right sibling exist
    {
        Node *rightNode = (Node *)parent->children[rightSibling];
        // transfer all keys to cursor and then transfer pointer to next leaf node
        for (i = ptr->size, j = 0; j < rightNode->size; i++, j++)
        {
            ptr->keys[i] = rightNode->keys[j];
            ptr->children[i] = ptr->children[j];
        }
        ptr->size += rightNode->size;
        ptr->children[keys_per_node] = rightNode->children[keys_per_node];
        cout << "Merging two leaf nodes\n";
        total_count++;
        count = remove_non_leaf(parent->keys[pos], parent, rightNode, count); // delete parent node key
        delete rightNode;
    }
    return total_count + count;
}

int BPlusTree::remove_non_leaf(int key, Node *curr, Node *child, int count)
{
    //deleting the key x first
    //checking if key from root is to be deleted
    if (curr == root)
    {
        if (curr->size == 1) //if only one key is left, change root
        {
            if (curr->children[1] == child)
            {
                delete child;
                root = (Node *)curr->children[0];
                delete curr;
                cout << "Changed root node\n";
                count += 2;
                return count;
            }
            else if (curr->children[0] == child)
            {
                delete child;
                root = (Node *)curr->children[1];
                delete curr;
                cout << "Changed root node\n";
                count += 2;
                return count;
            }
        }
    }

    int pos, i, j;

    for (pos=0; pos < curr->size && curr->children[pos] != child; pos++);
    if (pos == curr->size)
        pos--;
    for (i = pos; i < curr->size - 1; i++)
    {
        curr->keys[i] = curr->keys[i + 1];
        curr->children[i] = curr->children[i + 1];
    }
    // // now deleting the pointer child
    // for (pos = 0; pos < curr->size + 1; pos++)
    // {
    //     if (curr->children[pos] == child)
    //     {
    //         break;
    //     }
    // }
    // for (int i = pos; i < curr->size + 1; i++)
    // {
    //     curr->children[i] = curr->children[i + 1];
    // }
    curr->size--;
    if (curr->size >= (keys_per_node + 1) / 2 - 1 || curr == root) //no underflow or node is root
    {
        cout << "Deleted " << key << " from internal node successfully" << endl;
        return count;
    }

    cout << "Underflow in internal node!" << endl;

    //underflow, try to transfer first
    Node *parent = find_parent(curr);
    int leftSibling, rightSibling;

    //finding left n right sibling of cursor
    for (pos = 0; pos < parent->size && parent->children[pos] != curr; pos++);
    leftSibling = pos - 1;
    rightSibling = pos + 1;
    // for (pos = 0; pos < parent->size + 1; pos++)
    // {
    //     if (parent->children[pos] == curr)
    //     {
    //         leftSibling = pos - 1;
    //         rightSibling = pos + 1;
    //         break;
    //     }
    // }

    //try to transfer
    if (leftSibling >= 0) //if left sibling exists
    {
        Node *leftNode = (Node *)parent->children[leftSibling];
        //check if it is possible to transfer
        if (leftNode->size >= keys_per_node / 2 + 1)
        {
            //make space for transfer of key
            curr->children[curr->size+1] = curr->children[curr->size];
            for (i = curr->size; i > 0; i--)
            {
                curr->keys[i] = curr->keys[i - 1];
                curr->children[i] = curr->children[i - 1];
            }

            //transfer key from left sibling through parent
            curr->keys[0] = parent->keys[leftSibling];
            parent->keys[leftSibling] = leftNode->keys[leftNode->size - 1];

            //transfer ptr
            curr->children[0] = leftNode->children[leftNode->size];

            //transfer last pointer from leftnode to cursor
            //make space for transfer of ptr
            // for (int i = curr->size + 1; i > 0; i--)
            // {
            //     curr->children[i] = curr->children[i - 1];
            // }
            curr->size++;
            leftNode->size--;
            cout << "Transferred " << curr->keys[0] << " from left sibling of internal node\n";
            return count;
        }
    }
    if (rightSibling <= parent->size) //check if right sibling exist
    {
        Node *rightNode = (Node *)parent->children[rightSibling];
        //check if it is possible to transfer
        if (rightNode->size >= keys_per_node / 2 + 1)
        {
            //transfer key from right sibling through parent
            curr->keys[curr->size] = parent->keys[pos];
            parent->keys[pos] = rightNode->keys[0];
            
            //transfer first pointer from rightnode to cursor
            curr->children[curr->size + 1] = rightNode->children[0];

            //transfer keys
            for (i = 0; i < rightNode->size - 1; i++)
            {
                rightNode->keys[i] = rightNode->keys[i + 1];
                rightNode->children[i] = rightNode->children[i + 1];
            }
            rightNode->children[i] = rightNode->children[i + 1];
            
            //transfer ptr
            // for (int i = 0; i < rightNode->size - 1; ++i)
            // {
            //     rightNode->children[i] = rightNode->children[i + 1];
            // }

            curr->size++;
            rightNode->size--;
            cout << "Transferred " << curr->keys[0] << " from right sibling of internal node\n";
            return count;
        }
    }
    //transfer wasnt posssible hence do merging
    if (leftSibling >= 0)
    {
        //leftnode + parent key + cursor
        Node *leftNode = (Node *)parent->children[leftSibling];

        leftNode->keys[leftNode->size] = parent->keys[leftSibling];
        for (i = leftNode->size + 1, j = 0; j < curr->size; j++)
        {
            leftNode->keys[i] = curr->keys[j];
            leftNode->children[i] = curr->children[j];
        }
        // for (i = leftNode->size + 1, j = 0; j < curr->size + 1; j++)
        // {
        //     leftNode->children[i] = curr->children[j];
        //     curr->children[j] = NULL;
        // }
        leftNode->size += curr->size + 1;
        curr->size = 0;
        //delete cursor
        count = remove_non_leaf(parent->keys[leftSibling], parent, curr, count);
        cout << "Merged with left sibling\n";
    }
    else if (rightSibling <= parent->size)
    {
        //cursor + parent key + rightnode
        Node *rightNode = (Node *)parent->children[rightSibling];
        curr->keys[curr->size] = parent->keys[rightSibling - 1];
        for (i = curr->size + 1, j = 0; j < rightNode->size; j++)
        {
            curr->keys[i] = rightNode->keys[j];
            curr->children[i] = rightNode->children[j];
        }
        // for (int i = curr->size + 1, j = 0; j < rightNode->size + 1; j++)
        // {
        //     curr->children[i] = rightNode->children[j];
        //     rightNode->children[j] = NULL;
        // }
        curr->size += rightNode->size + 1;
        rightNode->size = 0;
        //delete cursor
        count = remove_non_leaf(parent->keys[rightSibling - 1], parent, rightNode, count);
        cout << "Merged with right sibling\n";
    }
    return count;
}

void BPlusTree::insert(int key, record *r)
{
    // Creating new B+ tree
    if (root == NULL)
    {
        root = new Node(keys_per_node, true);
        root->keys[0] = key;
        root->children[0] = r;
        root->size++;
        num_nodes++;

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
        for (i = 0; i < ptr->size && key >= ptr->keys[i]; i++)
            ;
        // Traverse to next level
        parent = ptr;
        ptr = (Node *)ptr->children[i];
    }

    // Directly insert
    if (ptr->size < keys_per_node)
    {
        // Find position to insert key
        for (i = 0; i < ptr->size && key >= ptr->keys[i]; i++)
            ;

        // Shift rest of the keys and pointers
        for (j = ptr->size; j > i; j--)
        {
            ptr->keys[j] = ptr->keys[j - 1];
            ptr->children[j] = ptr->children[j - 1];
        }

        // Insert new record
        ptr->keys[i] = key;
        ptr->children[i] = r;
        ptr->size++;
        num_nodes++;
    }
    // Need to split
    else
    {
        Node *new_node = new Node(keys_per_node, true);

        // Fix rightmost pointer of each node
        new_node->children[keys_per_node] = ptr->children[keys_per_node];
        ptr->children[keys_per_node] = new_node;

        // Find size of each node
        new_node->size = (keys_per_node + 1) / 2;
        ptr->size = keys_per_node + 1 - new_node->size;
        num_nodes++;

        // Find position to insert key
        bool inserted = false;
        for (i = keys_per_node - 1, j = new_node->size - 1; j >= 0; i--, j--)
        {
            // Inserting to new node
            if (key >= ptr->keys[i] && !inserted)
            {
                new_node->keys[j] = key;
                new_node->children[j] = r;
                inserted = true;
                j--;
                if (j < 0)
                    break;
            }
            new_node->keys[j] = ptr->keys[i];
            new_node->children[j] = ptr->children[i];
        }

        if (!inserted)
        {
            // Inserting to old node
            for (i = ptr->size - 1; i > 0 && key < ptr->keys[i - 1]; i--)
            {
                ptr->keys[i] = ptr->keys[i - 1];
                ptr->children[i] = ptr->children[i - 1];
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
        for (i = 0; i < node->size && key >= node->keys[i]; i++)
            ;

        // Shift rest of the keys and pointers
        for (j = node->size; j > i; j--)
        {
            node->keys[j] = node->keys[j - 1];
            node->children[j + 1] = node->children[j];
        }

        // Insert new record
        node->keys[i] = key;
        node->children[i + 1] = new_child;
        node->size++;
    }

    // Need to split
    else
    {
        Node *new_node = new Node(keys_per_node, false);

        // New key to be added to parent node
        int new_key;

        // Find size of each node
        new_node->size = keys_per_node / 2;
        node->size = keys_per_node - new_node->size;

        // Find position to insert key
        bool inserted = false;
        for (i = keys_per_node - 1, j = new_node->size - 1; j >= 0; i--, j--)
        {
            // Inserting to new node
            if (key >= node->keys[i] && !inserted)
            {
                new_node->keys[j] = key;
                new_node->children[j + 1] = new_child;
                inserted = true;
                j--;
                if (j < 0)
                    break;
            }
            new_node->keys[j] = node->keys[i];
            new_node->children[j + 1] = node->children[i + 1];
        }

        new_key = node->keys[i];
        new_node->children[0] = node->children[i + 1];

        if (!inserted)
        {
            if (key >= node->keys[i])
            {
                new_key = key;
                new_node->children[0] = new_child;
            }
            else
            {
                // Inserting to old node
                for (i = node->size - 1; i > 0 && key < node->keys[i - 1]; i--)
                {
                    node->keys[i] = node->keys[i - 1];
                    node->children[i + 1] = node->children[i];
                }
                node->keys[i] = key;
                node->children[i + 1] = new_child;
            }
        }

        insert_non_leaf(find_parent(node), new_node, new_key);
    }
}

Node *BPlusTree::find_parent(Node *node)
{
    if (node == NULL)
        return NULL;

    Node *ptr, *parent;
    int i, key;

    ptr = root;
    parent = NULL;
    key = node->keys[0];

    while (ptr != node)
    {
        // Reached the leaf node but still havent found parent, means no parent
        if (ptr->is_leaf)
            return NULL;

        parent = ptr;
        for (i = 0; i < ptr->size && key >= ptr->keys[i]; i++)
            ;
        ptr = (Node *)ptr->children[i];
    }
    return parent;
}

void BPlusTree::find(int key, vector<record *> &records, std::vector<Node *> &index_nodes, std::vector<block *> &record_blocks)
{
    find(key, key, records, index_nodes, record_blocks);
}

void BPlusTree::find(int start_key, int end_key, std::vector<record *> &records, std::vector<Node *> &index_nodes, std::vector<block *> &record_blocks)
{
    if (root == NULL)
        return;

    Node *ptr;
    int i;

    ptr = root;
    while (!ptr->is_leaf)
    {
        // Record non-leaf index nodes
        index_nodes.push_back(ptr);
        for (i = 0; i < ptr->size && start_key > ptr->keys[i]; i++)
            ;
        ptr = (Node *)ptr->children[i];
    }
    // Record leaf index node
    index_nodes.push_back(ptr);

    // Finding starting key
    for (i = 0; i < ptr->size && start_key > ptr->keys[i]; i++)
        ;
    if (ptr->keys[i] != start_key && i == ptr->size)
    {
        ptr = (Node *)ptr->children[keys_per_node];
        i = 0;
    }

    // Retrieving data from start key to end key
    while (ptr && ptr->keys[i] <= end_key)
    {
        records.push_back((record *)ptr->children[i]);
        i++;
        if (i >= ptr->size)
        {
            ptr = (Node *)ptr->children[keys_per_node];
            i = 0;
        }
    }
}

int BPlusTree::get_height()
{
    if (root == NULL)
        return 0;

    Node *ptr;
    int count;

    ptr = root;
    count = 1;

    while (!ptr->is_leaf)
    {
        ptr = (Node *)ptr->children[0];
        count++;
    }
    return count;
}

void BPlusTree::print_info()
{
    Node *ptr;

    cout << "n (maximum keys per node): " << keys_per_node << endl;
    cout << "Number of nodes: " << num_nodes << endl;
    cout << "Height of tree: " << get_height() << endl;

    cout << "Contents of root:" << endl;
    for (int i = 0; i < root->size; i++)
    {
        cout << '\t' << root->keys[i];
    }
    cout << endl;

    cout << "Contents of children:" << endl;
    if (root->is_leaf)
    {
        cout << "NULL";
    }
    else
    {
        for (int i = 0; i <= root->size; i++)
        {
            ptr = (Node *)root->children[i];
            for (int j = 0; j < ptr->size; j++)
            {
                cout << '\t' << ptr->keys[j];
            }
            cout << endl;
        }
    }
}