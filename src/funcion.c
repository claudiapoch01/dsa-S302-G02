#include "sample_lib.h"
#include <math.h>

// funcion para buscar la calle mas cercana y las adyacentes a partir de unas coordenadas
void buscar_coordenada(Grafo *g, StreetHashMap *map, double user_lat, double user_lon) {
    if (map == NULL || g == NULL) return;

    // buscamos el nodo más cercano a las coordenadas
    long long id_nodo_cercano = buscar_nodo_mas_cercano(g, user_lat, user_lon);
    int idx_nodo = obtener_indice_nodo(g, id_nodo_cercano);
    if (idx_nodo == -1 || g->nodos[idx_nodo].num_vecinos == 0) {
        printf("    No street segments found near these coordinates.\n");
        return;
    }

    // cogemos el nombre de la calle más cercano usando hash
    int idx_vecino_original = g->nodos[idx_nodo].vecinos[0].nodo_destino;
    long long id_v = g->nodos[idx_vecino_original].id;
    const char *nombre_calle_actual = hash_buscar(map, id_nodo_cercano, id_v);
    if (nombre_calle_actual == NULL) {
        nombre_calle_actual = hash_buscar(map, id_v, id_nodo_cercano);
    }
    if (nombre_calle_actual == NULL) {
        nombre_calle_actual = "Calle desconocida";
    }

    // imprimimos los datos encontrados
    printf("\n    Found at (%lf, %lf)\n", user_lat, user_lon);
    printf("    Closest street: %s\n", nombre_calle_actual);
    printf("    Between %lld (%lf, %lf) and %lld (%lf, %lf)\n", 
           id_nodo_cercano, g->nodos[idx_nodo].latitud, g->nodos[idx_nodo].longitud, 
           id_v, g->nodos[idx_vecino_original].latitud, g->nodos[idx_vecino_original].longitud);

    printf("\n    From this street segment, you can go to:\n");
    printf("    - %s\n", nombre_calle_actual);
    printf("        Which is connected to:\n");
    
    char calles_sugeridas[30][100]; 
    int num_sugeridas = 0;

    // buscamos los vecinos de ambos lados del nodo
    int nodos_a_explorar[2] = {idx_nodo, idx_vecino_original};

    for (int i = 0; i < 2; i++) {
        int idx_actual_explorando = nodos_a_explorar[i];
        long long id_actual_explorando = g->nodos[idx_actual_explorando].id;

        for (int v = 0; v < g->nodos[idx_actual_explorando].num_vecinos; v++) {
            int idx_destino = g->nodos[idx_actual_explorando].vecinos[v].nodo_destino;
            long long id_destino = g->nodos[idx_destino].id;

            // Recuperamos el nombre de la calle vecina
            const char *nombre_vecino = hash_buscar(map, id_actual_explorando, id_destino);
            if (nombre_vecino == NULL) { 
                nombre_vecino = hash_buscar(map, id_destino, id_actual_explorando); 
            }
            if (nombre_vecino == NULL) continue; 

            // Si es una calle distinta a la nuestra, la guardamos
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
    }
    
    if (num_sugeridas == 0) { // si no se encuentran calles vecinas, printamos un mensaje
        printf("         - No distinct connecting streets found.\n");
    }
    printf("\n");
}