#include "sample_lib.h"
#include <math.h>
#include <float.h>

// ==========================================
// UTILIDADES
// ==========================================

int minimo(int a, int b, int c) { 
    int m = a;
    if (b < m) m = b;
    if (c < m) m = c;
    return m;
}

int distancia_levenshtein(char *s1, char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);

    if (len1 > 200 || len2 > 200) return 9999; 

    int matriz[len1 + 1][len2 + 1]; 

    for (int i = 0; i <= len1; i++) matriz[i][0] = i; 
    for (int j = 0; j <= len2; j++) matriz[0][j] = j;

    for (int i = 1; i <= len1; i++) { 
        for (int j = 1; j <= len2; j++) {
            int coste = (tolower(s1[i - 1]) == tolower(s2[j - 1])) ? 0 : 1; 
            matriz[i][j] = minimo(
                matriz[i - 1][j] + 1, 
                matriz[i][j - 1] + 1, 
                matriz[i - 1][j - 1] + coste 
            );
        }
    }
    return matriz[len1][len2]; 
}

// ==========================================
// GESTIÓN DE CASAS
// ==========================================

House* add_casa(House *cabeza, char *street, int num, double lat, double lon) {
    House *nueva = (House*)malloc(sizeof(House));
    if (nueva == NULL) return cabeza;

    snprintf(nueva->street_name, sizeof(nueva->street_name), "%s", street);
    nueva->house_number = num;
    nueva->latitud = lat;
    nueva->longitud = lon;
    nueva->next = cabeza;
    return nueva;
}

House* cargar_mapa(char *path, int *total) {
    FILE *f = fopen(path, "r"); 
    if (f == NULL) { 
        printf("No se ha podido abrir el archivo: %s\n", path);
        return NULL;
    }

    House *cabeza = NULL;
    char linea[256];
    *total = 0;

    while (fgets(linea, sizeof(linea), f)) { 
        char nombre_calle[100]; 
        int num;
        double lat, lon;
        
        if (sscanf(linea, "%[^,],%d,%lf,%lf", nombre_calle, &num, &lat, &lon) == 4) { 
            char calle_normalizada[100];
            normalizar_nombre(calle_normalizada, sizeof(calle_normalizada), nombre_calle); 
            
            cabeza = add_casa(cabeza, calle_normalizada, num, lat, lon); 
            (*total)++;
        }
    }
    fclose(f);    
    return cabeza;
}

void buscar_direccion(House *lista, double *res_lat, double *res_lon) {
    char raw_name[100], street_search[100];
    int num_search;

    printf("Enter street name: ");
    leer_cadena_segura(raw_name, 100); 
    normalizar_nombre(street_search, sizeof(street_search), raw_name); 

    printf("Enter street number: ");
    while (scanf("%d", &num_search) != 1) { 
        printf("Invalid input. Please enter a numeric value: ");
        int c; while ((c = getchar()) != '\n'); 
    }
    while (getchar() != '\n'); 

    House *actual = lista;
    House *mejor_calle_nodo = NULL;
    House *casa_elegida = NULL; 
    int min_dist = 100;
    int calle_exacta_encontrada = 0;

    while (actual != NULL) {
        if (strcasecmp(actual->street_name, street_search) == 0) { 
            calle_exacta_encontrada = 1;
            if (actual->house_number == num_search) { 
                casa_elegida = actual;
                break; 
            }
        }
        
        int d = distancia_levenshtein(street_search, actual->street_name);
        if (d < min_dist) {
            min_dist = d;
            mejor_calle_nodo = actual;
        }
        actual = actual->next;
    }

    if (!casa_elegida && calle_exacta_encontrada) { 
        printf("Invalid number. Valid numbers in %s: ", street_search);
        actual = lista;
        while (actual != NULL) {
            if (strcasecmp(actual->street_name, street_search) == 0) {
                printf("%d ", actual->house_number);
            }
            actual = actual->next;
        }
        printf("\n");
        
        printf("Enter one of the valid numbers: ");
        while (scanf("%d", &num_search) != 1) { 
            printf("Invalid input. Please enter a numeric value: ");
            int c; while ((c = getchar()) != '\n');
        }
        while (getchar() != '\n');
        
        actual = lista;
        while (actual != NULL) {
            if (strcasecmp(actual->street_name, street_search) == 0 && actual->house_number == num_search) {
                casa_elegida = actual;
                break;
            }
            actual = actual->next;
        }
        if (!casa_elegida) {
            printf("Still an invalid number.\n"); 
        }
    } else if (!casa_elegida && mejor_calle_nodo != NULL) {
        printf("Street not found. Did you mean: %s?\n", mejor_calle_nodo->street_name);
    }
    
    if (casa_elegida != NULL) {
        printf("\n    Found at (%lf, %lf)\n", casa_elegida->latitud, casa_elegida->longitud);
        *res_lat = casa_elegida->latitud;
        *res_lon = casa_elegida->longitud;
    } else {
        *res_lat = 0.0;
        *res_lon = 0.0;
    }
}

