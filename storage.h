#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>

struct record
{
    int record_id;
    char tconst[9];
    float averageRating;
    int numVotes;
};

struct block
{
    record *records;
};

class Disk
{
    private:
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
        int get_records_per_block();
        record *get_record(int block_index, int record_index);
        void print_info();
        void print_block(int n);
        void import_tsv(std::string filename);
};

#endif