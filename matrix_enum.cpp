// Lab4- Enumeration
// Name- Fatima Bowers
// Due- 3/7/22 at 2am 
// Description- This program performs enumerations of square matrices, the elements can be a '.', 'X', or 'E'. The enumeration is 
parameterized by W, the number of rows, and E, the extra non-zero entries. The program takes three command line arguments: W, E, and
either an 'x' or an 'h'. It will enumerate all matrices for W and E anad print them out in one of two formats.

#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>

using namespace std;

class Matrix {
	public:
		size_t W;						// there are exactly W elements with X's and there must be an 'X' in every row and every column(Permutation Matrix)
		size_t E;						// there are exactly E elemnts that have E's in them
		size_t P;						// this is 'x' or 'h'
		vector <size_t> Perm;					// permutation of 0 .. (W-1), for the 'X' elements
		vector <size_t> Non_X;					// this is the row/col id of each of the non-X elements
		vector <size_t> E_ID;					// this is the row/col id of the E elements 
		void Print();						// print the matrix defined by W, Perm and E_ID 
		void Permute(size_t index);				// this is the recursive permuting method
		void Choose(size_t index, size_t k);			// this is the recursive n-choose-k method 
};

int main(int argc, char **argv)
{
	Matrix m;

	stringstream ss;
	char c;

	// error check for command line arg
	if(argc != 4) printf("Incorrect amount of arguments.\n");

	// set command line arguments to variables
	ss.str(argv[1]);
	ss >> m.W;
	ss.clear();
	ss.str(argv[2]);
	ss >> m.E;
	ss.clear();
	ss.str(argv[3]);
	ss >> c;
	m.P = c;

	// push elements 0 to W-1
	for(size_t i = 0; i < m.W; i++) m.Perm.push_back(i);

	// non X vector holds W^2-W potential locations for the 'E' characters
	m.Non_X.resize(m.W * m.W - m.W, '-');

	// call recursive Permute()
	m.Permute(0);

	return 0;
}

void Matrix::Print()
	//if the last argument is 'x', print the matrices as W lines of W characters that are '.', 'X', or 'E'
	//if the last argument is 'h', convert each line of the matrix to an integer, where if the element in a row is 'X' or 'E' you'll set the i-th bit of the number to 1 otherwise the i-th bit is 0, then print the integer in hex
{
	size_t i, j, k, noE = 0;
	int ints = 0;

	if(P == 'x'){

		// iterate through the rows and cols of the matrix
		for(i = 0; i < W; i++){
			for(j = 0; j < W; j++){

				// check if the location is the same as the X permutation val
				if(j == Perm[i]) printf("X");

				// check if there are any Es
				else if(E_ID.size() != 0){
					noE = 0;

					// iterate through the E_IDs and see if any match the matrix loaction
					for(k = 0; k < E_ID.size(); k++){
						if(i*W+j == E_ID[k]) printf("E");
						else noE++;
					}

					// if there were no Es to output, output a '.'
					if(noE == E_ID.size()) printf(".");
				}

				// if not X or E, output a '.'
				else printf(".");
			}
			printf("\n");
		}
	}


	if(P == 'h'){

		for(size_t i = 0; i < W; i++){

			ints = 0;
			for(size_t j = 0; j < W; j++){

				// check if the location is the same as the X perm val, set the j-th bit of ints variable to 1
				if(j == Perm[i]) ints |= (1ULL << j);

				else if(E_ID.size() != 0){

					for(size_t k = 0; k < E_ID.size(); k++){

						// check if there is a matching E_ID to the matrix location, set the j-th bit of ints variable to 1
						if(i*W+j == E_ID[k]) ints |= (1ULL << j);
						else noE++;
					}

					// if there were no Es to output, set the j-th bit of ints variable to 0
					if(noE == E_ID.size()) ints |= (0 << j);
				}

				// if not X or E, set the j-th bit of ints variable to 0
				else ints |= (0 << j);
			}

			// print ints value in hex
			printf("%x\n", ints);

		}
	}
	printf("\n");
}

void Matrix::Permute(size_t index)
	// each permutation is represented by the Perm vector of W numbers, the 'X' in row i will be specified by the element i of the vector
	// (1, 2, 0) represents
	// .X.
	// ..X
	// X..
{
	int tmp, cnt = 0;
	size_t i, j;

	// base case -- call choose() and return
	if(index == Perm.size()){

		// create non X matrix elements, stores as numbers (row * W + col)
		for(i = 0; i < Perm.size(); i++){
			for(j = 0; j < W; j++){

				if(j != Perm[i]){
					Non_X[cnt] = i*W+j;
					cnt++;
				}
			}
		}

		Choose(0, E);
		return;
	}

	// otherwise, for each element of the vector, swap it with the element in index, call Permute() recursively on the remainder of the vector, then swap it back.

	for(i = index; i < Perm.size(); i++){

		// swap Perm[index] with Perm[i]
		tmp = Perm[i];
		Perm[i] = Perm[index];
		Perm[index] = tmp;

		Permute(index+1);

		// swap back
		tmp = Perm[i];
		Perm[i] = Perm[index];
		Perm[index] = tmp;
	}
}

void Matrix::Choose(size_t index, size_t k)
	// enumerates all ways to choose E from the non X vector
{
	// base case -- if there are no more Es, call print function and return 
	if(k == 0){
		Print();
		return;
	}

	// second base case -- if there are more Es than non X places in the matrix, return
	if(k > (Non_X.size() - index)) return;

	// put the non X location in the E vector, recursively call choose(), then take the location off the E vector
	E_ID.push_back(Non_X[index]);
	Choose(index+1, k-1);
	E_ID.pop_back();

	// call choose() without putting non X location on the E vector
	Choose(index+1, k);
}
