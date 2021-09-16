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
    int number_of_records, block_index, record_index;
    record *some_record;
    bool found;
    
    some_record = NULL;

    for (int i=0; i<100; i++)
    {
        some_record = disk.get_record(i / 5, i % 5);
        if (some_record == NULL)
        {
            cout << "Record " << i << " not found!" << endl;
        }
        else
        {
            cout << "Record " << i << " contains:" << endl;
            cout << '\t' << some_record->tconst << '\t' << some_record->averageRating << '\t' << some_record->numVotes << endl;
            b_plus_tree.insert(some_record->numVotes, some_record);
        }
    }
    b_plus_tree.print_tree();

    return 0;
}