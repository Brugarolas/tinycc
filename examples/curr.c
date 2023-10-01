#define DEBUG 0

#ifndef count_of
#define count_of(x)                                                            \
  ((sizeof(x) / sizeof(0[x])))
#endif

#if !defined(DEBUG) || DEBUG == 0
#define __poxim__
/*
  If we're not using clang not gcc then we define poxim 
  this is for us to run both on tcc-poxim and gcc and not change the code
*/
#if !defined(__clang__) &&  !defined(__GNUC__) && !defined(__GNUG__)
  // other checks __clang_major__, __clang_minor__ and __clang_patchlevel__
  #define __poxim__
#endif

#if defined(__poxim__)
#include "_start.h"
int *terminal32 = (int *)(0x88888888 >> 2);

unsigned int strlen(const char *str);
void putchar(int c) { *terminal32 = c; }
void puti(int num);
void puts(const char *str);
#else 
#include <stdio.h>
#endif


//>>********************** Printing ********************* //
/* @Attention, to garantee puts will work the str has to be 4 byte aligned :)
  That means every string will in the whole program have to be padded with zero
  to ensure every string is a multiple of 4 hehe, remember that last byte of str
  literal is always zero
*/
#if defined(__poxim__)
void puts(const char *str) {
  int *ptr = (int *)str;
  int word;

  int len = 0;
  while (1) {
    word = *ptr++;

    len += 4;
    // Verifica se algum byte no int é zero (terminador nulo).
    int
      b1 = ((word >> 24) & 0xFF),
      b2 = ((word >> 16) & 0xFF),
      b3 = ((word >> 8 ) & 0xFF),
      b4 = ((word >> 0 ) & 0xFF)
    ;
    if ( b1 == 0 && len != 4) {
      return;
    }
    if (b1) {
      putchar((word >> 24) & 0xFF);
    }

    if (b2 == 0 && len != 4) {
      return;
    }
    if (b2) {
      putchar((word >> 16) & 0xFF);
    }

    if (b3 == 0 && len != 4) {
      return;
    }
    if (b3) {
      putchar((word >> 8) & 0xFF);
    }
    if (b4 == 0 && len != 4) {
      return;
    }
    if (b4) {
      putchar((word >> 0) & 0xFF);
    }
  }
}

unsigned int strlen(const char *str) {
  int *ptr = (int *)str;
  int word;

  int len = 0;
  while (1) {
    word = *ptr++;

    len += 4;
    // Verifica se algum byte no int é zero (terminador nulo).
    if (((word >> 24) & 0xFF) == 0) {
      return len - 1;
    }
    if (((word >> 16) & 0xFF) == 0) {
      return len - 2;
    }
    if (((word >> 8) & 0xFF) == 0) {
      return len - 3;
    }
    if (((word >> 0) & 0xFF) == 0) {
      return len - 4;
    }
  }
}



void puti(int num) {
  int divisor = 1;
  int is_negative = 0;

  // Handle negative numbers
  if (num < 0) {
    is_negative = 1;
    num = -num;
  }

  // Find the divisor for the largest digit place
  while (num / divisor > 9) {
    divisor *= 10;
  }

  // If the number was negative, print the minus sign
  if (is_negative > 0) {
    putchar('-');
  } else {
    // putchar('+');
  }
  // Extract and print each digit
  while (divisor > 0) {
    int digit = (num / divisor) % 10;
    putchar('0' + digit);
    divisor /= 10;
  }
}

#else
#define puti(x) printf("%d", x)
#define puts(input) fputs(input, stdout); // no new line just line mine
#endif
//<<********************** Printing ********************* //

//>>********************** Struct ********************* //

typedef struct {
  int x, y;
} vector2i;

int dot(vector2i v1, vector2i v2) { 
  int result = v1.x * v2.x + v1.y * v2.y; 
  return result;
}

int structure_play(void) {
  vector2i v1 = {.x=-7, .y=2}, v2 = {.x=3, .y=-4};

  puts("\nstructs: \n");

  puts("\t   v1.x = ");
  puti(v1.x);
  putchar(' ');

  puts("v1.y = ");
  puti(v1.y);
  putchar(' ');

  putchar('\n');

  puts("\t   v2.x = ");
  puti(v2.x);
  putchar(' ');

  puts("v2.y = ");
  puti(v2.y);
  putchar(' ');

  putchar('\n');

  puts("\ndot:\n\t\tv1*v2 =  ");
  puti(dot(v1, v2));
  return 1;
}
//<<********************** Struct ********************* //

