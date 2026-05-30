#include "sample_lib.h"

int main() {
    char map_name[20];
    char path_houses[150];
    char path_places[150];
    char path_streets[150];

    int num_houses = 0;
    int num_places = 0;
    int num_streets = 0;
    int opcion;

    House *lista_casas = NULL;
    Place *lista_lugares = NULL;
    Street *lista_streets = NULL;
    
    StreetHashMap mi_mapa_hash;

    Grafo mi_grafo;
    mi_grafo.nodos = NULL;
    mi_grafo.total_nodos = 0;

    double lat_origen = 0.0, lon_origen = 0.0;
    double lat_destino = 0.0, lon_destino = 0.0;

    while (1) { // Bucle para pedir el mapa hasta que se introduzca uno válido
        printf("Enter map name (xs_1, xs_2, md_1, lg_1, xl_1 or 2xl_1): ");
        clearerr(stdin);
        leer_cadena_segura(map_name, sizeof(map_name)); // Lee el nombre del mapa de forma segura

        if (!mapa_valido(map_name)) {  // Pregunta al usuario hasta que introduzca un mapa válido
            printf("Invalid map name '%s'. Try again.\n", map_name);
            continue; 
        }

        sprintf(path_houses, "maps/%s/houses.txt", map_name);
        sprintf(path_places, "maps/%s/places.txt", map_name);
        sprintf(path_streets, "maps/%s/streets.txt", map_name);

        num_houses = 0; num_places = 0; num_streets = 0;

        // Cargamos las casas, lugares y calles del mapa elegido.
        lista_casas = cargar_mapa(path_houses, &num_houses);
        lista_lugares = cargar_lugares(path_places, &num_places);
        lista_streets = cargar_streets_con_hash(path_streets, &num_streets, &mi_mapa_hash);

        //si no se pueden cargar los datos, muestra un mensaje error.
        if (lista_casas == NULL && lista_lugares == NULL && lista_streets == NULL) { 
            printf("Error: Could not load map data for '%s'. Check that the folder exists.\n", map_name);
            if (lista_casas) { liberar_lista(lista_casas); lista_casas = NULL; }
            if (lista_lugares) { liberar_lugares(lista_lugares); lista_lugares = NULL; }
            if (lista_streets) { liberar_streets(lista_streets); lista_streets = NULL; }
            continue; 
        }

        mi_grafo = construir_grafo(lista_streets);
        break; 
    }

    // Printa el número de datos cargados.
    printf("%d houses loaded\n", num_houses);
    printf("%d places loaded\n", num_places);
    printf("%d streets loaded\n", num_streets);
    printf("%d graph nodes generated\n", mi_grafo.total_nodos);


    // Primera parte. El usuario elige su ubicación de origen.
    printf("\n- ORIGIN -\n");

    while (1) { 
        printf("Where are you? Address (1), Place (2) or Coordinate (3)? "); // Pregunta que tipo de ubicación quiere introducir

        // Revisa que los datos introducidos sean correctos. Si no, vuelve a preguntar.
        if (scanf(" %d", &opcion) != 1) {  
            printf("Invalid input. Please enter a number.\n");
            int c; while ((c = getchar()) != '\n' && c != EOF);
        } else if (opcion < 1 || opcion > 3) { 
            printf("Invalid option. Please choose 1, 2 or 3.\n");
        } else { 
            int c; while ((c = getchar()) != '\n' && c != EOF);
            break;
        }
    }

    //segun la opción elegida, se llama a la función correspondiente
    if (opcion == 1) { 
        buscar_direccion(lista_casas, &lat_origen, &lon_origen); 
        buscar_coordenada(&mi_grafo, &mi_mapa_hash, lat_origen, lon_origen);
    } else if (opcion == 2) { 
        buscar_lugar(lista_lugares, &lat_origen, &lon_origen);
        buscar_coordenada(&mi_grafo, &mi_mapa_hash, lat_origen, lon_origen);
    } else if (opcion == 3) {
        pedir_coordenadas_seguras(&lat_origen, &lon_origen);
        buscar_coordenada(&mi_grafo, &mi_mapa_hash, lat_origen, lon_origen);
    }

    // Segunda parte. El usuario elige su destino.
    printf("\n- DESTINATION -\n");

    while (1) {
        printf("Where do you want to go? Address (1), Place (2) or Coordinate (3)? "); //pregunta que tipo de ubicación quiere introducir

        // comprueba que el valor sea válido. Si no, vuelve a preguntar.
        if (scanf(" %d", &opcion) != 1) {
            printf("Invalid input. Please enter a number.\n");
            int c; while ((c = getchar()) != '\n' && c != EOF);
        } else if (opcion < 1 || opcion > 3) {
            printf("Invalid option. Please choose 1, 2 or 3.\n");
        } else {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            break;
        }
    }

    // dependiendo de la opción elegida, se llama a la función correspondiente
    if (opcion == 1) { 
        buscar_direccion(lista_casas, &lat_origen, &lon_destino); 
        buscar_coordenada(&mi_grafo, &mi_mapa_hash, lat_origen, lon_destino);
    } else if (opcion == 2) { 
        buscar_lugar(lista_lugares, &lat_origen, &lon_destino);
        buscar_coordenada(&mi_grafo, &mi_mapa_hash, lat_origen, lon_destino);
    } else if (opcion == 3) {
        pedir_coordenadas_seguras(&lat_origen, &lon_destino);
        buscar_coordenada(&mi_grafo, &mi_mapa_hash, lat_origen, lon_destino);
    }

    // buscamos los nodos más cercanos a las coordenadas de origen y destino para poder calcular la ruta entre ellos
    long long id_nodo_origen = buscar_nodo_mas_cercano(&mi_grafo, lat_origen, lon_origen);
    long long id_nodo_destino = buscar_nodo_mas_cercano(&mi_grafo, lat_destino, lon_destino);

    printf("Closest origin graph node: %lld\n", id_nodo_origen);
    printf("Closest destination graph node: %lld\n\n", id_nodo_destino);

    // calculamos la ruta. usamos o bfs o dijkstra
    calcular_ruta_bfs(&mi_grafo, &mi_mapa_hash, id_nodo_origen, id_nodo_destino);
    // calcular_ruta_dijkstra(&mi_grafo, id_nodo_origen, id_nodo_destino);

    // liberamos la memoria antes de acabar
    if (lista_casas) liberar_lista(lista_casas); 
    if (lista_lugares) liberar_lugares(lista_lugares);
    if (lista_streets) liberar_streets(lista_streets);

    hash_liberar(&mi_mapa_hash);
    
    liberar_grafo(&mi_grafo); 

    return 0;
}
