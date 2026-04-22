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

// devuelve 1 si el mapa es válido, 0 si no lo es
int mapa_valido(const char *m) {
    return strcmp(m, "xs_1") == 0 ||
           strcmp(m, "xs_2") == 0 ||
           strcmp(m, "md_1") == 0 ||
           strcmp(m, "lg_1") == 0 ||
           strcmp(m, "xl_1") == 0 ||
           strcmp(m, "2xl_1") == 0;
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

// funcion para normalizar las direcciones
void normalizar_nombre(char *dest, const char *src) {
    // se usa la funcion strncasecmp que es como strncmp pero ignora mayúsculas y minúsculas
    // comprueba las abreviaturas de calle (C., C/, etc) y las normaliza a 'Carrer'
    if (strncasecmp(src, "C. de ", 6) == 0 || strncasecmp(src, "C/ de ", 6) == 0) {
        strcpy(dest, "Carrer ");
        strcat(dest, src + 6);
    } else if (strncasecmp(src, "Carrer de ", 10) == 0) {
        strcpy(dest, "Carrer ");
        strcat(dest, src + 10);
    } else if (strncasecmp(src, "C. ", 3) == 0 || strncasecmp(src, "C/ ", 3) == 0) {
        strcpy(dest, "Carrer ");
        strcat(dest, src + 3);
    } else if (strncasecmp(src, "Carrer ", 7) == 0) {
        strcpy(dest, "Carrer ");
        strcat(dest, src + 7);
    } 
    // comprobaciones de abreviaturas de avenida (Av., Avda., etc) y las normaliza a 'Avinguda'
    else if (strncasecmp(src, "Avda. de ", 9) == 0) {
        strcpy(dest, "Avinguda ");
        strcat(dest, src + 9);
    } else if (strncasecmp(src, "Av. de ", 7) == 0) {
        strcpy(dest, "Avinguda ");
        strcat(dest, src + 7);
    } else if (strncasecmp(src, "Avinguda de ", 12) == 0) {
        strcpy(dest, "Avinguda ");
        strcat(dest, src + 12);
    } else if (strncasecmp(src, "Avda. ", 6) == 0) {
        strcpy(dest, "Avinguda ");
        strcat(dest, src + 6);
    } else if (strncasecmp(src, "Av. ", 4) == 0) {
        strcpy(dest, "Avinguda ");
        strcat(dest, src + 4);
    } else if (strncasecmp(src, "Avinguda ", 9) == 0) {
        strcpy(dest, "Avinguda ");
        strcat(dest, src + 9);
    }
    // comprobaciones de abreviaturas de plaza (Pca., Pl., etc) y las normaliza a 'Placa'
    else if (strncasecmp(src, "Pca. de ", 8) == 0) {
        strcpy(dest, "Placa ");
        strcat(dest, src + 8);
    } else if (strncasecmp(src, "Pl. de ", 7) == 0) {
        strcpy(dest, "Placa ");
        strcat(dest, src + 7);
    } else if (strncasecmp(src, "Plaça de ", 10) == 0) {
        strcpy(dest, "Placa ");
        strcat(dest, src + 10); 
    } else if (strncasecmp(src, "Placa de ", 9) == 0) {
        strcpy(dest, "Placa ");
        strcat(dest, src + 9);
    } else if (strncasecmp(src, "Pl. ", 4) == 0 || strncasecmp(src, "Pca. ", 5) == 0) {
        strcpy(dest, "Placa ");
        strcat(dest, (src[2] == '.') ? src + 4 : src + 5);
    } else if (strncasecmp(src, "Plaça ", 7) == 0) {
        strcpy(dest, "Placa ");
        strcat(dest, src + 7);
    } else if (strncasecmp(src, "Placa ", 6) == 0) {
        strcpy(dest, "Placa ");
        strcat(dest, src + 6);
    }
    // comprobaciones de abreviaturas de rambla (Rbla., Rbla de., etc) y las normaliza a 'Rambla'
    else if (strncasecmp(src, "Rbla. de ", 9) == 0 || strncasecmp(src, "Rbla de ", 8) == 0) {
        strcpy(dest, "Rambla ");
        strcat(dest, (src[4] == '.') ? src + 9 : src + 8);
    } else if (strncasecmp(src, "Rambla de ", 10) == 0) {
        strcpy(dest, "Rambla ");
        strcat(dest, src + 10);
    }
    // si no coincide con ninguna de las abreviaturas, se copia el nombre tal cual
    else {
        strcpy(dest, src);
    }

    quitar_acentos(dest); // eliminamos acentos
}

void leer_cadena_segura(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; //quitamos el salto de línea
        } else {
            // si el usuario ha metido muchos caracteres, se eliminan los que sobran 
            int c;
            while ((c = getchar()) != '\n');
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
    leer_cadena_segura(raw_name, 100); // leemos el nombre de la calle
    normalizar_nombre(street_search, raw_name);

    printf("Enter street number: ");
    while (scanf("%d", &num_search) != 1) { // si el usuario no introduce un número, vuelve a preguntarlo
        printf("Invalid input. Please enter a numeric value: ");
        int c; while ((c = getchar()) != '\n'); // borra el valor anterior
    }

    while (getchar() != '\n'); // borramos la memoria

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
        while (scanf("%d", &num_search) != 1) { // <--- Añade esta validación aquí también
            printf("Invalid input. Please enter a numeric value: ");
            int c; while ((c = getchar()) != '\n');
        }
        
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
    char search_name_normalizado[150];
    
    printf("Enter place name: ");
    leer_cadena_segura(search_name, 100);

    strcpy(search_name_normalizado, search_name);
    quitar_acentos(search_name_normalizado); // eliminamos acentos del lugar escrito por el usuario
    
    Place *actual = lista; 

    Place *coincidencias[50]; // llista per guardar tots els llocs possibles
    int num_coincidencias = 0; // mida del array anterior

    Place *mejor_lugar = NULL;
    int min_dist = 100;

    while (actual != NULL) {
        char place_sin_acentos[150];
        strcpy(place_sin_acentos, actual->name);
        quitar_acentos(place_sin_acentos); // eliminamos acentos del lugar del documento

        if (strcasecmp(place_sin_acentos, search_name_normalizado) == 0) {
            if (num_coincidencias < 50) {
                coincidencias[num_coincidencias] = actual;
                num_coincidencias++;
            }
        }
        
        int d = distancia_levenshtein(search_name, actual->name);
        if (d < min_dist) {
            min_dist = d;
            mejor_lugar = actual;
        }
        actual = actual->next;
    }
    // comprobamos si hay varios lugares que se llaman igual
    if (num_coincidencias == 1) { // si solo hay uno, lo muestra
        printf("\n    Found at (%lf, %lf)\n", coincidencias[0]->latitud, coincidencias[0]->longitud);
    } else if (num_coincidencias > 1) { // si hay varios, deja elegir

        printf("\nMultiple places found with that name:\n");

        // por cada una de las coincidencias, muestra su nombre y coordenadas para que el usuario pueda elegir
        for (int i = 0; i < num_coincidencias; i++) { 
            printf("%d. %s at (%lf, %lf)\n", i + 1, coincidencias[i]->name, coincidencias[i]->latitud, coincidencias[i]->longitud);
        }
        
        int seleccion;
        printf("Choose one (1-%d): ", num_coincidencias); // pide al usuario que elija un lugar por su número
        while (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > num_coincidencias) {
            printf("Invalid selection. Please choose a number between 1 and %d: ", num_coincidencias);
            int c; while ((c = getchar()) != '\n');
        }
        
        printf("\n    Selected: %s at (%lf, %lf)\n", coincidencias[seleccion - 1]->name, coincidencias[seleccion - 1]->latitud, coincidencias[seleccion - 1]->longitud);
    } 
    else if (mejor_lugar != NULL && min_dist < 10) {
        printf("Place not found. Did you mean: %s?\n", mejor_lugar->name);
    } 
    else {
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