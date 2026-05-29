#include "sample_lib.h"
#include <math.h>

void buscar_coordenada(Grafo *g, Street *lista_streets, House *lista_casas, double user_lat, double user_lon) {
    if (lista_streets == NULL || g == NULL) return;

    Street *actual = lista_streets;
    Street *closest = NULL;
    double min_dist = -1.0;

    // 1. Buscamos el segmento de calle más cercano al usuario
    while (actual != NULL) {
        double real_mid_lat = (actual->lat1 + actual->lat2) / 2.0;
        double real_mid_lon = (actual->lon1 + actual->lon2) / 2.0;

        double dist = haversine(user_lat, user_lon, real_mid_lat, real_mid_lon);
        if (min_dist < 0 || dist < min_dist) {
            min_dist = dist;
            closest = actual;
        }
        actual = actual->next;
    }
    
    if (closest == NULL) {
        printf("    No street segments found near these coordinates.\n");
        return;
    }

    // 2. Imprimimos los datos del tramo
    printf("\n    Found at (%lf, %lf)\n", user_lat, user_lon);
    printf("    Closest street: %s\n", closest->street_name);
    printf("    Between %lld (%lf, %lf) and %lld (%lf, %lf)\n", 
           closest->id1, closest->lat1, closest->lon1, 
           closest->id2, closest->lat2, closest->lon2);
    printf("    Distance to street center: %.2f meters\n", min_dist);

    printf("\n    From this street segment, you can go to:\n");
    printf("    - %s\n", closest->street_name);
    printf("        Which is connected to:\n");
    
    int idx1 = obtener_indice_nodo(g, closest->id1);
    int idx2 = obtener_indice_nodo(g, closest->id2);

    char calles_sugeridas[30][100]; 
    int num_sugeridas = 0;

    int nodos_extremos[2] = {idx1, idx2};
    for (int i = 0; i < 2; i++) {
        int nodo_actual = nodos_extremos[i];
        if (nodo_actual == -1) continue;

        for (int v = 0; v < g->nodos[nodo_actual].num_vecinos; v++) {
            int idx_destino = g->nodos[nodo_actual].vecinos[v].nodo_destino;
            long long id_destino = g->nodos[idx_destino].id;
            long long id_origen = g->nodos[nodo_actual].id;

            Street *st = lista_streets;
            double v_mid_lat = 0.0, v_mid_lon = 0.0;
            
            // Usamos el Grafo para sacar el segmento vecino al instante
            while (st != NULL) {
                if ((st->id1 == id_origen && st->id2 == id_destino) ||
                    (st->id2 == id_origen && st->id1 == id_destino)) {
                    v_mid_lat = (st->lat1 + st->lat2) / 2.0;
                    v_mid_lon = (st->lon1 + st->lon2) / 2.0;
                    break; 
                }
                st = st->next;
            }

            if (v_mid_lat == 0.0) continue;

            // --- AQUÍ VUELVE TU SOLUCIÓN EVOLUCIONADA ---
            char nombre_vecino[100];
            strcpy(nombre_vecino, "Calle desconocida");
            House *act_casa = lista_casas;
            double m_d = -1.0;
            
            while (act_casa != NULL) {
                double d = haversine(v_mid_lat, v_mid_lon, act_casa->latitud, act_casa->longitud);
                if (m_d < 0 || d < m_d) { 
                    m_d = d; 
                    strncpy(nombre_vecino, act_casa->street_name, 99); 
                    nombre_vecino[99] = '\0';
                }
                act_casa = act_casa->next;
            }

            // Comparamos el nombre oficial filtrado por tu lógica
            if (strcasecmp(closest->street_name, nombre_vecino) != 0) {
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
    
    if (num_sugeridas == 0) {
        printf("         - No distinct connecting streets found.\n");
    }
    printf("\n");
}