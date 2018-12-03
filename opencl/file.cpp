#include "file.h"

#include <iostream>
#include <fstream>


void EmptyMatrix(double* matrix, int size_i, int size_j) {
	for (int i=0; i<size_i*size_j; i++)
		matrix[i]=0;
}

void ReadMatrix(double* matrix, string filename, int size_i, int size_j) {
	ifstream file(filename.c_str());

	for(int i=0; i<size_i*size_j; i++) {
		int row, col;
		double val;

		file >> row >> col >> val;
		matrix[row*size_j + col] = val;
	}

	file.close();
}


void PrintMatrix(double* matrix, int size_i, int size_j, string filename) {
	ofstream file;
	if(!filename.empty())
		file.open(filename.c_str());

	for(int row=0; row<size_i; row++)
		for(int col=0; col<size_j; col++) {
			if(!filename.empty())
				file << row << ' ' << col << ' ' << matrix[row*size_j + col] << endl;
			else
				cout << row << ' ' << col << ' ' << matrix[row*size_j + col] << endl;
		}

	if(!filename.empty())
		file.close();
}

void PrintMatrix_Nice(double* matrix, int size_i, int size_j, string filename) {
	ofstream file;
	if(!filename.empty())
		file.open(filename.c_str());

	for(int row=0; row<size_i; row++) {
		for(int col=0; col<size_j; col++) {
			if(!filename.empty())
				file << matrix[row*size_j + col] << '\t';
			else
				cout << matrix[row*size_j + col] << '\t';
		}

		if(!filename.empty())
			file << endl;
		else
			cout << endl;
	}

	if(!filename.empty())
		file.close();
}

