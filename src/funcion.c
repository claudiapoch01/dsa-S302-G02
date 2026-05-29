#include "sample_lib.h"
#include <math.h>

void buscar_coordenada(Grafo *g, StreetHashMap *map, House *lista_casas, double user_lat, double user_lon) {
    if (map == NULL || g == NULL) return;

    // 1. Buscamos el nodo más cercano en el grafo para geolocalizar al usuario
    long long id_nodo_cercano = buscar_nodo_mas_cercano(g, user_lat, user_lon);
    int idx_nodo = obtener_indice_nodo(g, id_nodo_cercano);
    if (idx_nodo == -1 || g->nodos[idx_nodo].num_vecinos == 0) {
        printf("    No street segments found near these coordinates.\n");
        return;
    }

    // Identificamos el nombre de la calle usando el mapa hash en O(1)
    int idx_vecino = g->nodos[idx_nodo].vecinos[0].nodo_destino;
    long long id_v = g->nodos[idx_vecino].id;
    const char *nombre_calle_actual = hash_buscar(map, id_nodo_cercano, id_v);

    if (nombre_calle_actual == NULL) {
        nombre_calle_actual = "Calle desconocida";
    }

    // 2. Imprimimos los datos del tramo
    printf("\n    Found at (%lf, %lf)\n", user_lat, user_lon);
    printf("    Closest street: %s\n", nombre_calle_actual);
    printf("    Between %lld (%lf, %lf) and %lld (%lf, %lf)\n", 
           id_nodo_cercano, g->nodos[idx_nodo].latitud, g->nodos[idx_nodo].longitud, 
           id_v, g->nodos[idx_vecino].latitud, g->nodos[idx_vecino].longitud);

    printf("\n    From this street segment, you can go to:\n");
    printf("    - %s\n", nombre_calle_actual);
    printf("        Which is connected to:\n");
    
    char calles_sugeridas[30][100]; 
    int num_sugeridas = 0;

    // ¡MÁXIMA EVOLUCIÓN O(1)! Buscamos las calles conectadas directamente
    for (int v = 0; v < g->nodos[idx_nodo].num_vecinos; v++) {
        int idx_destino = g->nodos[idx_nodo].vecinos[v].nodo_destino;
        long long id_destino = g->nodos[idx_destino].id;

        // Recuperamos el nombre de la calle vecina instantáneamente desde la Hash
        const char *nombre_vecino = hash_buscar(map, id_nodo_cercano, id_destino);
        if (nombre_vecino == NULL) continue;

        if (strcasecmp(nombre_calle_actual, nombre_vecino) != 0) {
            int ya_existe = 0;
            for (int k = 0; k < num_sugeridas; k++) {
                if (strcasecmp(calles_sugeridas[k], nombre_vecino) == 0) {
                    ya_existe = 1;
                    break;
                }
            }

            if (!ya_existe && num_sugeridas < 30) {
                strcpy(calles_sugeridas[num_sugeridas], nombre_vecino);
                num_sugeridas++;
                printf("         - %s\n", nombre_vecino);
            }
        }
    }
    
    if (num_sugeridas == 0) {
        printf("         - No distinct connecting streets found.\n");
    }
    printf("\n");
}