//////////////////////////////////////////////////////////
// compile with:  mpic++ -std=c++11 ping_pong.cpp
//////////////////////////////////////////////////////////

#include <iostream>
#include <ctime>
#include <chrono>

using namespace std;

#include "mpi.h"

#define NTRIALS 200

#undef DELAY


int main (int argc, char **argv) {

    int me, numinstances, i, len, min_len, max_len, partner;

    MPI_Status status[2];
    int tag = 4;

    unsigned char *buffer;

    chrono::high_resolution_clock::time_point t1,t2;
    chrono::duration<double> diff;
    double min;

    typedef struct {
        int x;
        double y;
    } tr;
    tr *record;

    int idx;


    MPI_Init(&argc, &argv);

    min_len = atoi(argv[1]);
    max_len = atoi(argv[2]);

    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    MPI_Comm_size(MPI_COMM_WORLD, &numinstances);
    partner = (me + 1) % numinstances;

    record = new tr[5000];
    buffer = new unsigned char[max_len];

    MPI_Barrier(MPI_COMM_WORLD);

    idx = 0;
    len = min_len;

    MPI_Request req[2];

    while (len <= max_len) {

    min = 1e100;

    for (i=0;i<NTRIALS;i++) {
        if (me == 0) {
            t1 = chrono::high_resolution_clock::now();
            MPI_Isend(buffer,len,MPI_CHAR,partner,tag,MPI_COMM_WORLD, &req[0]);
            MPI_Irecv(buffer,len,MPI_CHAR,partner,tag,MPI_COMM_WORLD, &req[1]);
            MPI_Waitall(2, req, status);
            t2 = chrono::high_resolution_clock::now();
            diff = t2 - t1;
            if (diff.count() < min) min = diff.count();
        } else {
            MPI_Isend(buffer,len,MPI_CHAR,partner,tag,MPI_COMM_WORLD, &req[0]);
            MPI_Irecv(buffer,len,MPI_CHAR,partner,tag,MPI_COMM_WORLD, &req[1]);
            MPI_Waitall(2, req, status);
        }
    }

    min = min/2;

    if (me == 0) {
	record[idx].x = len;
#ifdef DELAY
        record[idx].y = min*1e6;
#else
        if (!len)
            record[idx].y = min*1e6;
        else
            record[idx].y = len/(min*1e6);
#endif
    }

    idx++;
    if (len < 8000) len += 4;
    else if (len <29000) len += 1000;
    else if (len <29900) len += 100;
    else if (len <30100) len += 4;
    else if (len <31000) len += 100;
    else if (len <100000) len += 1000;
    else len += 10000;

    }

    if (me == 0) {
        for (i=0; i<idx; i++)
            cout << record[i].x << " " << record[i].y << endl;
    }

    delete [] record;
    delete [] buffer;

    MPI_Finalize();

    return 0;

}