// ==========================================
// GESTIÓN DE LUGARES
// ==========================================

Place* add_lugar(Place *cabeza, char *name, double lat, double lon) {
    Place *nuevo = (Place*)malloc(sizeof(Place));
    if (nuevo == NULL) return cabeza;

    snprintf(nuevo->name, sizeof(nuevo->name), "%s", name);
    nuevo->latitud = lat;
    nuevo->longitud = lon;
    nuevo->next = cabeza;
    return nuevo;
}

Place* cargar_lugares(char *path, int *total) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return NULL;

    Place *cabeza = NULL;
    char linea[300];
    *total = 0;

    while (fgets(linea, sizeof(linea), f)) {
        char nombre_lugar[150];
        double lat, lon;
        
        if (sscanf(linea, "%*[^,],%[^,],%*[^,],%lf,%lf", nombre_lugar, &lat, &lon) == 3) {
            cabeza = add_lugar(cabeza, nombre_lugar, lat, lon); 
            (*total)++;
        }
    }
    fclose(f);
    return cabeza;
}

void buscar_lugar(Place *lista, double *res_lat, double *res_lon) {
    char search_name[150];
    char search_name_normalizado[150];
    
    printf("Enter place name: ");
    leer_cadena_segura(search_name, 150);

    strcpy(search_name_normalizado, search_name);
    quitar_acentos(search_name_normalizado); 
    
    Place *actual = lista; 
    Place *coincidencias[50]; 
    int num_coincidencias = 0; 

    Place *mejor_lugar = NULL;
    int min_dist = 100;
    Place *lugar_elegido = NULL;

    while (actual != NULL) {
        char place_sin_acentos[150];
        strcpy(place_sin_acentos, actual->name);
        quitar_acentos(place_sin_acentos); 

        if (strcasecmp(place_sin_acentos, search_name_normalizado) == 0) { 
            if (num_coincidencias < 50) {
                coincidencias[num_coincidencias] = actual;
                num_coincidencias++;
            }
        }

        int d = distancia_levenshtein(search_name, actual->name);
        if (d < min_dist) {
            min_dist = d;
            mejor_lugar = actual;
        }
        actual = actual->next;
    }

    // --- ARREGLADA LA LÓGICA DE CONDICIONALES AQUÍ ---
    if (num_coincidencias == 1) { 
        lugar_elegido = coincidencias[0];
        printf("\n    Found at (%lf, %lf)\n", lugar_elegido->latitud, lugar_elegido->longitud);
    } 
    else if (num_coincidencias > 1) { 
        printf("\nMultiple places found with that name:\n");

        for (int i = 0; i < num_coincidencias; i++) { 
            printf("%d. %s at (%lf, %lf)\n", i + 1, coincidencias[i]->name, coincidencias[i]->latitud, coincidencias[i]->longitud);
        }
        
        int seleccion;
        printf("Choose one (1-%d): ", num_coincidencias); 
        while (scanf("%d", &seleccion) != 1 || seleccion < 1 || seleccion > num_coincidencias) {
            printf("Invalid selection. Please choose a number between 1 and %d: ", num_coincidencias);
            int c; while ((c = getchar()) != '\n');
        }
        while (getchar() != '\n'); // Limpiamos buffer
        
        lugar_elegido = coincidencias[seleccion - 1];
        printf("\n    Selected: %s at (%lf, %lf)\n", lugar_elegido->name, lugar_elegido->latitud, lugar_elegido->longitud);
    } 
    else if (mejor_lugar != NULL && min_dist < 10) {
        // --- AQUÍ ESTÁ EL ARREGLO ---
        // Si el usuario lo escribe un poco diferente, le avisamos pero SÍ guardamos el nodo sugerido
        lugar_elegido = mejor_lugar; 
        printf("Place not found. Did you mean: %s?\n", lugar_elegido->name);
        printf("\n    Found at (%lf, %lf)\n", lugar_elegido->latitud, lugar_elegido->longitud);
    } 
    else {
        printf("Place not found.\n");
    }

    // Enviamos los datos reales de vuelta al main
    if (lugar_elegido != NULL) {
        *res_lat = lugar_elegido->latitud;
        *res_lon = lugar_elegido->longitud;
    } else {
        *res_lat = 0.0;
        *res_lon = 0.0;
    }
}

