#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <cmath>

#include "storage.h"

using namespace std;

// Constructor
Disk::Disk(int block_size)
{
    this->block_size = block_size;
    this->records_per_block = block_size / sizeof(record);
    this->num_records = 0;
    this->blocks = vector<block>();
}

// Destructor
Disk::~Disk()
{
    for (int i=0; i<blocks.size(); i++)
    {
        // Free up memory
        delete[] blocks[i].records;
    }
}

// Insert record into disk
void Disk::insert(record r)
{
    int block_index, record_index;
    
    block_index = num_records / records_per_block;
    record_index = num_records % records_per_block;

    if (record_index == 0)
    {
        // Insert new block
        block new_block;
        new_block.records = new record[records_per_block];
        new_block.records[0] = r;
        blocks.push_back(new_block);
    }
    else
    {
        // Insert into last block
        record *ptr;

        ptr = blocks[block_index].records;
        ptr[record_index] = r;
    }
    num_records++;
}

int Disk::get_num_records()
{
    return num_records;
}

int Disk::get_records_per_block()
{
    return records_per_block;
}

record *Disk::get_record(int block_index, int record_index)
{
    if (block_index * records_per_block + record_index > num_records)
    {
        return NULL;
    }
    else
    {
        return &(blocks[block_index].records[record_index]);
    }
}

void Disk::print_info()
{
    cout << "Number of blocks: \t" << blocks.size() << " blocks" << endl;
    cout << "Size of database: \t" << setprecision(3) << num_records * sizeof(record) / pow(2, 20) << " MB" << endl;
}

void Disk::print_block(int n)
{
    record *ptr;

    cout << "\tBlock " << n << endl;
    for (int i=0; i<records_per_block; i++)
    {
        ptr = blocks[n].records;
        cout << "\t\t" << ptr[i].tconst << '\t' << ptr[i].numVotes << endl;
    }
}

// Import tsv
void Disk::import_tsv(string filename)
{
    ifstream myFile(filename);

    if (myFile.fail())
    {
        cout << "Could not read .tsv file" << endl;
        return;
    }

    string line;
    int record_id = 0;

    // Ignore headers
    getline(myFile, line);

    while (getline(myFile, line))
    {
        stringstream ss(line);
        record tmp_record;
        string tmp;

        // Get record id
        tmp_record.record_id = record_id;
        record_id++;

        // Get tconst
        getline(ss, tmp, '\t');
        strcpy(tmp_record.tconst, tmp.c_str());

        // Get average rating
        getline(ss, tmp, '\t');
        tmp_record.averageRating = stof(tmp);

        // Get num votes
        getline(ss, tmp, '\t');
        tmp_record.numVotes = atoi(tmp.c_str());

        // Append to result
        insert(tmp_record);
    }
    
    myFile.close();
    return;
}