#include "sample_lib.h"
#include <math.h>

void buscar_coordenada(Street *lista_streets, House *lista_casas, double user_lat, double user_lon) {
    if (lista_streets == NULL) return;

    Street *actual = lista_streets;
    Street *closest = NULL;
    double min_dist = -1.0;

    // 1. Buscamos el segmento de calle más cercano al usuario con Haversine
    while (actual != NULL) {
        double dist = haversine(user_lat, user_lon, actual->mid_lat, actual->mid_lon);
        if (min_dist < 0 || dist < min_dist) {
            min_dist = dist;
            closest = actual;
        }
        actual = actual->next;
    }
    
    if (closest == NULL) {
        printf("No street segments found near these coordinates.\n");
        return;
    }

    // 2. Imprimimos los datos del tramo más cercano
    printf("\nFound at (%lf, %lf)\n", user_lat, user_lon);
    printf("Closest street: %s\n", closest->street_name);
    printf("Between %lld (%lf, %lf) and %lld (%lf, %lf)\n", 
           closest->id1, closest->lat1, closest->lon1, 
           closest->id2, closest->lat2, closest->lon2);
    printf("Distance to street center: %.2f meters\n", min_dist);

    // 3. Buscamos e imprimimos las calles conectadas sin repetir nombres
    printf("\nFrom this street segment, you can go to:\n");
    printf("- %s\n", closest->street_name);
    printf("    Which is connected to:\n");

    actual = lista_streets;
    char calles_sugeridas[20][100]; // Array de control local para evitar duplicados en pantalla
    int num_sugeridas = 0;

    while (actual != NULL) {
        // Comprobamos si comparte algún nodo extremo con nuestro segmento más cercano
        if (actual != closest) {
            if (actual->id1 == closest->id1 || actual->id1 == closest->id2 ||
                actual->id2 == closest->id1 || actual->id2 == closest->id2) {
                
                // Filtro 1: No sugerir la misma calle principal en la que ya nos encontramos
                if (strcasecmp(closest->street_name, actual->street_name) != 0) {
                    
                    // Filtro 2: Comprobar si ya metimos este nombre de calle de cruce antes
                    int ya_existe = 0;
                    for (int i = 0; i < num_sugeridas; i++) {
                        if (strcasecmp(calles_sugeridas[i], actual->street_name) == 0) {
                            ya_existe = 1;
                            break;
                        }
                    }

                    // Si es una calle de intersección nueva y válida, la guardamos e imprimimos
                    if (!ya_existe && num_sugeridas < 20) {
                        strcpy(calles_sugeridas[num_sugeridas], actual->street_name);
                        num_sugeridas++;
                        printf("     - %s\n", actual->street_name);
                    }
                }
            }
        }
        actual = actual->next;
    }

    if (num_sugeridas == 0) {
        printf("     - No distinct connecting streets found (dead end or single segment).\n");
    }
    printf("\n");
}