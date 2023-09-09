#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct bits {
    unsigned b1: 1;
    unsigned b2: 1;
    unsigned b3: 1;
    unsigned b4: 1;
    unsigned b5: 1;
    unsigned b6: 1;
    unsigned b7: 1;
    unsigned b8: 1;
};

union bit_set {
    unsigned char byte;
    struct bits byte_bits;
};

int decode(const char *filename, char *txt, int size) {
    if(filename == NULL || txt == NULL || size <= 0) return 1;

    FILE *file = fopen(filename, "r");
    if(file == NULL) return 2;

    int amount_of_bits = 0, amount_of_bytes = 0, amount_of_nums = 0;
    long int file_size;
    union bit_set pass_char, file_char;

    while(!feof(file)) {
        fscanf(file, "%s", &file_char.byte);
        amount_of_nums++;
    }

    if(amount_of_nums == (size + 1)) {
        fclose(file);
        return 3;
    }

    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    while (!feof(file)) {
        if (fscanf(file, "%hhu", &file_char.byte) != 1) {
            if(ftell(file) < file_size) {
                fclose(file);
                return 3;
            }
            else {
                fclose(file);
                return 0;
            }
        }

        if (amount_of_bits == 0) pass_char.byte_bits.b8 = file_char.byte_bits.b1;
        else if (amount_of_bits == 1) pass_char.byte_bits.b7 = file_char.byte_bits.b1;
        else if (amount_of_bits == 2) pass_char.byte_bits.b6 = file_char.byte_bits.b1;
        else if (amount_of_bits == 3) pass_char.byte_bits.b5 = file_char.byte_bits.b1;
        else if (amount_of_bits == 4) pass_char.byte_bits.b4 = file_char.byte_bits.b1;
        else if (amount_of_bits == 5) pass_char.byte_bits.b3 = file_char.byte_bits.b1;
        else if (amount_of_bits == 6) pass_char.byte_bits.b2 = file_char.byte_bits.b1;
        else if (amount_of_bits == 7) {
            pass_char.byte_bits.b1 = file_char.byte_bits.b1;
            *(txt + amount_of_bytes) = pass_char.byte;
            amount_of_bits = -1;
            amount_of_bytes++;

            if(amount_of_bytes == size - 1) {
                *(txt + amount_of_bytes) = '\0';
                fclose(file);
                return 0;
            }
        }

        amount_of_bits++;
    }

    fclose(file);
    return 0;
}

