#include <iostream>

#include "bplustree.h"

using namespace std;

void BPlusTree::insert(int key, record *r)
{
    cout << "Inserting id " << r->tconst << " with value " << r->numVotes << " at address " << r << endl;
}