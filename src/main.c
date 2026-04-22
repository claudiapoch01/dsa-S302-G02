#include "sample_lib.h"

// devuelve 1 si el mapa es válido, 0 si no lo es
int mapa_valido(const char *m) {
    return strcmp(m, "xs_1") == 0 ||
           strcmp(m, "xs_2") == 0 ||
           strcmp(m, "md_1") == 0 ||
           strcmp(m, "lg_1") == 0 ||
           strcmp(m, "xl_1") == 0 ||
           strcmp(m, "2xl_1") == 0;
}

int main() {
    char map_name[20];
    char path_houses[150];
    char path_places[150];
    int num_houses = 0;
    int num_places = 0;
    int opcion;
    House *lista_casas = NULL;
    Place *lista_lugares = NULL;
    while (1) {
        // pide el nombre del mapa al usuario
        printf("Enter map name (xs_1, xs_2, md_1, lg_1, xl_1 or 2xl_1): ");
        leer_cadena_segura(map_name, sizeof(map_name));

        if (!mapa_valido(map_name)) { // si el mapa no es válido, muestra un mensaje de error y vuelve a pedirlo
            printf("Invalid map name '%s'. Try again.\n", map_name);
            continue; // vuelve a pedir
        }

        snprintf(path_houses, sizeof(path_houses), "maps/%s/houses.txt", map_name);
        snprintf(path_places, sizeof(path_places), "maps/%s/places.txt", map_name);

        num_houses = 0;
        num_places = 0;

        lista_casas = cargar_mapa(path_houses, &num_houses);
        lista_lugares = cargar_lugares(path_places, &num_places);

        if (lista_casas == NULL && lista_lugares == NULL) {
            // muestra error si no se han podido cargar datos 
            printf("Error: Could not load map data for '%s'. Check that the folder exists.\n", map_name);

            // liberamos la memoria
            if (lista_casas) { liberar_lista(lista_casas); lista_casas = NULL; }
            if (lista_lugares) { liberar_lugares(lista_lugares); lista_lugares = NULL; }
            continue; // vuelve a pedir mapa
        }

        break; // sale del bucle si el mapa se ha cargado bien
    }

    // muestra el número de casas y lugares cargados
    printf("%d houses loaded\n", num_houses);
    printf("%d places loaded\n", num_places);

    // Muestra el menú de opciones (dirección, lugar o coordenada)
    printf("--- ORIGIN ---\n");
    printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
    while (scanf("%d", &opcion) != 1) {
        printf("Invalid input. Please enter a number: ");
        int c; while ((c = getchar()) != '\n'); // Limpia la memoria
    }
    int c; while ((c = getchar()) != '\n'); // limpiamos la memoria
    
    switch (opcion) {
        case 1:  // caso 1: dirección
            buscar_direccion(lista_casas);
            break;
        case 2: // caso 2: lugar
            buscar_lugar(lista_lugares);
            break;
        case 3: // caso 3: coordenada
            printf("Not implemented yet.\n");
            break;
        default:
            printf("Invalid option.\n"); // Si no coincide con ninguna de las opciones, muestra un mensaje de error
            break;
    }

    // Se libera la memoria antes de salir
    if (lista_casas) liberar_lista(lista_casas);
    if (lista_lugares) liberar_lugares(lista_lugares);

    return 0;
}