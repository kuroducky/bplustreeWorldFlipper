#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>

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
    std::vector<block> blocks;

public:
    // Constructor
    Disk(int block_size);

    // Destructor
    ~Disk();

    // Methods
    void insert(record r);
    int get_num_records();
    record get_record(int block_index, int record_index, bool &found);
    void print_records();
    void print_records(int n);
    void import_tsv(std::string filename);
};

#endif