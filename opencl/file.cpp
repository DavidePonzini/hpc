#include "file.h"

#include <iostream>
#include <fstream>


void ReadMatrix(double* matrix, string filename, int size) {
	ifstream file(filename.c_str());

	for(int i=0; i<size*size; i++) {
		int row, col;
		double val;

		file >> row >> col >> val;
		matrix[row*size + col] = val;
	}

	file.close();
}


void PrintMatrix(double* matrix, int size, string filename) {
	ofstream file;
	if(!filename.empty())
		file.open(filename.c_str());

	for(int row=0; row<size; row++)
		for(int col=0; col<size; col++) {
			if(!filename.empty())
				file << row << ' ' << col << ' ' << matrix[row*size + col] << endl;
			else
				cout << row << ' ' << col << ' ' << matrix[row*size + col] << endl;
		}

	if(!filename.empty())
		file.close();
}

void PrintMatrix_Nice(double* matrix, int size, string filename) {
	ofstream file;
	if(!filename.empty())
		file.open(filename.c_str());

	for(int row=0; row<size; row++) {
		for(int col=0; col<size; col++) {
			if(!filename.empty())
				file << matrix[row*size + col] << '\t';
			else
				cout << matrix[row*size + col] << '\t';
		}

		if(!filename.empty())
			file << endl;
		else
			cout << endl;
	}

	if(!filename.empty())
		file.close();
}