//>>********************** Sorting ********************* //

void bubble_sort(int arr[], int n) {
  int temp;
  int swapped;

  for (int i = 0; i < n - 1; i++) {
    swapped = 0;

    for (int j = 0; j < n - 1 - i; j++) {
      // Compare adjacent elements
      if (arr[j] > arr[j + 1]) {
        // Swap them if they are in the wrong order
        temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = 1; // Set the swapped flag
      }
    }

    // If no two elements were swapped in this pass, the array is sorted
    if (swapped == 0) {
      break;
    }
  }
}

void sort_play(void) {
  int arr[] = {-1, -564, 420, 69, 1200, 300};
  int n = sizeof(arr) / sizeof(arr[0]);
  int i, a;
  int l = 2;
  puts("before sort:\n\t");
  for (i = 0; i < n; i++) {
    a = arr[i];
    puti(a);
    putchar(' ');
  }

  putchar('\n');
  bubble_sort(arr, n);

  puts(" after sort:\n\t");
  for (i = 0; i < n; i++) {
    a = arr[i];
    puti(a);
    putchar(' ');
  }
}
//<<********************** Sorting ********************* //


//>>********************** Recursion ********************* //
#define MAX_FIB 8
int fibonnacci(int n) {
  if (n < 2) {
    return n;
  }
  return fibonnacci(n - 1) +  fibonnacci(n - 2);
}

void recursion_play(void) {

  for (int i = 0; i < MAX_FIB; i++) {
    puts("  fibonnaci ");
    putchar('(');
    puti(i);
    putchar(')');
    putchar(' ');
    putchar('=');
    putchar(' ');
    puti(fibonnacci(i));
    putchar('\n');
  }
}
//<<********************** Recursion ********************* //

//>>********************** Machine Code Execution ********************* //

// Machine instructions
unsigned int code[] =  {
  0x01E001A4,  // mov r15, 420 
  0x75E70000,  // s32 [r7-0], r15  /* r7-0 is a pointer to the fist element on the current function frame */
  0x7C000000,  // ret
};

void machine_code_play(void) {
  int num = 2; // &a is r7-0
  puts("value before runtime machine code execution");
  puts(" \nnum = "); puti(num); 

  void (*jit)(void);
  jit = (void(*)(void))(code);
  jit();

  puts("\nvalue of 'num' after jit ");
  puts(" \nnum = "); puti(num); 
   
}

//<<********************** Machine Code Execution ********************* //


//>>********************** Polymorphism ********************* //
typedef struct {
  void (*speak)(void);
} Animal;

typedef struct{
  Animal base;
  void (*speak)(void);
  int age;
} Cat;

typedef struct {
  Animal base;
} Dog;


void dog_speak(void) {
  puts(
"hello I'm doggo         \n"
"              /)-_-(\\  \n"
"               (o o)    \n"
"       .-----__/\\o/    \n"
"      /  __      /      \n"
"  \\__/\\ /  \\_\\ |/   \n"
"       \\\\     ||      \n"
"       //     ||        \n"
"       |\\     |\\      \n"
  );
}

void cat_speak(void) {
  puts(
    "i'm little kitty       \n"
    "   ,_      _           \n"
    "   |\\\\_,-~/          \n"
    "   / _  _ |    ,--.    \n"
    "  (  @  @ )   / ,-'    \n"
    "   \\  _T_/-._( (      \n"
    "   /         `. \\     \n"
    "  |         _  \\ |    \n"
    "   \\ \\ ,  /      |   \n"
    "    || |-_\\__   /     \n"
    "   ((_/`(____,-'       \n"
  );
}

void create_cat(Cat* c) {
  c->base.speak = cat_speak;
}

void create_dog(Dog* d) {
  d->base.speak = dog_speak;
}

void animal_generic_speak(Animal* a){
  a->speak();
}

void polymorphism_play(){
    Dog d; create_dog(&d);
    Cat c; create_cat(&c);
    animal_generic_speak((Animal*)&c);
    putchar('\n');
    animal_generic_speak((Animal*)&d);

}
//<<********************** Polymorphism ********************* //

