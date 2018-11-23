#include <iostream>
#include <fstream>
#include <string>

using namespace std;


#define SZ 3


double* ReadMatrix(char* filename)
{
	ifstream file(filename);

	double* matrix= new double[SZ*SZ];

	for(int i=0; i<SZ*SZ; i++)
	{
		int row, col;
		double val;

		file >> row >> col >> val;
		matrix[row*SZ + col] = val;
	}

	file.close();
	return matrix;
}


void PrintMatrix(double* matrix, char* filename=NULL)
{
	ofstream file;
	if(filename)
		file.open(filename);

	for(int row=0; row<SZ; row++)
		for(int col=0; col<SZ; col++)
		{
			if(filename)
				file << row << ' ' << col << ' ' << matrix[row*SZ + col] << endl;
			else
				cout << row << ' ' << col << ' ' << matrix[row*SZ + col] << endl;
		}

	if(filename)
		file.close();
}

void PrintMatrix_Nice(double* matrix, char* filename=NULL)
{
	ofstream file;
	if(filename)
		file.open(filename);

	for(int row=0; row<SZ; row++)
	{
		for(int col=0; col<SZ; col++)
		{
			if(filename)
				file << matrix[row*SZ + col] << ' ';
			else
				cout << matrix[row*SZ + col] << ' ';
		}

		if(filename)
			file << endl;
		else
			cout << endl;
	}


	if(filename)
		file.close();
}




int main()
{
	double* matrix = ReadMatrix("in");
	PrintMatrix_Nice(matrix);
}
