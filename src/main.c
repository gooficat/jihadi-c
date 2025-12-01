#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

unsigned char typedef byte;
unsigned short typedef word;
unsigned int typedef dword;
unsigned long long typedef qword;


struct {
    byte *data;
    unsigned char len;
} typedef bytearr_s;

byte typedef reg_t;

union {
    dword dw;
    byte b[2];
} typedef dw_b_u;

bytearr_s mov_r_dw(reg_t reg, dword imm) {
    bytearr_s out;
    out.data = malloc((out.len = 3));
    out.data[0] = 0x88 + reg;
    dw_b_u u = {
        imm
    };

    out.data[1] = u.b[0];
    out.data[2] = u.b[1];

    return out;
}

void fetch_token(FILE * f, char * b) {
    int c = fgetc(f);

    while (isspace(c) && c != EOF) c = fgetc(f);
    if (c == EOF) {
        b[0] = 0;
        return;
    }
    memset(b, 0, 32);

    unsigned char bl = 0;

    if (isalnum(c)) {
        do {
            b[bl++] = c;
            c = fgetc(f);
        } while (isalnum(c));
        ungetc(c, f);
    }
    else
        b[bl++] = c;
    b[bl] = '\0';
}

struct {
    char * name;
    long long offset;
    unsigned char size;
    bool is_static;
} typedef identifier_s;

struct {
    identifier_s *identifiers;
    long long unsigned size;
} identifiers;

struct reg {
    const char * qw, * dw;
    const char * w, * h, * l;
    reg_t code;
    bool reserved;
} registers[4] = {
    {
        "rax", "eax", "ax",
        "ah", "al",
        0x00,
        false
    },
    {
        "rcx", "ecx", "cx",
        "ch", "cl",
        0x01,
        false
    },
    {
        "rdx", "edx", "dx",
        "dh", "dl",
        0x02,
        false
    },
    {
        "rbx", "ebx", "bx",
        "bh", "bl",
        0x03,
        false
    },
};

static char * types[] = {
    "u8",
    "u16",
    "u32",
    "u64"
};

int main(int argc, char * argv[]) {
    assert(argc == 2 && "Wrong args");
    FILE * f; fopen_s(&f, argv[1], "rt");

    char tk[32];
    fetch_token(f, tk);

    identifiers.size = 0;

    while (tk[0]) {
        // printf_s("%s\n", tk.data);

        int vl = -1;
        for (int i = 0; i != 4; ++i) {
            if (!strcmp(types[i], tk)) {
                vl = pow(2, i); // is a definition
                break;
            }
        }

        
        if (vl == -1) { // is a definition
            for (int i = 0; i != identifiers.size; ++i) {
                if (!strcmp(identifiers.identifiers[i].name, tk)) {
                    printf_s("assignment to %s\n", tk);
                    fetch_token(f, tk);

                    if (tk[0] == '=') {
                        char ** statement = NULL;

                        int j = 0;
                        while (1) {
                            fetch_token(f, tk);

                            if (tk[0] == ';')
                                break;

                            statement = realloc(statement, sizeof(char*) * (j+1));
                            
                            int tl = strlen(tk)+1;
                            statement[j] = malloc(tl);

                            strcpy_s(statement[j++], tl, tk);
                        }

                        register int k = 0;
                        while (k != j) {
                            printf_s("%s\n", statement[k]);
                            free(statement[k++]);
                        }
                        if (statement != NULL) free(statement);
                    }

                    break;
                }
            }
        }
        else { // is a definition
            printf_s("%s %d\n", tk, vl);
            unsigned long long id_id = identifiers.size++;
            identifiers.identifiers = realloc(identifiers.identifiers, sizeof(identifier_s) * identifiers.size);

            fetch_token(f, tk);
            printf_s("|%s|\n", tk);

            identifiers.identifiers[id_id].name = malloc(strlen(tk)+1);
            strcpy_s(identifiers.identifiers[id_id].name, strlen(tk)+1, tk);
            identifiers.identifiers[id_id].size = vl;
        }
        
        fetch_token(f, tk);
    }

    bytearr_s test = mov_r_dw(registers[1].code, 0xFFAA);

    for (int i = 0; i != test.len; ++i) {
        printf("%i: %hhx, %hhu\n", i, test.data[i], test.data[i]);
    }

    free(test.data);

    return 0;
}