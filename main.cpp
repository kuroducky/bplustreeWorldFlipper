#include <iostream>
#include <string>
#include <vector>

#include "storage.h"
#include "bplustree.h"

using namespace std;

void print_header(string header);

int main()
{
    /**
     * Experiment 1
     * Storing data into blocks
     */
    print_header("EXPERIMENT 1");

    int block_size = 100;

    // Pack records into a block, and store it in a disk
    Disk disk(block_size);
    disk.import_tsv("data.tsv");
    disk.print_records();

    /**
     * Experiment 2
     * Building a B+ tree
     */
    print_header("EXPERIMENT 2");

    // Inserting into B+ tree
    BPlusTree b_plus_tree(block_size);
    record *some_record;
    bool found;
    
    some_record = NULL;

    for (int i=0; i<disk.get_num_records(); i++)
    {
        some_record = disk.get_record(i / 5, i % 5);
        if (some_record != NULL)
            b_plus_tree.insert(some_record->numVotes, some_record);
    }
    b_plus_tree.print_info();
    
    /**
     * Experiment 3
     * Finding values in a B+ tree
     */
    print_header("EXPERIMENT 3");

    // Finding values
    vector<record *> records;
    vector<Node *> index_nodes;
    vector<block *> record_blocks;

    int key;
    
    key = 500;
    cout << "Searching with key: " << key << endl;
    b_plus_tree.find(key, records, index_nodes, record_blocks);

    // for(record *r : records)
    // {
    //     cout << '\t' << r->tconst << '\t' << r->averageRating << '\t' << r->numVotes << endl;
    // }
    
    for(int i=0; i<5 && i<index_nodes.size(); i++)
    {
        index_nodes[i]->print_contents();
    }
    cout << "Num records: " << records.size() << endl;

    /**
     * Experiment 4
     * Finding range of values in a B+ tree
     */
    print_header("EXPERIMENT 4");

    // Finding values
    int start_key, end_key;
    
    records.clear();
    index_nodes.clear();
    record_blocks.clear();
    
    start_key = 30000;
    end_key = 40000;
    cout << "Searching with start key: " << start_key << " and end key: " << end_key << endl;
    b_plus_tree.find(start_key, end_key, records, index_nodes, record_blocks);

    // for(record *r : records)
    // {
    //     cout << '\t' << r->tconst << '\t' << r->averageRating << '\t' << r->numVotes << endl;
    // }

    for(int i=0; i<5 && i<index_nodes.size(); i++)
    {
        index_nodes[i]->print_contents();
    }
    cout << "Num records: " << records.size() << endl;

    /**
     * Experiment 5
     * Deleting values in a B+ tree
     */
    print_header("EXPERIMENT 5");

    return 0;
}

void print_header(string header)
{
    cout << endl << "=================================" << endl;
    cout << header << endl;
    cout << "=================================" << endl;
}