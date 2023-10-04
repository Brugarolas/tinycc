int *terminal32 = (int *)(0x88888888 >> 2);

unsigned int strlen(const char *str);
void putchar(int c) { *terminal32 = c; }
void puti(int num);
void puts(const char *str);
void print_arr(int *arr, int n);
void print_matrix(int *mat, int rows, int cols);

//>>********************** Printing ********************* //
/* @Attention, to garantee puts will work the str has to be 4 byte aligned :)
  That means every string will in the whole program have to be padded with zero
  to ensure every string is a multiple of 4 hehe, remember that last byte of str
  literal is always zero -> '\0'
*/
#if defined(PRINT_IMPLEMENTATION)
void puts(const char *str) {
  int *ptr = (int *)str;
  int word;

  int len = 0;
  while (1) {
    word = *ptr++;

    len += 4;
    // Verifica se algum byte no int é zero (terminador nulo).
    int b1 = ((word >> 24) & 0xFF), b2 = ((word >> 16) & 0xFF),
        b3 = ((word >> 8) & 0xFF), b4 = ((word >> 0) & 0xFF);
    if (b1 == 0 && len != 4) {
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

void print_arr(int *arr, int n) {
  putchar('[');
  for (unsigned int i = 0; i < n; i++) {
    puti(i[arr]);
    if (i < n - 1) {
      putchar(',');
      putchar(' ');
    }
  }
  putchar(']');
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

#endif //PRINT_IMPLEMENTATION
