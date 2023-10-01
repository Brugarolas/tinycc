#include <stdio.h>
#include <stdint.h>

// Define a custom integer type for precision (adjust as needed)
typedef int64_t integer;

void calculate_pi(integer *a, integer *b, integer *t, integer *p) {
    // Constants for initial values
    const integer two = 2;
    const integer three = 3;
    const integer one = 1;
    const integer precision = 10000;  // Adjust the precision as needed

    *a = precision;
    *b = precision / two;
    *t = precision / 4;
    *p = one;

    for (int i = 0; i < precision; i++) {
        integer a_next = (*a + *b) / two;
        integer b_next = (*a * *b) / precision;
        integer t_next = *t - (*p * (*a - a_next) * (*a - a_next));

        *a = a_next;
        *b = b_next;
        *t = t_next;
        *p = *p * two;

        // Print the current approximation (optional)
        printf("%lld.", (*a * 4) / *t);

        *a *= precision;
        *b *= precision;
        *t *= precision;
    }
}

int main() {
    integer a, b, t, p;
    calculate_pi(&a, &b, &t, &p);

    printf("\nApproximation of pi: %lld\n", (a * 4) / t);

    return 0;
}

