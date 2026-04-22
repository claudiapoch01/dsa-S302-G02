#include "sample_lib.h"

int main() {
    char map_name[20];
    char path_houses[150];
    char path_places[150];
    int num_houses = 0;
    int num_places = 0;
    int opcion;

    // Pide el nombre del mapa
    printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
    leer_cadena_segura(map_name, 20);

    // Se crean las rutas de los archivos de casas y lugares
    sprintf(path_houses, "maps/%s/houses.txt", map_name);
    sprintf(path_places, "maps/%s/places.txt", map_name);

    // Carga los datos en las listas enlazadas
    House *lista_casas = cargar_mapa(path_houses, &num_houses);
    Place *lista_lugares = cargar_lugares(path_places, &num_places);

    // Comprueba si se ha cargado al menos una de las dos listas, sino muestra un mensaje de error
    if(lista_casas == NULL && lista_lugares == NULL) {
        printf("Error: Could not load map data for %s\n", map_name);
        return 1;
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
        case 1:  // Caso 1: dirección
            buscar_direccion(lista_casas);
            break;
        case 2: // Caso 2: lugar
            buscar_lugar(lista_lugares);
            break;
        case 3: // Caso 3: coordenada
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