#include "sample_lib.h"

int main() {
    char map_name[20], path[150];
    int total_houses = 0;
    int opcion;

    printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
    scanf("%s", map_name);

    sprintf(path, "maps/%s/houses.txt", map_name);
    House *lista_casas = cargar_mapa(path, &total_houses);

    if (lista_casas == NULL) {
        printf("Error: Could not open map %s\n", path);
        return 1;
    }
    printf("%d houses loaded\n\n", total_houses);

    printf("--- ORIGIN ---\n");
    printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            buscar_direccion(lista_casas);
            break;
        case 2:
        case 3:
            printf("Not implemented yet.\n");
            break;
        default:
            printf("Invalid option.\n");
    }

    liberar_lista(lista_casas);
    return 0;
}