//>>********************** Matrices ********************* //
#define ROW1 4
#define COL1 3
#define ROW2 3
#define COL2 4
void multiply_matrices(int *mat1, int row1, int col1, int *mat2, int row2, int col2, int *result) {
    int i, j, k;

    for (i = 0; i < row1; i++) {
        for (j = 0; j < col2; j++) {
            result[i * col2 + j] = 0;
            for (k = 0; k < col1; k++) {
                result[i * col2 + j] += mat1[i * col1 + k] * mat2[k * col2 + j];
            }
        }
    }
}

void print_matrix(int *mat, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            putchar(' ');
            puti(mat[i * cols + j]);
            putchar(' ');
        }
        putchar('\n');
    }
}

int matrices_play() {
    int matrix1[ROW1][COL1] = {{1, 2, 3},
                              {4, 5, 6},
                              {7, 8, 9},
                              {10, 11, 12}};

    int matrix2[ROW2][COL2] = {{1, 2, 3, 4},
                              {5, 6, 7, 8},
                              {9, 10, 11, 12}};

    int result[ROW1][COL2];
    

    if (COL1 != ROW2) {
        puts("Matrix multiplication not possible. Inner dimensions must match.\n");
        return 1;
    }

    puts("\n\tMatrix A\n");
    print_matrix((int*) matrix1, ROW1, COL1);

    puts("\n\tMatrix B\n");
    print_matrix((int*) matrix2, ROW2, COL2);


    multiply_matrices((int*)matrix1, ROW1, COL1, (int*)matrix2, ROW2, COL2, (int*)result);
    puts("\n\tA*B = Matrix C\n");
    print_matrix((int*) result , ROW1, COL2);

    return 0;
}
//<<********************** Matrices ********************* //

//>>********************** Memory play  ********************* //
int global_arr1[] = {991, 992, 993}; 
int global_arr2[] = {881, 882, 883}; 
int global_arr3[count_of(global_arr1)];

void print_arr(int* arr, int n) {
  putchar('[');
  for (size_t i = 0; i < n; i++) {
    puti(i[arr]);
    if(i < n-1) {
      putchar(',');
      putchar(' ');
    }
  } 
  putchar(']');
}

void mem_play(void) {
  // Change the global_arr to point to global_arr2
  int local_arr4[count_of(global_arr1)];

  puts("global array 1: \n");
  print_arr(global_arr1, count_of(global_arr1));

  puts("\nglobal array 2: \n");
  print_arr(global_arr2, count_of(global_arr2));

  puts("\nmemmove(dst=global_arr1, src=global_arr2, sizeof(global_arr1))\n");
  memmove(global_arr1, global_arr2, sizeof(global_arr1));
  puts("global array 1 after memmove : \n");
  print_arr(global_arr1, count_of(global_arr1));

  puts("\nglobal array 3 is global and therefore zero initialized: \n");
  print_arr(global_arr3, count_of(global_arr3));

  puts("\nlocal array 4 is local not initialized and therefore its values are garbage: \n");
  print_arr(local_arr4, count_of(local_arr4));

  puts("\nmemset(dst=local_arr4, value=22, sizeof(local_arr4))\n");
  memset(local_arr4, 22, sizeof(local_arr4));
  print_arr(local_arr4, count_of(local_arr4));

}
//<<********************** Memory play ********************* //


#define MARK_CONST 40
#define mark(n)  do{ int i = n; putchar('\n'); while(i--) putchar('-'); putchar('\n');} while(0);

#define example(x)  mark(MARK_CONST); puts( " \t    \t>> " #x); putchar('\n'); x(); \
  // do{ int i = 2; ; while(i--) puts("    "); } while(0);
int main(void) {

  example(recursion_play);
  
  example(sort_play);

  example(structure_play);

  example(matrices_play);

  example(mem_play);

  example(polymorphism_play);

  example(machine_code_play);
  return 69;
}

#else
#include "_start.h"

int *terminal32 = (int *)(0x88888888 >> 2);
void putchar(int c) { *terminal32 = c; }

int a = 'a';
int main(void) {
  a = 'b';
  putchar(a);
}


#endif
