#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>

using namespace std;

struct record
{
    char tconst[9];
    float averageRating;
    unsigned int numVotes;
};

struct block {
    record *records;
};

class Disk {
    // Attributes
    int block_size;
    int records_per_block;
    int num_records;
    vector<block> blocks;

public:
    // Constructor
    Disk(int block_size);

    // Destructor
    ~Disk();

    // Methods
    void insert(record r);
    void print_records();
    void print_records(int n);
    void import_tsv(string filename);
};

#endif