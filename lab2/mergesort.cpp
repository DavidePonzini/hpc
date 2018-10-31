#include <iostream>
#include <ctime>
#include <chrono>

using namespace std;

#define TRIALS 30
#define SIZE 10000000


void merge (int* arr, int *temp, int low, int mid, int high) {

    int i=low;
    int j=mid+1;
    int k=low;
        
    while (i<=mid && j<=high) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }
        
    while (i<=mid)
        temp[k++] = arr[i++];
                
    while (j<=high)
        temp[k++] = arr[j++];
                
    for (i=low; i<k; i++)
        arr[i] = temp[i];
}


void mergesort (int* arr, int *temp, int low, int high) {
    if (low < high) {                
        int mid = (low + high) / 2;
        mergesort(arr, temp, low, mid);
        mergesort(arr, temp, mid+1, high);
        merge(arr, temp, low, mid, high);
    }
}


int main (int argc, char * argv[]) {

    int i;
    double min = 1.0e100;
    chrono::high_resolution_clock::time_point init,end;
    chrono::duration<double> diff;
        
    int *toBeSorted = new int[SIZE];
    if (toBeSorted == NULL) return -1;
    int *temp = new int[SIZE];
    if (temp == NULL) return -1;
        
// worst case: init with reversed order values
    for (i=0; i<SIZE; i++)
        toBeSorted[i] = SIZE-i;
        
    for (i=0; i<TRIALS; i++) {

        init = chrono::high_resolution_clock::now();
        mergesort(toBeSorted, temp, 0, SIZE-1);
        end = chrono::high_resolution_clock::now();

        diff = end - init;        
                
        if (diff.count() < min)
            min = diff.count();

#if 0
// debug; use it only for small SIZE
        int z;
        for (z=0; z<SIZE-1; z++)
            if (toBeSorted[z] > toBeSorted[z+1])
                printf ("ERR %d %d %d\n", z, toBeSorted[z], toBeSorted[z+1]);
        for (z=0; z<SIZE; z++)
            printf ("%d \n", toBeSorted[z]);
        printf ("################################\n");
#endif
                
    }
        
    cout << SIZE << " " << min << endl;
        
    delete [] toBeSorted; delete [] temp;
        
    return 0;        

}
