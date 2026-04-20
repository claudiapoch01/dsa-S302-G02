#include "sample_lib.h"

// Función auxiliar para Levenshtein
int minimo(int a, int b, int c) {
    int m = a;
    if (b < m) m = b;
    if (c < m) m = c;
    return m;
}

// Lógica de Levenshtein: mide qué tan diferentes son dos palabras
int distancia_levenshtein(char *s1, char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matriz[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) matriz[i][0] = i;
    for (int j = 0; j <= len2; j++) matriz[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int coste = (tolower(s1[i - 1]) == tolower(s2[j - 1])) ? 0 : 1;
            matriz[i][j] = minimo(
                matriz[i - 1][j] + 1,
                matriz[i][j - 1] + 1,
                matriz[i - 1][j - 1] + coste
            );
        }
    }
    return matriz[len1][len2];
}

// Función para leer el archivo houses.txt y meterlo en la lista enlazada
House* cargar_mapa(char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return NULL;

    House *cabeza = NULL;
    char linea[256];

    while (fgets(linea, sizeof(linea), f)) {
        House *nueva = (House*)malloc(sizeof(House));
        // Formato: Nombre de calle;Número;Lat;Lon
        // Usamos sscanf para leer cada parte separada por ';'
        if (sscanf(linea, "%[^;];%d;%lf;%lf", nueva->street_name, &nueva->house_number, &nueva->latitud, &nueva->longitud) == 4) {
            nueva->next = cabeza;
            cabeza = nueva;
        } else {
            free(nueva); // Si la línea está mal, liberamos memoria
        }
    }
    fclose(f);
    return cabeza;
}

// Busca la calle más parecida si no hay coincidencia exacta
void buscar_con_sugerencias(char *busqueda, House *lista) {
    House *actual = lista;
    char mejor_calle[100] = "";
    int mejor_distancia = 100;

    while (actual != NULL) {
        int d = distancia_levenshtein(busqueda, actual->street_name);
        if (d < mejor_distancia) {
            mejor_distancia = d;
            strcpy(mejor_calle, actual->street_name);
        }
        actual = actual->next;
    }

    if (mejor_distancia > 0 && mejor_distancia < 5) {
        printf("No encontrada. ¿Quisiste decir: %s?\n", mejor_calle);
    } else {
        printf("Calle no encontrada y no hay sugerencias claras.\n");
    }
}