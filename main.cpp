#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "storage.h"
#include "bplustree.h"

using namespace std;

void print_header(string header);
void print_index_nodes(vector<Node *> index_nodes, int n);
void print_record_blocks(vector<record *> records, Disk *disk, int n);
float find_average(vector<record *> records);

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
    int records_per_block;
    bool found;

    some_record = NULL;
    records_per_block = disk.get_records_per_block();
    for (int i=0; i<disk.get_num_records(); i++)
    {
        some_record = disk.get_record(i / records_per_block, i % records_per_block);
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

    int key, block_index;
    
    key = 500;
    cout << "Searching with key: " << key << endl << endl;
    b_plus_tree.find(key, records, index_nodes);

    // Print number and contents of index nodes
    print_index_nodes(index_nodes, 5);

    // Print number and contents of data blocks
    print_record_blocks(records, &disk, 5);

    // Find average of averageRating
    cout << "Average: " << setprecision(5) << find_average(records) << endl;
    
    /**
     * Experiment 4
     * Finding range of values in a B+ tree
     */
    print_header("EXPERIMENT 4");

    // Finding values
    int start_key, end_key;

    records.clear();
    index_nodes.clear();
    
    start_key = 30000;
    end_key = 40000;
    cout << "Searching with start key: " << start_key << " and end key: " << end_key << endl;
    b_plus_tree.find(start_key, end_key, records, index_nodes);

    // Print number and contents of index nodes
    print_index_nodes(index_nodes, 5);

    // Print number and contents of data blocks
    print_record_blocks(records, &disk, 5);

    // Find average of averageRating
    cout << "Average: " << setprecision(5) << find_average(records) << endl;

    /**
     * Experiment 5
     * Deleting values in a B+ tree
     */
    print_header("EXPERIMENT 5");
    int delete_key, total_count, count;

    delete_key = 1000;

    cout << "Deleting key: " << delete_key << endl;
    
    total_count = 0;
    count = 0;
    while (count != -1)
    {
        total_count += count;
        count = b_plus_tree.remove(delete_key);
    }
    cout << "Total number of times node was deleted or merged: " << total_count << endl;
    b_plus_tree.print_info();

    return 0;
}

void print_header(string header)
{
    cout << endl
         << "=================================" << endl;
    cout << header << endl;
    cout << "=================================" << endl;
}

void print_index_nodes(vector<Node *> index_nodes, int n)
{
    cout << "Number of index nodes: " << index_nodes.size() << endl;
    cout << "Contents of index nodes:" << endl;
    for(int i=0; i<n && i<index_nodes.size(); i++)
    {
        index_nodes[i]->print_contents();
    }
    cout << endl;
}

void print_record_blocks(vector<record *> records, Disk *disk, int n)
{
    int block_index, prev_index, records_per_block, count;

    records_per_block = disk->get_records_per_block();

    cout << "Number of data blocks (including duplicates): " << records.size() << endl;

    prev_index = -1;
    count = 0;
    for(int i=0; i<records.size(); i++)
    {
        block_index = records[i]->record_id / records_per_block;
        if (block_index != prev_index)
            count++;
    }
    cout << "Number of data blocks (not including duplicates): " << count << endl;

    cout << "Contents of data blocks:" << endl;
    for(int i=0; i<n && i<records.size(); i++)
    {
        block_index = records[i]->record_id / records_per_block;
        disk->print_block(block_index);
    }
    cout << endl;
}

float find_average(vector<record *> records)
{
    float sum;

    sum = 0;
    for (record *r : records)
    {
        sum += r->averageRating;
    }
    return sum / records.size();
}