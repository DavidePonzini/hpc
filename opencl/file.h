#pragma once

#include <string>

using namespace std;

void ReadMatrix(double* matrix, string filename, int size);
void PrintMatrix(double* matrix, int size, string filename="");
void PrintMatrix_Nice(double* matrix, int size, string filename="");
