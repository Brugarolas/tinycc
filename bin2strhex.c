#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: binary_to_hex input.bin output.hex\n");
        return 1;
    }

    const char *inputFileName = argv[1];
    const char *outputFileName = argv[2];

    FILE *inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    FILE *outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }

    unsigned char buffer[4];
    while (fread(buffer, sizeof(unsigned char), 4, inputFile) == 4) {
        unsigned int value = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
        fprintf(outputFile, "0x%08X\n", value);
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
