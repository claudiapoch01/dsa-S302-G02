#include "sample_lib.h"

int main() {
    char map_name[20];
    char path_houses[150];
    char path_places[150];
    int num_houses = 0;
    int num_places = 0;
    int opcion;
    House *lista_casas = NULL;
    Place *lista_lugares = NULL;

    while (1) { // va a estar en el bucle hasta que se haga un break al cargar un mapa correctamente
        // pide el nombre del mapa al usuario
        printf("Enter map name (xs_1, xs_2, md_1, lg_1, xl_1 or 2xl_1): ");
        leer_cadena_segura(map_name, sizeof(map_name));

        if (!mapa_valido(map_name)) { // si el mapa no es válido, muestra un mensaje de error y vuelve a pedirlo
            printf("Invalid map name '%s'. Try again.\n", map_name);
            continue; // vuelve a pedir
        }

        // Se crean las rutas de los archivos de casas y lugares
        sprintf(path_houses, "maps/%s/houses.txt", map_name);
        sprintf(path_places, "maps/%s/places.txt", map_name);

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
    while (1) { // bucle que no finaliza hasta el break cuando el usuario introduce una opción válida
        printf("Where are you? Address (1), Place (2) or Coordinate (3)? ");

        if (scanf("%d", &opcion) != 1) { // controla el error de si el usuario no introduce un valor numérico
            printf("Invalid input. Please enter a number.\n");
        } else if (opcion < 1 || opcion > 3) { // comprueba que el número introducido es 1, 2 o 3
            printf("Invalid option. Please choose 1, 2 or 3.\n");
        } else { // si es correcto, sale del bucle
            int c;
            while ((c = getchar()) != '\n') { } // limpia resto de línea
            break;
        }

        // si no ha salido aun del bucle, significa que ha introducido un valor incorrecto
        int c;
        while ((c = getchar()) != '\n') { } // se limpia la memoria y se repite el bucle
    }
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