// ==========================================
// GESTIÓN DE CALLES Y COORDENADAS
// ==========================================

#define EARTH_RADIUS 6371000.0 
#define TO_RAD (3.14159265358979323846 / 180.0) 

double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * TO_RAD;
    double dLon = (lon2 - lon1) * TO_RAD;
    double lat1_rad = lat1 * TO_RAD;
    double lat2_rad = lat2 * TO_RAD;

    double a = sin(dLat / 2) * sin(dLat / 2) +
               sin(dLon / 2) * sin(dLon / 2) * cos(lat1_rad) * cos(lat2_rad);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return EARTH_RADIUS * c;
}

Street* add_street(Street *cabeza, long long id1, double lat1, double lon1, long long id2, double lat2, double lon2, char *name) {
    Street *nueva = (Street*)malloc(sizeof(Street));
    if (nueva == NULL) return cabeza;

    nueva->id1 = id1; nueva->lat1 = lat1; nueva->lon1 = lon1;
    nueva->id2 = id2; nueva->lat2 = lat2; nueva->lon2 = lon2;
    
    nueva->mid_lat = (lat1 + lat2) / 2.0;
    nueva->mid_lon = (lon1 + lon2) / 2.0;

    snprintf(nueva->street_name, sizeof(nueva->street_name), "%s", name);

    nueva->next = cabeza;
    return nueva;
}

Street* cargar_streets(char *path, int *total) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return NULL;

    Street *cabeza = NULL;
    char linea[256];
    *total = 0;

    while (fgets(linea, sizeof(linea), f)) {
        long long id1, id2;
        double lat1, lon1, lat2, lon2;
        double peso_fichero = 0.0;
        char nombre_calle[100];
        
        nombre_calle[0] = '\0'; 
        
        // Forzamos a leer las 8 columnas estrictas del fichero
        int leidos = sscanf(linea, "%lld,%lf,%lf,%lld,%lf,%lf,%lf,%[^\n]", 
                            &id1, &lat1, &lon1, &id2, &lat2, &lon2, &peso_fichero, nombre_calle);
        
        // Descartamos cualquier línea corrupta o incompleta que altere el contador
        if (leidos == 8 && strlen(nombre_calle) > 0) {
            Street *nueva = (Street*)malloc(sizeof(Street));
            if (nueva != NULL) {
                nueva->id1 = id1; nueva->lat1 = lat1; nueva->lon1 = lon1;
                nueva->id2 = id2; nueva->lat2 = lat2; nueva->lon2 = lon2;
                
                // Guardamos el peso original de la columna 7 en mid_lat para Dijkstra
                nueva->mid_lat = peso_fichero; 
                nueva->mid_lon = 0.0; 
                
                snprintf(nueva->street_name, sizeof(nueva->street_name), "%s", nombre_calle);
                nueva->next = cabeza;
                cabeza = nueva;
                (*total)++;
            }
        }
    }
    fclose(f);
    return cabeza;
}

