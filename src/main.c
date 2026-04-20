#include "sample_lib.h"

int main() {
    char map_name[20], path_houses[150], path_places[150];
    int num_houses = 0, num_places = 0;
    int opcion;

    printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
    scanf("%s", map_name);

    // Cargar Casas
    sprintf(path_houses, "maps/%s/houses.txt", map_name);
    House *lista_casas = cargar_mapa(path_houses, &num_houses);
    
    // Cargar Lugares
    sprintf(path_places, "maps/%s/places.txt", map_name);
    Place *lista_lugares = cargar_lugares(path_places, &num_places);

    if (!lista_casas && !lista_lugares) {
        printf("Error: Could not load map data for %s\n", map_name);
        return 1;
    }

    printf("%d houses loaded\n", num_houses);
    printf("%d places loaded\n\n", num_places);

    printf("--- ORIGIN ---\n");
    printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            buscar_direccion(lista_casas);
            break;
        case 2:
            buscar_lugar(lista_lugares);
            break;
        case 3:
            printf("Not implemented yet.\n");
            break;
        default:
            printf("Invalid option.\n");
    }

    // Limpieza
    if (lista_casas) liberar_lista(lista_casas);
    if (lista_lugares) liberar_lugares(lista_lugares);

    return 0;
}