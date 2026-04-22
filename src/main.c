#include "sample_lib.h"

int main() {
    char map_name[20];
    char path_houses[150];
    char path_places[150];
    int num_houses = 0;
    int num_places = 0;
    int opcion;

    // 1. Pedir el nombre del mapa
    printf("Enter map name (e.g. 'xs_2' or 'xl_1'): ");
    if (scanf("%s", map_name) != 1) return 1;

    // 2. Construir rutas de archivos
    sprintf(path_houses, "maps/%s/houses.txt", map_name);
    sprintf(path_places, "maps/%s/places.txt", map_name);

    // 3. Cargar datos en las listas enlazadas
    House *lista_casas = cargar_mapa(path_houses, &num_houses);
    Place *lista_lugares = cargar_lugares(path_places, &num_places);

    // 4. Verificar si se ha cargado algo
    if (lista_casas == NULL && lista_lugares == NULL) {
        printf("Error: Could not load map data for %s\n", map_name);
        printf("Asegurate de que la carpeta 'maps' este en la raiz del proyecto.\n");
        return 1;
    }

    // 5. Mostrar resumen de carga
    printf("%d houses loaded\n", num_houses);
    printf("%d places loaded\n\n", num_places);

    // 6. Menú principal
    printf("--- ORIGIN ---\n");
    printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            buscar_direccion(lista_casas);
            break;
        case 2:
            getchar();
            buscar_lugar(lista_lugares);
            break;
        case 3:
            printf("Not implemented yet.\n");
            break;
    }

    // 7. Liberar memoria antes de salir
    if (lista_casas) liberar_lista(lista_casas);
    if (lista_lugares) liberar_lugares(lista_lugares);

    return 0;
}