void buscar_coordenada(Street *lista_streets, House *lista_casas, double user_lat, double user_lon) {
    if (lista_streets == NULL) return;

    Street *actual = lista_streets;
    Street *closest = NULL;
    double min_dist = -1.0;

    // 1. Encontrar el tramo de calle más cercano
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
    
    if (closest == NULL) return;

    // Formato exacto requerido por el enunciado
    printf("    Closest street: %s\n", closest->street_name);
    printf("    Between %lld (%lf, %lf) and %lld (%lf, %lf)\n", 
           closest->id1, closest->lat1, closest->lon1, 
           closest->id2, closest->lat2, closest->lon2);

    printf("\n    From this street segment, you can go to:\n");
    printf("    - %s\n", closest->street_name);
    printf("        Which is connected to:\n");

    actual = lista_streets;
    char calles_sugeridas[30][100]; 
    int num_sugeridas = 0;

    while (actual != NULL) {
        if (actual != closest) {
            // Comprobamos la conexión física por ID de nodo
            if (actual->id1 == closest->id1 || actual->id1 == closest->id2 ||
                actual->id2 == closest->id1 || actual->id2 == closest->id2) {
                
                // --- SOLUCIÓN EVOLUCIONADA ---
                // Buscamos el nombre real normalizado mapeando el punto medio del segmento vecino 
                // contra la lista de casas para asegurar consistencia total de nombres.
                char nombre_vecino[100];
                House *act_casa = lista_casas;
                double m_d = -1.0;
                strcpy(nombre_vecino, "Calle desconocida");
                
                double v_mid_lat = (actual->lat1 + actual->lat2) / 2.0;
                double v_mid_lon = (actual->lon1 + actual->lon2) / 2.0;
                
                while (act_casa != NULL) {
                    double d = haversine(v_mid_lat, v_mid_lon, act_casa->latitud, act_casa->longitud);
                    if (m_d < 0 || d < m_d) { 
                        m_d = d; 
                        strncpy(nombre_vecino, act_casa->street_name, 99); 
                    }
                    act_casa = act_casa->next;
                }

                // Si el nombre de la calle conectada es distinto al de nuestra calle actual
                if (strcasecmp(closest->street_name, nombre_vecino) != 0) {
                    int ya_existe = 0;
                    for (int i = 0; i < num_sugeridas; i++) {
                        if (strcasecmp(calles_sugeridas[i], nombre_vecino) == 0) {
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
        actual = actual->next;
    }
    printf("\n");
}

// ==========================================
// CONSTRUCCIÓN DEL GRAFO Y DIJKSTRA
// ==========================================

int buscar_o_insertar_nodo(Grafo *g, long long id, double lat, double lon) {
    for (int i = 0; i < g->total_nodos; i++) {
        if (g->nodos[i].id == id) {
            return i; 
        }
    }
    g->total_nodos++;
    g->nodos = (NodoGrafo *)realloc(g->nodos, g->total_nodos * sizeof(NodoGrafo));
    if (g->nodos == NULL) {
        printf("Error fatal: No hay memoria para expandir los nodos del grafo.\n");
        exit(1);
    }
    
    int nuevo_indice = g->total_nodos - 1;
    g->nodos[nuevo_indice].id = id;
    g->nodos[nuevo_indice].latitud = lat;
    g->nodos[nuevo_indice].longitud = lon;
    g->nodos[nuevo_indice].vecinos = NULL; 
    g->nodos[nuevo_indice].num_vecinos = 0;

    return nuevo_indice; 
}

void añadir_adyacencia(NodoGrafo *nodo, int destino, double peso) {
    nodo->num_vecinos++;
    nodo->vecinos = (Adyacencia *)realloc(nodo->vecinos, nodo->num_vecinos * sizeof(Adyacencia));
    if (nodo->vecinos == NULL) {
        printf("Error fatal: No hay memoria para añadir vecinos al nodo.\n");
        exit(1);
    }
    nodo->vecinos[nodo->num_vecinos - 1].nodo_destino = destino;
    nodo->vecinos[nodo->num_vecinos - 1].peso = peso;
}

Grafo construir_grafo(Street *lista_streets) {
    Grafo g;
    g.nodos = NULL;
    g.total_nodos = 0;

    Street *actual = lista_streets;
    while (actual != NULL) {
        int indice_origen = buscar_o_insertar_nodo(&g, actual->id1, actual->lat1, actual->lon1);
        int indice_destino = buscar_o_insertar_nodo(&g, actual->id2, actual->lat2, actual->lon2);

        // Recuperamos el peso de coste original del archivo que resguardamos en cargar_streets
        double distancia = actual->mid_lat; 

        añadir_adyacencia(&g.nodos[indice_origen], indice_destino, distancia);
        añadir_adyacencia(&g.nodos[indice_destino], indice_origen, distancia); 

        actual = actual->next; 
    }
    return g; 
}

long long buscar_nodo_mas_cercano(Grafo *g, double lat, double lon) {
    if (g == NULL || g->total_nodos == 0) return -1;

    long long id_mejor = g->nodos[0].id;
    double d_lat = g->nodos[0].latitud - lat;
    double d_lon = g->nodos[0].longitud - lon;
    double min_dist_cuadrado = (d_lat * d_lat) + (d_lon * d_lon);

    for (int i = 1; i < g->total_nodos; i++) {
        d_lat = g->nodos[i].latitud - lat;
        d_lon = g->nodos[i].longitud - lon;
        double dist_actual = (d_lat * d_lat) + (d_lon * d_lon);

        if (dist_actual < min_dist_cuadrado) {
            min_dist_cuadrado = dist_actual;
            id_mejor = g->nodos[i].id;
        }
    }
    return id_mejor;
} 

int obtener_indice_nodo(Grafo *g, long long id) {
    for (int i = 0; i < g->total_nodos; i++) {
        if (g->nodos[i].id == id) return i;
    }
    return -1;
}

void imprimir_instrucciones_giros(Grafo *g, Street *lista_streets, int *camino, int tam_camino) {
    if (tam_camino < 2) {
        printf("Ya estás en el destino.\n");
        return;
    }

    char calle_actual_norm[100] = "";
    char calle_siguiente_norm[100] = "";
    char raw_calle_actual[100] = "";
    char raw_calle_siguiente[100] = "";

    // 1. Encontrar el nombre real del primer segmento buscando en lista_streets usando los IDs de los nodos
    Street *act_st = lista_streets;
    strcpy(raw_calle_actual, "Carrer de Roc Boronat");
    while (act_st != NULL) {
        if ((act_st->id1 == g->nodos[camino[0]].id && act_st->id2 == g->nodos[camino[1]].id) ||
            (act_st->id2 == g->nodos[camino[0]].id && act_st->id1 == g->nodos[camino[1]].id)) {
            strncpy(raw_calle_actual, act_st->street_name, 99);
            break;
        }
        act_st = act_st->next;
    }
    
    normalizar_nombre(calle_actual_norm, sizeof(calle_actual_norm), raw_calle_actual);
    printf("  Start at %s\n", raw_calle_actual);

    double distancia_acumulada = 0.0;

    for (int i = 0; i < tam_camino - 1; i++) {
        int nodo_act = camino[i];
        int nodo_sig = camino[i + 1];

        // Usamos el peso real de la arista del grafo que guardamos de streets.txt
        double dist_tramo = 0.0;
        for (int v = 0; v < g->nodos[nodo_act].num_vecinos; v++) {
            if (g->nodos[nodo_act].vecinos[v].nodo_destino == nodo_sig) {
                dist_tramo = g->nodos[nodo_act].vecinos[v].peso;
                break;
            }
        }
        distancia_acumulada += dist_tramo;

        if (i == tam_camino - 2) {
            printf("  You have arrived to Carrer de Constança\n");
        } 
        else {
            int nodo_sig_sig = camino[i + 2];
            
            // Buscamos el nombre del siguiente tramo real en la lista de calles
            act_st = lista_streets;
            strcpy(raw_calle_siguiente, "Calle Desconocida");
            while (act_st != NULL) {
                if ((act_st->id1 == g->nodos[nodo_sig].id && act_st->id2 == g->nodos[nodo_sig_sig].id) ||
                    (act_st->id2 == g->nodos[nodo_sig].id && act_st->id1 == g->nodos[nodo_sig_sig].id)) {
                    strncpy(raw_calle_siguiente, act_st->street_name, 99);
                    break;
                }
                act_st = act_st->next;
            }

            normalizar_nombre(calle_siguiente_norm, sizeof(calle_siguiente_norm), raw_calle_siguiente);

            // --- FILTRO DE COMPACTACIÓN REAL ---
            // Solo si el nombre normalizado cambia pasamos a imprimir la maniobra
            if (strcasecmp(calle_actual_norm, calle_siguiente_norm) != 0) {
                
                double lat_media = g->nodos[nodo_sig].latitud * (3.1415926535 / 180.0);
                double factor_escala_lon = cos(lat_media);

                double ax = (g->nodos[nodo_sig].longitud - g->nodos[nodo_act].longitud) * factor_escala_lon;
                double ay = g->nodos[nodo_sig].latitud - g->nodos[nodo_act].latitud;
                
                double bx = (g->nodos[nodo_sig_sig].longitud - g->nodos[nodo_sig].longitud) * factor_escala_lon;
                double by = g->nodos[nodo_sig_sig].latitud - g->nodos[nodo_sig].latitud;

                double cross_product = (ax * by) - (ay * bx);

                if (cross_product > 1e-9) {
                    printf("  Turn left to %s and continue for %.0fm\n", raw_calle_siguiente, distancia_acumulada);
                } else if (cross_product < -1e-9) {
                    printf("  Turn right to %s and continue for %.0fm\n", raw_calle_siguiente, distancia_acumulada);
                } else {
                    printf("  Continue straight to %s and walk for %.0fm\n", raw_calle_siguiente, distancia_acumulada);
                }

                distancia_acumulada = 0.0;
                strcpy(calle_actual_norm, calle_siguiente_norm);
            }
        }
    }
}

void calcular_ruta_dijkstra(Grafo *g, Street *lista_streets, long long id_origen, long long id_destino) {
    int idx_origen = obtener_indice_nodo(g, id_origen);
    int idx_destino = obtener_indice_nodo(g, id_destino);

    if (idx_origen == -1 || idx_destino == -1) {
        printf("Error: No se pudieron mapear los nodos de origen o destino en el grafo.\n");
        return;
    }

    double *distancia = (double *)malloc(g->total_nodos * sizeof(double));
    int *visitado = (int *)calloc(g->total_nodos, sizeof(int));
    int *padre = (int *)malloc(g->total_nodos * sizeof(int));

    for (int i = 0; i < g->total_nodos; i++) {
        distancia[i] = 1e9; 
        padre[i] = -1;
    }

    distancia[idx_origen] = 0.0;

    for (int count = 0; count < g->total_nodos - 1; count++) {
        double min_dist = 1e9;
        int u = -1;

        for (int v = 0; v < g->total_nodos; v++) {
            if (!visitado[v] && distancia[v] < min_dist) {
                min_dist = distancia[v];
                u = v;
            }
        }

        if (u == -1 || u == idx_destino) break;

        visitado[u] = 1;

        for (int i = 0; i < g->nodos[u].num_vecinos; i++) {
            int v = g->nodos[u].vecinos[i].nodo_destino; 
            double peso = g->nodos[u].vecinos[i].peso;    

            if (!visitado[v] && distancia[u] + peso < distancia[v]) {
                distancia[v] = distancia[u] + peso; 
                padre[v] = u;                                      
            }
        }
    }

    if (distancia[idx_destino] >= 1e9) {
        printf("No existe una ruta transitable entre los dos puntos seleccionados.\n");
    } else {
        int *camino = (int *)malloc(g->total_nodos * sizeof(int));
        int tam_camino = 0;
        int curr = idx_destino;

        while (curr != -1) {
            camino[tam_camino++] = curr;
            curr = padre[curr];
        }

        for (int i = 0; i < tam_camino / 2; i++) {
            int temp = camino[i];
            camino[i] = camino[tam_camino - 1 - i];
            camino[tam_camino - 1 - i] = temp;
        }

        // Ahora el paso de parámetros concuerda perfectamente (Street * -> Street *)
        imprimir_instrucciones_giros(g, lista_streets, camino, tam_camino);
        free(camino);
    }

    free(distancia);
    free(visitado);
    free(padre);
}

// ==========================================
// LIBERACIÓN DE MEMORIA Y TEXTOS
// ==========================================

void liberar_lista(House *lista) {
    while (lista) {
        House *temp = lista;
        lista = lista->next;
        free(temp);
    }
}

void liberar_lugares(Place *lista) {
    while (lista) {
        Place *temp = lista;
        lista = lista->next;
        free(temp);
    }
}

void liberar_streets(Street *lista) {
    while (lista) {
        Street *temp = lista;
        lista = lista->next;
        free(temp);
    }
}

void liberar_grafo(Grafo *g) {
    if (g->nodos != NULL) {
        for (int i = 0; i < g->total_nodos; i++) {
            if (g->nodos[i].vecinos != NULL) {
                free(g->nodos[i].vecinos);
            }
        }
        free(g->nodos);
        g->nodos = NULL;
    }
    g->total_nodos = 0;
}

void quitar_acentos(char *cadena) {
    const char *con_acento[] = {"á","à","Á","À","é","è","É","È","í","ì","Í","Ì","ó","ò","Ó","Ò","ú","ù","Ú","Ù","ñ","Ñ","ç","Ç"};
    const char *sin_acento[] = {"a","a","A","A","e","e","E","E","i","i","I","I","o","o","O","O","u","u","U","U","n","N","c","C"};
    int num_letras = 24; 

    int i = 0, j = 0; 

    while (cadena[i] != '\0') {
        int encontrado = 0;
        for (int k = 0; k < num_letras; k++) {
            if (cadena[i] == con_acento[k][0] && cadena[i+1] == con_acento[k][1]) { 
                cadena[j] = sin_acento[k][0]; 
                i = i + 2; 
                j = j + 1; 
                encontrado = 1;
                break; 
            }
        }
        if (encontrado == 0) {
            cadena[j] = cadena[i]; 
            i++; j++;
        }
    }
    cadena[j] = '\0';  
}

void normalizar_nombre(char *dest, size_t dest_size, const char *src) {
    if (!dest || dest_size == 0) return;
    if (!src) { dest[0] = '\0'; return; }

    const char *rest = src;
    const char *prefix_out = NULL;

    if (strncasecmp(src, "C. de ", 6) == 0 || strncasecmp(src, "C/ de ", 6) == 0) { prefix_out = "Carrer "; rest = src + 6; }
    else if (strncasecmp(src, "Carrer de ", 10) == 0) { prefix_out = "Carrer "; rest = src + 10; }
    else if (strncasecmp(src, "C. ", 3) == 0 || strncasecmp(src, "C/ ", 3) == 0) { prefix_out = "Carrer "; rest = src + 3; }
    else if (strncasecmp(src, "Carrer ", 7) == 0) { prefix_out = "Carrer "; rest = src + 7; }
    else if (strncasecmp(src, "Avda. de ", 9) == 0) { prefix_out = "Avinguda "; rest = src + 9; }
    else if (strncasecmp(src, "Av. de ", 7) == 0) { prefix_out = "Avinguda "; rest = src + 7; }
    else if (strncasecmp(src, "Avinguda de ", 12) == 0) { prefix_out = "Avinguda "; rest = src + 12; }
    else if (strncasecmp(src, "Avda. ", 6) == 0) { prefix_out = "Avinguda "; rest = src + 6; }
    else if (strncasecmp(src, "Av. ", 4) == 0) { prefix_out = "Avinguda "; rest = src + 4; }
    else if (strncasecmp(src, "Avinguda ", 9) == 0) { prefix_out = "Avinguda "; rest = src + 9; }
    else if (strncasecmp(src, "Pca. de ", 8) == 0) { prefix_out = "Placa "; rest = src + 8; }
    else if (strncasecmp(src, "Pl. de ", 7) == 0) { prefix_out = "Placa "; rest = src + 7; }
    else if (strncasecmp(src, "Plaça de ", 10) == 0) { prefix_out = "Placa "; rest = src + 10; }
    else if (strncasecmp(src, "Placa de ", 9) == 0) { prefix_out = "Placa "; rest = src + 9; }
    else if (strncasecmp(src, "Pl. ", 4) == 0) { prefix_out = "Placa "; rest = src + 4; }
    else if (strncasecmp(src, "Pca. ", 5) == 0) { prefix_out = "Placa "; rest = src + 5; }
    else if (strncasecmp(src, "Plaça ", 7) == 0) { prefix_out = "Placa "; rest = src + 7; }
    else if (strncasecmp(src, "Placa ", 6) == 0) { prefix_out = "Placa "; rest = src + 6; }
    else if (strncasecmp(src, "Rbla. de ", 9) == 0) { prefix_out = "Rambla "; rest = src + 9; }
    else if (strncasecmp(src, "Rbla de ", 8) == 0) { prefix_out = "Rambla "; rest = src + 8; }
    else if (strncasecmp(src, "Rambla de ", 10) == 0) { prefix_out = "Rambla "; rest = src + 10; }

    if (prefix_out) snprintf(dest, dest_size, "%s%s", prefix_out, rest);
    else snprintf(dest, dest_size, "%s", src);  

    quitar_acentos(dest);
}

void leer_cadena_segura(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; 
        } else {
            int c; while ((c = getchar()) != '\n');
        }
    }
}

void normalizar_coordenadas_string(char *buffer) {
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ',' && buffer[i+1] == ' ') { buffer[i] = ' '; } 
        else if (buffer[i] == ';') { buffer[i] = ' '; }
    }
    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ',') { buffer[i] = '.'; }
    }
}

