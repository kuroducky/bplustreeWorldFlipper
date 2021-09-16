#include <iostream>
#include <string>
#include <vector>

#include "storage.h"
#include "bplustree.h"

using namespace std;

int main()
{
    int block_size = 100;

    // Pack records into a block, and store it in a disk
    Disk disk(block_size);
    disk.import_tsv("data.tsv");
    disk.print_records();

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

    // Finding values
    vector<record *> records;
    int key;
    
    key = 500;
    cout << "Searching with key: " << key << endl;
    b_plus_tree.find(key, records);

    for(record *r : records)
    {
        cout << '\t' << r->tconst << '\t' << r->averageRating << '\t' << r->numVotes << endl;
    }

    return 0;
}