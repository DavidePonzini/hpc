#include <iostream>
#include <ctime>
#include <chrono>

using namespace std;

#include <omp.h>

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
	
#ifdef DEBUG2
	int t_no = omp_get_thread_num();
	cout << t_no << '\t' << low << '\t' << high << endl;
#endif
	
    if (low < high) {                
        int mid = (low + high) / 2;
        mergesort(arr, temp, low, mid);
        mergesort(arr, temp, mid+1, high);
        merge(arr, temp, low, mid, high);
    }
}


void mergesort1 (int* arr, int *temp, int low, int high) {
	int threads_no = omp_get_num_threads();
	int elems_no = high-low;
	
	printf("low: %d, high: %d\n", low, high);
	printf("elems: %d, threads: %d\n", elems_no, threads_no);
	
	if(threads_no > elems_no)
		threads_no = elems_no;
	
	float step = elems_no / threads_no;
	
    if (low < high) {
		float i=low;
		for (int t=0; t < threads_no; t++, i += step)
		{
			#pragma omp task
			{
#ifdef DEBUG
	int t_no = omp_get_thread_num();
	printf("thread %d from %f to %f\n", t_no, i+1, i+step);
#endif	
				//mergesort(arr, temp, i, i+step-1);
			}
			
			
		}
		
        #pragma omp taskwait
        //merge(arr, temp, low, mid, high);
    }
}


int main (int argc, char * argv[]) {

#ifdef DEBUG
	#undef TRIALS
	#define TRIALS 1
	
	#undef SIZE
	#define SIZE 10
#endif

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

        #pragma omp parallel
        #pragma omp single
        {
            init = chrono::high_resolution_clock::now();
            mergesort1(toBeSorted, temp, 0, SIZE-1);
            end = chrono::high_resolution_clock::now();
        }
        
        diff = end - init;        
                
        if (diff.count() < min)
            min = diff.count();
                
#ifdef DEBUG3
/*debug; use it only for small SIZE*/
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