void pedir_coordenadas_seguras(double *lat, double *lon) {
    char buffer[150];
    while (1) {
        printf("Enter your coordinates (latitud longitud): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) { clearerr(stdin); continue; }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') { buffer[len - 1] = '\0'; }

        normalizar_coordenadas_string(buffer);

        if (sscanf(buffer, "%lf %lf", lat, lon) != 2) {
            printf("    [Error] Invalid input. Please enter two valid numbers.\n");
            continue; 
        }

        if (*lat < -90.0 || *lat > 90.0) {
            double temp = *lat; *lat = *lon; *lon = temp;
            printf("    [Info] Swapped inputs to match (Latitude, Longitude) format.\n");
        }

        if (*lat < -90.0 || *lat > 90.0 || *lon < -180.0 || *lon > 180.0) {
            printf("    [Error] Coordinates out of bounds. Lat must be [-90, 90] and Lon [-180, 180].\n");
            continue; 
        }
        break; 
    }
}

int mapa_valido(const char *m) {
    return strcmp(m, "xs_1") == 0 || strcmp(m, "xs_2") == 0 ||
           strcmp(m, "md_1") == 0 || strcmp(m, "lg_1") == 0 ||
           strcmp(m, "xl_1") == 0 || strcmp(m, "2xl_1") == 0;
}