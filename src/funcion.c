#include "sample_lib.h"

// --- UTILIDADES ---

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
            matriz[i][j] = minimo(
                matriz[i - 1][j] + 1,
                matriz[i][j - 1] + 1,
                matriz[i - 1][j - 1] + coste
            );
        }
    }
    return matriz[len1][len2];
}

// Maneja abreviaturas comunes (C. -> Carrer)
void normalizar_nombre(char *dest, const char *src) {
    if (strncasecmp(src, "C. ", 3) == 0 || strncasecmp(src, "C/ ", 3) == 0) {
        strcpy(dest, "Carrer ");
        strcat(dest, src + 3);
    } else {
        strcpy(dest, src);
    }

}

void leer_cadena_segura(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; //quitamos el salto de línea
        } else {
            // si el usuario ha metido muchos caracteres, se eliminan los que sobran 
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
}

// --- GESTIÓN DE CASAS (ADDRESS) ---

House* cargar_mapa(char *path, int *total) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        printf("No se pudo abrir el archivo en la ruta: %s\n", path);
        return NULL;
    }

    House *cabeza = NULL;
    char linea[256];
    *total = 0;

    while (fgets(linea, sizeof(linea), f)) {
        House *nueva = (House*)malloc(sizeof(House));
        if (nueva == NULL) break;

        char raw_street[100];
        // Leemos primero el nombre crudo de la calle a una variable temporal
        if (sscanf(linea, "%[^,],%d,%lf,%lf", raw_street, &nueva->house_number, &nueva->latitud, &nueva->longitud) == 4) {
            
            // Normalizamos el nombre al cargarlo para evitar problemas con las abreviaturas
            normalizar_nombre(nueva->street_name, raw_street);
            
            nueva->next = cabeza;
            cabeza = nueva;
            (*total)++;
        } else {
            free(nueva); // Línea mal formateada o vacía, la ignoramos
        }
    }
    fclose(f);    
    return cabeza;
}

void buscar_direccion(House *lista) {
    char raw_name[100], street_search[100];
    int num_search;

    printf("Enter street name: ");
    // Sustituimos getchar() y fgets() por tu función segura
    leer_cadena_segura(raw_name, 100);
    normalizar_nombre(street_search, raw_name);

    printf("Enter street number: ");
    // Bucle para evitar que el programa crashee si meten letras
    while (scanf("%d", &num_search) != 1) {
        printf("Invalid input. Please enter a numeric value: ");
        int c; while ((c = getchar()) != '\n' && c != EOF); // Limpiar buffer
    }
    // Limpiamos el salto de línea sobrante
    int c; while ((c = getchar()) != '\n' && c != EOF);

    House *actual = lista;
    House *mejor_calle_nodo = NULL;
    int min_dist = 100;
    int calle_exacta_encontrada = 0;

    while (actual != NULL) {
        // Coincidencia de calle (sin importar mayúsculas)
        if (strcasecmp(actual->street_name, street_search) == 0) {
            calle_exacta_encontrada = 1;
            if (actual->house_number == num_search) {
                printf("\n    Found at (%lf, %lf)\n", actual->latitud, actual->longitud);
                return;
            }
        }
        int d = distancia_levenshtein(street_search, actual->street_name);
        if (d < min_dist) {
            min_dist = d;
            mejor_calle_nodo = actual;
        }
        actual = actual->next;
    }

    if (calle_exacta_encontrada) {
        printf("Invalid number. Valid numbers in %s: ", street_search);
        actual = lista;
        while (actual != NULL) {
            if (strcasecmp(actual->street_name, street_search) == 0) printf("%d ", actual->house_number);
            actual = actual->next;
        }
        printf("\n");
    } else if (mejor_calle_nodo != NULL) {
        printf("Street not found. Did you mean: %s?\n", mejor_calle_nodo->street_name);
    }
}

// --- GESTIÓN DE LUGARES (PLACE) ---

Place* cargar_lugares(char *path, int *total) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return NULL;

    Place *cabeza = NULL;
    char linea[300];
    *total = 0;

    while (fgets(linea, sizeof(linea), f)) {
        Place *nuevo = (Place*)malloc(sizeof(Place));
        if (nuevo == NULL) break;
        if (sscanf(linea, "%*[^,],%[^,],%*[^,],%lf,%lf", nuevo->name, &nuevo->latitud, &nuevo->longitud) == 3) {
            nuevo->next = cabeza;
            cabeza = nuevo;
            (*total)++;
        }
        else {
            free(nuevo);
        }
    }
    fclose(f);
    return cabeza;
}

void buscar_lugar(Place *lista) {
    char search_name[150];
    
    printf("Enter place name: ");
    // Usamos la función segura
    leer_cadena_segura(search_name, 150);

    Place *actual = lista;
    Place *mejor_sug = NULL;
    int min_dist = 100;

    // Array para almacenar posibles coincidencias múltiples
    Place *coincidencias[50]; 
    int num_coincidencias = 0;

    while (actual != NULL) {
        // Comparamos sin importar mayúsculas
        if (strcasecmp(actual->name, search_name) == 0) {
            if (num_coincidencias < 50) {
                coincidencias[num_coincidencias++] = actual;
            }
        } else {
            int d = distancia_levenshtein(search_name, actual->name);
            if (d < min_dist) {
                min_dist = d;
                mejor_sug = actual;
            }
        }
        actual = actual->next;
    }

    // Lógica de desempate
    if (num_coincidencias == 1) {
        printf("\n    Found at (%lf, %lf)\n", coincidencias[0]->latitud, coincidencias[0]->longitud);
        return;
    } else if (num_coincidencias > 1) {
        printf("\nMultiple places found. Please choose one:\n");
        for (int i = 0; i < num_coincidencias; i++) {
            printf("  %d) %s at (%lf, %lf)\n", i + 1, coincidencias[i]->name, coincidencias[i]->latitud, coincidencias[i]->longitud);
        }
        
        int choice;
        printf("Enter your choice (1-%d): ", num_coincidencias);
        while (scanf("%d", &choice) != 1 || choice < 1 || choice > num_coincidencias) {
            printf("Invalid choice. Try again: ");
            int c; while ((c = getchar()) != '\n' && c != EOF); // Limpiar basura
        }
        int c; while ((c = getchar()) != '\n' && c != EOF); // Limpiar buffer tras éxito
        
        printf("\n    Found at (%lf, %lf)\n", coincidencias[choice-1]->latitud, coincidencias[choice-1]->longitud);
        return;
    }

    if (mejor_sug != NULL && min_dist < 10) {
        printf("Place not found. Did you mean: %s?\n", mejor_sug->name);
    } else {
        printf("Place not found.\n");
    }
}

// --- LIBERACIÓN DE MEMORIA ---

void liberar_lista(House *lista) {
    while (lista) {
        House *temp = lista;
        lista = lista->next;
        free(temp);
    }
}

void liberar_lugares(Place *lista) {
    while (lista) {
        Place *temp = lista;
        lista = lista->next;
        free(temp);
    }
}