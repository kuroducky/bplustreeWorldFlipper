#include <iostream>
#include <string>
#include <vector>

#include "storage.h"

using namespace std;

int main()
{
    int block_size = 100;

    // Pack records into a block, and store it in a disk
    Disk disk(block_size);
    disk.import_tsv("data.tsv");
    disk.print_records();

    // Initialize records sequentially
    int number_of_records, block_index, record_index;
    record some_record;
    bool found;

    number_of_records = disk.get_num_records();
    block_index = 4;
    record_index = 2;
    some_record = disk.get_record(block_index, record_index, found);
    if (found)
    {
        cout << "Block " << block_index << " record " << record_index << " contains:" << endl;
        cout << '\t' << some_record.tconst << '\t' << some_record.averageRating << '\t' << some_record.numVotes << endl;
    }

    return 0;
}