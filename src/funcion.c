#include "sample_lib.h"

// utilidades

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

// normaliza el nombre (cambia 'c.' y 'c/' por 'carrer')
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

// funciones para gestionar casas

House* cargar_mapa(char *path, int *total) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        printf("No se ha podido abrir el archivo: %s\n", path);
        return NULL;
    }

    House *cabeza = NULL;
    char linea[256];
    *total = 0;

    while (fgets(linea, sizeof(linea), f)) {
        House *nueva = (House*)malloc(sizeof(House));
        if (nueva == NULL) break;

        char nombre_calle[100]; // variable para guardar el nombre de la calle.
        if (sscanf(linea, "%[^,],%d,%lf,%lf", nombre_calle, &nueva->house_number, &nueva->latitud, &nueva->longitud) == 4) {

            // si en streets.txt está escrito como 'c.' o 'c/', lo substituimos por 'carrer'
            normalizar_nombre(nueva->street_name, nombre_calle);

            nueva->next = cabeza;
            cabeza = nueva;
            (*total)++;
        } else {
            free(nueva); // si la línea no es correcta (o vacía), se ignora
        }
    }
    fclose(f);    
    return cabeza;
}

void buscar_direccion(House *lista) {
    char raw_name[100], street_search[100];
    int num_search;

    printf("Enter street name: ");
    getchar(); //limpiar buffer de entrada
    leer_cadena_segura(raw_name, 100); // leemos el nombre de la calle
    normalizar_nombre(street_search, raw_name);

    printf("Enter street number: ");
    scanf("%d", &num_search);

    House *actual = lista;
    House *mejor_calle_nodo = NULL;
    int min_dist = 100;
    int calle_exacta_encontrada = 0;

    while (actual != NULL) {
        if (strcasecmp(actual->street_name, street_search) == 0) { // comprueba si encuentra la calle
            calle_exacta_encontrada = 1;
            if (actual->house_number == num_search) { //comprueba si existe el número en esa calle
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

    if (calle_exacta_encontrada) { // si la calle existe pero el número no, se le vuelve a pedir el número al usuario
        printf("Invalid number. Valid numbers in %s: ", street_search);
        actual = lista;
        while (actual != NULL) {
            if (strcasecmp(actual->street_name, street_search) == 0) {
                printf("%d ", actual->house_number);
            }
            actual = actual->next;
        }
        printf("\n");
        
        // aqui lista los números válidos
        printf("Enter one of the valid numbers: ");
        scanf("%d", &num_search);
        
        //si el usuario escribe un número incorrecto, vuelve a preguntar
        actual = lista;
        while (actual != NULL) {
            if (strcasecmp(actual->street_name, street_search) == 0 && actual->house_number == num_search) {
                printf("\n    Found at (%lf, %lf)\n", actual->latitud, actual->longitud);
                return;
            }
            actual = actual->next;
        }
        printf("Still an invalid number.\n"); //si el número sigue siendo incorrecto, muestra el mensaje
    } else if (mejor_calle_nodo != NULL) {
        printf("Street not found. Did you mean: %s?\n", mejor_calle_nodo->street_name);
    }
}

// funciones para gestionar lugares

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
    leer_cadena_segura(search_name, 100);
    
    Place *actual = lista;
    Place *mejor_sug = NULL;
    int min_dist = 100;

    while (actual != NULL) {
        // Comparamos sin importar mayúsculas
        if (strcasecmp(actual->name, search_name) == 0) {
            printf("\n    Found at (%lf, %lf)\n", actual->latitud, actual->longitud);
            return;
        }
        
        int d = distancia_levenshtein(search_name, actual->name);
        if (d < min_dist) {
            min_dist = d;
            mejor_sug = actual;
        }
        actual = actual->next;
    }

    if (mejor_sug != NULL && min_dist < 10) {
        printf("Place not found. Did you mean: %s?\n", mejor_sug->name);
    } else {
        printf("Place not found.\n");
    }
}

//funciones para liberar la memoria

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