//////////////////////////////////////////////////////////////
// Make a program that swaps the values between two arrays. //
//////////////////////////////////////////////////////////////

#include <iostream>
using namespace std;


void swap(int &a, int &b)
{
    int tmp_a = a;
    int tmp_b = b;
    b = tmp_a;
    a = tmp_b;
}

void print_arr(int* arr, int size_arr)
{
    for (int i = 0; i < size_arr; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main()
{
    // declare arrays
    int A[] = {0, 1, 2, 3, 4};
    int B[] = {5, 6, 7, 8, 9};

    int size_arr = sizeof(A) / sizeof(A[0]);
    
    // print inputs
    print_arr(A, size_arr);
    print_arr(B, size_arr);

    // swap the variables
    for (int i = 0; i < size_arr; i++) {
        swap(A[i], B[i]);
    }

    // print outputs
    print_arr(A, size_arr);
    print_arr(B, size_arr);
}