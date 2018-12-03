#pragma once

#include <string>

using namespace std;

void EmptyMatrix(double* matrix, int size_i, int size_j);
void ReadMatrix(double* matrix, string filename, int size_i, int size_j);
void PrintMatrix(double* matrix, int size_i, int size_j, string filename="");
void PrintMatrix_Nice(double* matrix, int size_i, int size_j, string filename="");
