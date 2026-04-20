#include "sample_lib.h"

int minimo(int a, int b, int c) {
    int m = a;
    if (b < m) m = b;
    if (c < m) m = c;
    return m;
}

int distancia_levenshtein(char *s1, char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matriz[len1 + 1][len2 + 1];
    for (int i = 0; i <= len1; i++) matriz[i][0] = i;
    for (int j = 0; j <= len2; j++) matriz[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int coste = (tolower(s1[i - 1]) == tolower(s2[j - 1])) ? 0 : 1;
            matriz[i][j] = minimo(matriz[i - 1][j] + 1, matriz[i][j - 1] + 1, matriz[i - 1][j - 1] + coste);
        }
    }
    return matriz[len1][len2];
}

// Reemplaza "C." o "C/" por "Carrer" para mejorar la búsqueda
void normalizar_nombre(char *dest, const char *src) {
    if (strncmp(src, "C. ", 3) == 0 || strncmp(src, "c. ", 3) == 0 || strncmp(src, "C/ ", 3) == 0) {
        strcpy(dest, "Carrer ");
        strcat(dest, src + 3);
    } else {
        strcpy(dest, src);
    }
}

House* cargar_mapa(char *path, int *total) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return NULL;
    House *cabeza = NULL;
    char linea[256];
    *total = 0;
    while (fgets(linea, sizeof(linea), f)) {
        House *nueva = (House*)malloc(sizeof(House));
        if (sscanf(linea, "%[^;];%d;%lf;%lf", nueva->street_name, &nueva->house_number, &nueva->latitud, &nueva->longitud) == 4) {
            nueva->next = cabeza;
            cabeza = nueva;
            (*total)++;
        } else { free(nueva); }
    }
    fclose(f);
    return cabeza;
}

void buscar_direccion(House *lista) {
    char raw_name[100], street_search[100];
    int num_search;

    printf("Enter street name: ");
    getchar(); // Limpiar buffer del scanf anterior
    fgets(raw_name, 100, stdin);
    raw_name[strcspn(raw_name, "\n")] = 0;
    normalizar_nombre(street_search, raw_name);

    printf("Enter street number: ");
    scanf("%d", &num_search);

    House *actual = lista;
    House *mejor_calle_nodo = NULL;
    int min_dist = 100;
    int calle_exacta_encontrada = 0;

    // 1. Buscar coincidencia de calle (insensible a mayúsculas)
    while (actual != NULL) {
        if (strcasecmp(actual->street_name, street_search) == 0) {
            calle_exacta_encontrada = 1;
            if (actual->house_number == num_search) {
                printf("\n    Found at (%lf, %lf)\n", actual->latitud, actual->longitud);
                return;
            }
        }
        // Guardar la más parecida por si falla
        int d = distancia_levenshtein(street_search, actual->street_name);
        if (d < min_dist) {
            min_dist = d;
            mejor_calle_nodo = actual;
        }
        actual = actual->next;
    }

    // 2. Si la calle existe pero el número no
    if (calle_exacta_encontrada) {
        printf("Invalid number. Valid numbers in %s: ", street_search);
        actual = lista;
        while (actual != NULL) {
            if (strcasecmp(actual->street_name, street_search) == 0) {
                printf("%d ", actual->house_number);
            }
            actual = actual->next;
        }
        printf("\n");
    } 
    // 3. Si la calle no existe, sugerir la más parecida
    else if (mejor_calle_nodo != NULL) {
        printf("Street not found. Did you mean: %s?\n", mejor_calle_nodo->street_name);
    }
}

void liberar_lista(House *lista) {
    while (lista) {
        House *temp = lista;
        lista = lista->next;
        free(temp);
    }
}