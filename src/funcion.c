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

House* add_casa(House *cabeza, char *street, int num, double lat, double lon) {
    House *nueva = (House*)malloc(sizeof(House));
    if (nueva == NULL) return cabeza;
    strcpy(nueva->street_name, street);
    nueva->house_number = num;
    nueva->latitud = lat;
    nueva->longitud = lon;
    nueva->next = cabeza;
    return nueva;
}

Place* add_lugar(Place *cabeza, char *name, double lat, double lon) {
    Place *nuevo = (Place*)malloc(sizeof(Place));
    if (nuevo == NULL) return cabeza;
    strcpy(nuevo->name, name);
    nuevo->latitud = lat;
    nuevo->longitud = lon;
    nuevo->next = cabeza;
    return nuevo;
}

// normaliza el nombre (cambia 'c.', 'c/', 'carrer de', etc. por 'carrer')
void normalizar_nombre(char *dest, const char *src) {
    // comprueba si coincide con algun formato o abreviatura (ignorando mayúsculas) y lo substituye, sino lo deja igual
    // se usa la funcion strncasecmp que es como strncmp pero ignora mayúsculas y minúsculas
    if (strncasecmp(src, "C. de ", 6) == 0 || strncasecmp(src, "C/ de ", 6) == 0) {
        strcpy(dest, "Carrer ");
        //Se concatena el resto del string a partir de la posición 6, que es donde empieza el nombre de la calle
        strcat(dest, src + 6);
    } else if (strncasecmp(src, "C. ", 3) == 0 || strncasecmp(src, "C/ ", 3) == 0) {
        strcpy(dest, "Carrer ");
        //Se concatena el resto del string a partir de la posición 3, que es donde empieza el nombre de la calle
        strcat(dest, src + 3);
    } else if (strncasecmp(src, "Carrer de ", 10) == 0) {
        strcpy(dest, "Carrer ");
        //Se concatena el resto del string a partir de la posición 10, que es donde empieza el nombre de la calle
        strcat(dest, src + 10);
    } else {
        strcpy(dest, src);
    }

    quitar_acentos(dest); // se eliminan los acentos
}

// Elimina los acentos 
void quitar_acentos(char *cadena) {
    // creamos arrays con las letras con acento y sin acento (para substituirlo luego)
    const char *con_acento[] = {"á","à","Á","À","é","è","É","È","í","ì","Í","Ì","ó","ò","Ó","Ò","ú","ù","Ú","Ù","ñ","Ñ","ç","Ç"};
    const char *sin_acento[] = {"a","a","A","A","e","e","E","E","i","i","I","I","o","o","O","O","u","u","U","U","n","N","c","C"};
    int num_letras = 24; // tamaño del array

    int i = 0; // posición para leer el string original
    int j = 0; // posición para escribir en el string final

    while (cadena[i] != '\0') {
        int encontrado = 0;
        for (int k = 0; k < num_letras; k++) {
            //para poder comparar, hay que comparar dos espacios porque las letras con acento ocupan el doble que las de sin acento
            if (cadena[i] == con_acento[k][0] && cadena[i+1] == con_acento[k][1]) { // si hay una letra con acento, se cambia
                cadena[j] = sin_acento[k][0]; // se guarda la letra sin acento en el string final
                i = i + 2; // se saltan dos posiciones en el string inicial, porque la letra acentuada ocupa el doble
                j = j + 1; // movemos solo una posicion del string final, porque la letra sin acento ocupa solo una posición
                encontrado = 1;
                break; // como ya hemos encontrado la coincidencia, acabamos el bucle
            }
        }
        if (encontrado == 0) {
            cadena[j] = cadena[i]; /// Si no hemos encontrado ningún acento, copiamos la letra tal cual
            i++;
            j++;
        }
    }
    cadena[j] = '\0';  // usamos un caracter vacio para acabar la palabra
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
        char nombre_calle[100]; 
        int num;
        double lat, lon;
        
        if (sscanf(linea, "%[^,],%d,%lf,%lf", nombre_calle, &num, &lat, &lon) == 4) {
            char calle_normalizada[100];
            normalizar_nombre(calle_normalizada, nombre_calle); // normalizamos el nombre
            
            cabeza = add_casa(cabeza, calle_normalizada, num, lat, lon); // si es correcto, se añade a la lista
            (*total)++;
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
        char nombre_lugar[150];
        double lat, lon;
        
        if (sscanf(linea, "%*[^,],%[^,],%*[^,],%lf,%lf", nombre_lugar, &lat, &lon) == 3) {
            cabeza = add_lugar(cabeza, nombre_lugar, lat, lon); // si el luegar es correcto, se añade a la lista
            (*total)++;
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