int encode(const char *input, char *txt, const char *output) {
    if(input == NULL || txt == NULL || output == NULL) return 1;

    FILE *fin, *fout;
    fin = fopen(input, "r");
    if(fin == NULL) return 2;
    fout = fopen(output, "w");
    if(fout == NULL) {
        fclose(fin);
        return 4;
    }

    int amount_of_bits = 0, nums_in_line = 0, used_nums = 0, str_size = strlen(txt), used_chars = 0;
    size_t amount_of_nums = 0;
    union bit_set pass_char, file_char;
    char c, *ptr;

    while(!feof(fin)) {
        fscanf(fin, "%s", &file_char.byte);
        amount_of_nums++;
    }
    amount_of_nums--;

    if(amount_of_nums < strlen(txt)) {
        fclose(fin);
        fclose(fout);
        return 3;
    }

    fseek(fin, 0, SEEK_SET);

    while(!feof(fin)) {
        fscanf(fin, "%c", &c);
        if(isalpha(c)) {
            fclose(fin);
            fclose(fout);
            return 3;
        }
    }

    fseek(fin, 0, SEEK_SET);

    c = 0;

    while(c != '\n') {
        fscanf(fin, "%c", &c);
        if(c == ' ') nums_in_line++;
    }

    fseek(fin, 0, SEEK_SET);

    ptr = txt;

    while(txt != NULL && !feof(fin)) {
        if (fscanf(fin, "%hhu", &file_char.byte) != 1) {
            fclose(fin);
            fclose(fout);
            return 0;
        }

        if (amount_of_bits == 0) {
            if(used_chars != str_size) {
                pass_char.byte = *ptr;
                file_char.byte_bits.b1 = pass_char.byte_bits.b8;
            }
            else file_char.byte_bits.b1 = 0;
        }
        else if (amount_of_bits == 1) {
            if(used_chars != str_size) file_char.byte_bits.b1 = pass_char.byte_bits.b7;
            else file_char.byte_bits.b1 = 0;
        }
        else if (amount_of_bits == 2) {
            if(used_chars != str_size) file_char.byte_bits.b1 = pass_char.byte_bits.b6;
            else file_char.byte_bits.b1 = 0;
        }
        else if (amount_of_bits == 3) {
            if(used_chars != str_size) file_char.byte_bits.b1 = pass_char.byte_bits.b5;
            else file_char.byte_bits.b1 = 0;
        }
        else if (amount_of_bits == 4) {
            if(used_chars != str_size) file_char.byte_bits.b1 = pass_char.byte_bits.b4;
            else file_char.byte_bits.b1 = 0;
        }
        else if (amount_of_bits == 5) {
            if(used_chars != str_size) file_char.byte_bits.b1 = pass_char.byte_bits.b3;
            else file_char.byte_bits.b1 = 0;
        }
        else if (amount_of_bits == 6) {
            if(used_chars != str_size) file_char.byte_bits.b1 = pass_char.byte_bits.b2;
            else file_char.byte_bits.b1 = 0;
        }
        else if (amount_of_bits == 7) {
            if(used_chars != str_size) {
                used_chars++;
                file_char.byte_bits.b1 = pass_char.byte_bits.b1;
            }
            else file_char.byte_bits.b1 = 0;
            amount_of_bits = -1;
            ptr++;
        }

        if(used_nums % nums_in_line == 0 && used_nums != 0) {
            fprintf(fout, "%c", '\n');
        }
        if(used_nums % nums_in_line != 0) fprintf(fout, " %hhu", file_char.byte);
        else fprintf(fout, "%hhu", file_char.byte);
        used_nums++;
        amount_of_bits++;
    }

    fclose(fin);
    fclose(fout);
    return 0;
}

int main() {
    char password[1001], decision, message[1001], input_filename[30], output_filename[30];
    int res, c;
    printf("Do you want to encode (E/e) or decode (D/d) a message?:");
    scanf("%c", &decision);
    getchar();
    if(decision == 'e' || decision == 'E') {
        printf("Enter a message:");
        scanf("%1000[^\n]", message);
        while((c = getchar()) && c != '\n' && c != EOF) {}
        printf("Enter input file name:");
        scanf("%29[^\n]", input_filename);
        while((c = getchar()) && c != '\n' && c != EOF) {}
        printf("Enter output file name:");
        scanf("%29[^\n]", output_filename);
        while((c = getchar()) && c != '\n' && c != EOF) {}
        res = encode(input_filename, message, output_filename);
        if(res == 2) {
            printf("Couldn't open file");
            return 4;
        }
        else if(res == 3) {
            printf("File corrupted");
            return 6;
        }
        else if(res == 4) {
            printf("Couldn't create file");
            return 5;
        }
        else if(res == 0) {
            printf("File saved");
            return 0;
        }
    }
    else if(decision == 'd' || decision == 'D') {
        printf("Enter input file name:");
        scanf("%29[^\n]", input_filename);
        while((c = getchar()) && c != '\n' && c != EOF) {}
        res = decode(input_filename, password, 1001);
        if(res == 2) {
            printf("Couldn't open file");
            return 4;
        }
        else if(res == 3) {
            printf("File corrupted");
            return 6;
        }
        else if(res == 0) {
            for(int i = 0; i < 1000 && *(password + i) != '\0'; i++) {
                if(*(password + i) < 32 || isdigit(*(password + i))) {
                    printf("File corrupted");
                    return 6;
                }
            }
            printf("%s", password);
        }
    }
    else {
        printf("Incorrect input data");
        return 1;
    }
    return 0;
}
