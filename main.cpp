#include <iostream>
#include <string>
#include <vector>

#include "storage.h"

using namespace std;

int main()
{
    int block_size = 100;

    Disk disk(block_size);
    disk.import_tsv("data.tsv");
    disk.print_records();

    return 0;
}