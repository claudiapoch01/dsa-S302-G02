#include "sample_lib.h"
#include <math.h>
#include <float.h>


// Funciones auxiliares
int minimo(int a, int b, int c) { // Devuelve el mínimo de 3 números
    int m = a;
    if (b < m) m = b;
    if (c < m) m = c;
    return m;
}

int distancia_levenshtein(char *s1, char *s2) { // Calcula la distancia de Levenshtein
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


// Funciones para la gestión de casas

House* add_casa(House *cabeza, char *street, int num, double lat, double lon) { // Añade una casa a la lista enlazada de casas
    House *nueva = (House*)malloc(sizeof(House));
    if (nueva == NULL) return cabeza;

    snprintf(nueva->street_name, sizeof(nueva->street_name), "%s", street);
    nueva->house_number = num;
    nueva->latitud = lat;
    nueva->longitud = lon;
    nueva->next = cabeza;
    return nueva;
}

House* cargar_mapa(char *path, int *total) { // Carga las casas desde el archivo
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

void buscar_direccion(House *lista, double *res_lat, double *res_lon) { // Busca una dirección en la lista de casas y devuelve coordenadas
    char raw_name[100], street_search[100];
    int num_search;

    printf("Enter street name: ");
    leer_cadena_segura(raw_name, 100); 
    normalizar_nombre(street_search, sizeof(street_search), raw_name); 

    printf("Enter street number: ");
    while (scanf("%d", &num_search) != 1) { // Si el valor no es un número, pide que se introduzca de nuevo
        printf("Invalid input. Please enter a numeric value: ");
        int c; while ((c = getchar()) != '\n'); 
    }
    while (getchar() != '\n'); 

    House *actual = lista;
    House *mejor_calle_nodo = NULL;
    House *casa_elegida = NULL; 
    int min_dist = 100;
    int calle_exacta_encontrada = 0;

    while (actual != NULL) { // Buscamos la casa que coincida con el nombre y num
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

    if (!casa_elegida && calle_exacta_encontrada) { // Si no encuentra el numero, lo vuelve a pedir mostrando los posibles números de esa calle
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
    
    if (casa_elegida != NULL) { // Si la encuentra, devuelve las coordenadas
        printf("\n    Found at (%lf, %lf)\n", casa_elegida->latitud, casa_elegida->longitud);
        *res_lat = casa_elegida->latitud;
        *res_lon = casa_elegida->longitud;
    } else {
        *res_lat = 0.0;
        *res_lon = 0.0;
    }
}

// Funciones para la gestión de lugares

Place* add_lugar(Place *cabeza, char *name, double lat, double lon) { // Añade un lugar a la lista enlazada de lugares
    Place *nuevo = (Place*)malloc(sizeof(Place));
    if (nuevo == NULL) return cabeza;

    snprintf(nuevo->name, sizeof(nuevo->name), "%s", name);
    nuevo->latitud = lat;
    nuevo->longitud = lon;
    nuevo->next = cabeza;
    return nuevo;
}

Place* cargar_lugares(char *path, int *total) { // Carga los lugares desde el archivo
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

void buscar_lugar(Place *lista, double *res_lat, double *res_lon) { // Busca un lugar por su nombre y devuelve coordenadas
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

    while (actual != NULL) { // Buscamos el lugar que coincida con el nombre
        char place_sin_acentos[150];
        strcpy(place_sin_acentos, actual->name);
        quitar_acentos(place_sin_acentos); 

        if (strcasecmp(place_sin_acentos, search_name_normalizado) == 0) { 
            if (num_coincidencias < 50) {
                coincidencias[num_coincidencias] = actual;
                num_coincidencias++;
            }
        }

        int d = distancia_levenshtein(search_name, actual->name); // Calculamos el lugar más parecido si el usuario se equivoca
        if (d < min_dist) {
            min_dist = d;
            mejor_lugar = actual;
        }
        actual = actual->next;
    }

    if (num_coincidencias == 1) { // Si coincide, lo printamos directamente
        lugar_elegido = coincidencias[0];
        printf("\n    Found at (%lf, %lf)\n", lugar_elegido->latitud, lugar_elegido->longitud);
    } 
    else if (num_coincidencias > 1) { // Si hay varias coincidencias, se las mostramos al usuario para que elija (ej. bon area)
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
        while (getchar() != '\n');
        
        lugar_elegido = coincidencias[seleccion - 1];
        printf("\n    Selected: %s at (%lf, %lf)\n", lugar_elegido->name, lugar_elegido->latitud, lugar_elegido->longitud);
    } 
    else if (mejor_lugar != NULL && min_dist < 10) { // Si no hay coincidencias, elige un lugar parecido
        lugar_elegido = mejor_lugar; 
        printf("Place not found. Did you mean: %s?\n", lugar_elegido->name);
        printf("\n    Found at (%lf, %lf)\n", lugar_elegido->latitud, lugar_elegido->longitud);
    } 
    else {
        printf("Place not found.\n");
    }

    // Devolvemos las coordenadas o 0,0 si no se ha encontrado el lugar
    if (lugar_elegido != NULL) {
        *res_lat = lugar_elegido->latitud;
        *res_lon = lugar_elegido->longitud;
    } else {
        *res_lat = 0.0;
        *res_lon = 0.0;
    }
}

// Gestión de coordenadas y calles

#define EARTH_RADIUS 6371000.0 
#define TO_RAD (3.14159265358979323846 / 180.0) 

double haversine(double lat1, double lon1, double lat2, double lon2) { // Calcula distancia entre dos coordenadas usando harversine
    double dLat = (lat2 - lat1) * TO_RAD;
    double dLon = (lon2 - lon1) * TO_RAD;
    double lat1_rad = lat1 * TO_RAD;
    double lat2_rad = lat2 * TO_RAD;

    double a = sin(dLat / 2) * sin(dLat / 2) +
               sin(dLon / 2) * sin(dLon / 2) * cos(lat1_rad) * cos(lat2_rad);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return EARTH_RADIUS * c;
}

// Añade una calle a la lista de calles
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

// Carga las calles desde el archivo
Street* cargar_streets_con_hash(char *path, int *total, StreetHashMap *map) {
    FILE *f = fopen(path, "r");
    if (f == NULL) return NULL;

    Street *cabeza = NULL;
    char linea[256];
    *total = 0;

    memset(map, 0, sizeof(StreetHashMap));

    while (fgets(linea, sizeof(linea), f)) {
        long long id1, id2;
        double lat1, lon1, lat2, lon2;
        double peso_fichero = 0.0;
        char nombre_calle[100];
        
        nombre_calle[0] = '\0'; 
        
        int leidos = sscanf(linea, "%lld,%lf,%lf,%lld,%lf,%lf,%lf,%[^\n]", 
                            &id1, &lat1, &lon1, &id2, &lat2, &lon2, &peso_fichero, nombre_calle);
        
        if (leidos == 8 && strlen(nombre_calle) > 0) {
            Street *nueva = (Street*)malloc(sizeof(Street));
            if (nueva != NULL) {
                nueva->id1 = id1; nueva->lat1 = lat1; nueva->lon1 = lon1;
                nueva->id2 = id2; nueva->lat2 = lat2; nueva->lon2 = lon2;
                nueva->mid_lat = peso_fichero; 
                nueva->mid_lon = 0.0; 
                snprintf(nueva->street_name, sizeof(nueva->street_name), "%s", nombre_calle);
                nueva->next = cabeza;
                cabeza = nueva;
                (*total)++;

                // Aquí se instancia la hash en O(1)
                hash_insertar(map, id1, id2, nombre_calle);
            }
        }
    }
    fclose(f);
    return cabeza;
}

// Versión antigua de buscar_coordenadas, del lab 4 (se le ha modificado el nombre para que no haya conflicto)
void buscar_coordenada_antigua(Street *lista_streets, House *lista_casas, double user_lat, double user_lon) {
    if (lista_streets == NULL) return;

    Street *actual = lista_streets;
    Street *closest = NULL;
    double min_dist = -1.0;

    // Encuentra la calle más cercana
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

    // Printa la calle más cercana y sus adyacentes
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
            if (actual->id1 == closest->id1 || actual->id1 == closest->id2 ||
                actual->id2 == closest->id1 || actual->id2 == closest->id2) {

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

                // Si el nombre de la calle conectada es distinto al de nuestra calle actual, lo añadimos como sugerencia
                if (strcasecmp(closest->street_name, nombre_vecino) != 0) {
                    int ya_existe = 0;
                    for (int i = 0; i < num_sugeridas; i++) {
                        if (strcasecmp(calles_sugeridas[i], nombre_vecino) == 0) {
                            ya_existe = 1;
                            break;
                        }
                    }
                    // Si no existe en las sugerencias, lo añadimos y printamos
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

// Grafos y dijkstra

int buscar_o_insertar_nodo(Grafo *g, long long id,double lat, double lon) { // Funcion que añade o busca un nodo por id
    for (int i = 0; i < g->total_nodos; i++) { // Si existe el nodo, devuelve su indice
        if (g->nodos[i].id == id) {
            return i; 
        }
    }
    // Sino, lo añade al grafo
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

void añadir_adyacencia(NodoGrafo *nodo, int destino, double peso) { // añade una adyacencia a un nodo del grafo
    nodo->num_vecinos++;
    nodo->vecinos = (Adyacencia *)realloc(nodo->vecinos, nodo->num_vecinos * sizeof(Adyacencia));
    if (nodo->vecinos == NULL) {
        printf("Error fatal: No hay memoria para añadir vecinos al nodo.\n");
        exit(1);
    }
    nodo->vecinos[nodo->num_vecinos - 1].nodo_destino = destino;
    nodo->vecinos[nodo->num_vecinos - 1].peso = peso;
}

Grafo construir_grafo(Street *lista_streets) { // Construye el grafo a partir de la lista de calles
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

long long buscar_nodo_mas_cercano(Grafo *g, double lat, double lon) { // Busca el nodo del grafo más cercano a unas coordenadas
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

int obtener_indice_nodo(Grafo *g, long long id) { // Encuentra el índice de un nodo en el grafo a partir de su id
    for (int i = 0; i < g->total_nodos; i++) {
        if (g->nodos[i].id == id) return i;
    }
    return -1;
}

double toRadians(double degree) { // Pasa d grados a radiantes
    return degree * (3.14159265358979323846 / 180.0);
}

void latlon_to_xy(double lat_ref, double lon_ref,
                  double lat, double lon,
                  double *x, double *y) {
    double lat_ref_rad = toRadians(lat_ref);
    double dlat = toRadians(lat - lat_ref);
    double dlon = toRadians(lon - lon_ref);
    *x = EARTH_RADIUS_KM * dlon * cos(lat_ref_rad);
    *y = EARTH_RADIUS_KM * dlat;
}

void imprimir_instrucciones_giros_hash(Grafo *g, StreetHashMap *map, int *camino, int tam_camino) { // Imprime la ruta con las instrucciones
    if (tam_camino < 2) {
        printf("Ya estás en el destino.\n");
        return;
    }

    char calle_actual_norm[100] = "";
    char calle_siguiente_norm[100] = "";
    char raw_calle_actual[100] = "";
    char raw_calle_siguiente[100] = "";

    // Coge el nombre de la calle actual
    const char *nombre_hash = hash_buscar(map, g->nodos[camino[0]].id, g->nodos[camino[1]].id);
    if (nombre_hash != NULL) {
        strncpy(raw_calle_actual, nombre_hash, 99);
    } else {
        strcpy(raw_calle_actual, "Calle de Inicio");
    }
    
    normalizar_nombre(calle_actual_norm, sizeof(calle_actual_norm), raw_calle_actual);
    printf("  Start at %s\n", raw_calle_actual);

    double distancia_acumulada = 0.0;

    // Por cada tramo, calcula la distancia, el nombre de la proxima calle y el giro. Luego lo printa
    for (int i = 0; i < tam_camino - 1; i++) {
        int nodo_act = camino[i];
        int nodo_sig = camino[i + 1];

        double dist_tramo = 0.0;
        for (int v = 0; v < g->nodos[nodo_act].num_vecinos; v++) { // Buscamos los vecinos del nodo actual
            if (g->nodos[nodo_act].vecinos[v].nodo_destino == nodo_sig) { 
                dist_tramo = g->nodos[nodo_act].vecinos[v].peso;
                break;
            }
        }
        distancia_acumulada += dist_tramo; // acumulamos la distancia

        // Si es el último tramo, printa que ya llegas al destino
        if (i == tam_camino - 2) {
            printf("  Continue straight for %.0fm and you will arrive at your destination.\n", distancia_acumulada);
        }
        else { // Si no, calculamos el nombre de la siguiente calle y el giro
            int nodo_sig_sig = camino[i + 2];
            
            // Consigue el nombre de la calle siguiente
            const char *nombre_sig_hash = hash_buscar(map, g->nodos[nodo_sig].id, g->nodos[nodo_sig_sig].id);
            if (nombre_sig_hash != NULL) {
                strncpy(raw_calle_siguiente, nombre_sig_hash, 99);
            } else {
                strcpy(raw_calle_siguiente, "Calle Desconocida");
            }

            normalizar_nombre(calle_siguiente_norm, sizeof(calle_siguiente_norm), raw_calle_siguiente);

            // Si el nombre de la calle siguiente es distinto al de la calle actual, calculamos el giro
            if (strcasecmp(calle_actual_norm, calle_siguiente_norm) != 0) {
                double Ax, Ay, Bx, By, Cx, Cy;
                double lat_ref = g->nodos[nodo_act].latitud;
                double lon_ref = g->nodos[nodo_act].longitud;

                latlon_to_xy(lat_ref, lon_ref, g->nodos[nodo_act].latitud, g->nodos[nodo_act].longitud, &Ax, &Ay);
                latlon_to_xy(lat_ref, lon_ref, g->nodos[nodo_sig].latitud, g->nodos[nodo_sig].longitud, &Bx, &By);
                latlon_to_xy(lat_ref, lon_ref, g->nodos[nodo_sig_sig].latitud, g->nodos[nodo_sig_sig].longitud, &Cx, &Cy);

                double cross_product = (Bx - Ax) * (Cy - By) - (By - Ay) * (Cx - Bx);

                if (cross_product > 1e-6) {
                    printf("  Turn left to %s and continue for %.0fm\n", raw_calle_siguiente, distancia_acumulada);
                } else if (cross_product < -1e-6) {
                    printf("  Turn right to %s and continue for %.0fm\n", raw_calle_siguiente, distancia_acumulada);
                } else {
                    printf("  Continue straight to %s and walk for %.0fm\n", raw_calle_siguiente, distancia_acumulada);
                }

                distancia_acumulada = 0.0; // Reseteamos contador para la nueva calle
                strcpy(raw_calle_actual, raw_calle_siguiente);
                strcpy(calle_actual_norm, calle_siguiente_norm);
            }
        }
    }
}

// Calcula la ruta con dijkstra (actualmente usamos bfs)
void calcular_ruta_dijkstra(Grafo *g, long long id_origen, long long id_destino) {
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
        imprimir_instrucciones_giros_hash(g, NULL, camino, tam_camino);
        free(camino);
    }

    free(distancia);
    free(visitado);
    free(padre);
}



// Funciones que liberan la memoria

void liberar_lista(House *lista) { // Libera la memoria de la lista de casas
    while (lista) {
        House *temp = lista;
        lista = lista->next;
        free(temp);
    }
}

void liberar_lugares(Place *lista) { // Libera la memoria de la lista de lugares
    while (lista) {
        Place *temp = lista;
        lista = lista->next;
        free(temp);
    }
}

void liberar_streets(Street *lista) { // Libera la memoria de la lista de calles
    while (lista) {
        Street *temp = lista;
        lista = lista->next;
        free(temp);
    }
}

void liberar_grafo(Grafo *g) { // Libera la memoria del grafo, incluyendo los nodos y adyacencias
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

void quitar_acentos(char *cadena) { // Función que quita los acentos de un string (por ej. una calle)
    const char *con_acento[] = {"á","à","Á","À","é","è","É","È","í","ì","Í","Ì","ó","ò","Ó","Ò","ú","ù","Ú","Ù","ñ","Ñ","ç","Ç"};
    const char *sin_acento[] = {"a","a","A","A","e","e","E","E","i","i","I","I","o","o","O","O","u","u","U","U","n","N","c","C"};
    int num_letras = 24; 

    int i = 0, j = 0; 

    while (cadena[i] != '\0') {
        int encontrado = 0;
        for (int k = 0; k < num_letras; k++) { // Compara cada letra con acento con la letra actual de la cadena
            if (cadena[i] == con_acento[k][0] && cadena[i+1] == con_acento[k][1]) { 
                cadena[j] = sin_acento[k][0]; 
                i = i + 2; // Sumamos dos porque las letras con acento ocupan dos bytes
                j = j + 1; // Sumamos uno porque la letra sin acento ocupa un byte
                encontrado = 1; // Marca el booleano como true
                break; 
            }
        }
        if (encontrado == 0) { // Si no hay acentos, copiamos el caracter tal cual
            cadena[j] = cadena[i]; 
            i++; j++;
        }
    }
    cadena[j] = '\0';  
}

void normalizar_nombre(char *dest, size_t dest_size, const char *src) { // Normaliza un string unificando abreviaturas
    if (!dest || dest_size == 0) return;
    if (!src) { dest[0] = '\0'; return; }

    const char *rest = src;
    const char *prefix_out = NULL;

    // Comprueba diferentes abreviaturas y las unifica, guardando el resto del nombre en "rest"
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

    if (prefix_out) snprintf(dest, dest_size, "%s%s", prefix_out, rest); // Si hay prefijo, lo añadimos al principio del nombre
    else snprintf(dest, dest_size, "%s", src);  // Si no, copiamos el nombre tal cual

    quitar_acentos(dest); // Quita los acentos del nombre
}

void leer_cadena_segura(char *buffer, int size) { // Lee una cadena de forma segura
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; 
        } else {
            int c; while ((c = getchar()) != '\n');
        }
    }
}

void normalizar_coordenadas_string(char *buffer) { // Normaliza el string de coordenadas

    for (int i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ',' && buffer[i+1] == ' ') { buffer[i] = ' '; }  // Si hay una coma seguida de espacio, lo cambiamos por espacio
        else if (buffer[i] == ';') { buffer[i] = ' '; } // Si hay un punto y coma, lo cambiamos por espacio
    }
    for (int i = 0; buffer[i] != '\0'; i++) { // Si hay una coma, lo cambiamos por punto 
        if (buffer[i] == ',') { buffer[i] = '.'; } // Si hay un punto, lo dejamos igual
    }
}

void pedir_coordenadas_seguras(double *lat, double *lon) { // Pide las coordenadas de forma segura
    char buffer[150];
    while (1) {
        printf("Enter your coordinates (latitud longitud): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) { clearerr(stdin); continue; }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') { buffer[len - 1] = '\0'; }

        normalizar_coordenadas_string(buffer);

        if (sscanf(buffer, "%lf %lf", lat, lon) != 2) { // Si no se pudieron leer dos números, el formato es inválido
            printf("    [Error] Invalid input. Please enter two valid numbers.\n");
            continue; 
        }

        // Si la latitud no existe, pero la longitud sí, lo giramos por si el usuario se ha equivocado de orden
        if (*lat < -90.0 || *lat > 90.0) {
            double temp = *lat; *lat = *lon; *lon = temp;
            printf("    [Info] Swapped inputs to match (Latitude, Longitude) format.\n");
        }
        // Ahora comprobamos si las coordenadas están dentro de los límites válidos
        if (*lat < -90.0 || *lat > 90.0 || *lon < -180.0 || *lon > 180.0) {
            printf("    [Error] Coordinates out of bounds. Lat must be [-90, 90] and Lon [-180, 180].\n");
            continue; 
        }
        break; 
    }
}

int mapa_valido(const char *m) { // Comprueba si el mapa introducido coincide con uno existente
    return strcmp(m, "xs_1") == 0 || strcmp(m, "xs_2") == 0 ||
           strcmp(m, "md_1") == 0 || strcmp(m, "lg_1") == 0 ||
           strcmp(m, "xl_1") == 0 || strcmp(m, "2xl_1") == 0;
}

void init_queue(Queue *q) { // Inicializa la cola
    q->front = NULL;
    q->rear = NULL;
}

int queue_is_empty(Queue *q) { // Comprueba si la cola está vacía
    return (q->front == NULL);
}

void enqueue(Queue *q, int *path_array, int len) { // Añade un camino a la cola
    QueueNode *nuevo = (QueueNode *)malloc(sizeof(QueueNode));
    nuevo->path = (int *)malloc(len * sizeof(int));
    memcpy(nuevo->path, path_array, len * sizeof(int));
    nuevo->path_len = len;
    nuevo->next = NULL;
    
    if (queue_is_empty(q)) {
        q->front = nuevo; 
    } else {
        q->rear->next = nuevo;
    }
    q->rear = nuevo;
}

void dequeue(Queue *q, int **path_out, int *len_out) { // Quita el primer camino de la cola.
    if (queue_is_empty(q)) return;
    
    QueueNode *temp = q->front;
    *path_out = temp->path;
    *len_out = temp->path_len;
    
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
}

// Calculos de ruta con bfs

void calcular_ruta_bfs(Grafo *g, StreetHashMap *map, long long id_origen, long long id_destino) {
    int idx_origen = obtener_indice_nodo(g, id_origen);
    int idx_destino = obtener_indice_nodo(g, id_destino);

    if (idx_origen == -1 || idx_destino == -1) { // Si no se encuentran los nodos, no se puede calcular la ruta
        printf("Error: Nodos no encontrados en el grafo.\n");
        return;
    }

    int *visitado = (int *)calloc(g->total_nodos, sizeof(int)); // Crea un array para guardar los nodos

    Queue Q;
    init_queue(&Q);
    int initial_path[1] = {idx_origen};
    enqueue(&Q, initial_path, 1);

    int *mejor_camino = NULL;
    int mejor_tam = 0;

    while (!queue_is_empty(&Q)) { // Mientras la cola no esté vacía, seguimos buscando caminos
        int *path;
        int len;
        
        dequeue(&Q, &path, &len);

        int current_node = path[len - 1];

        if (current_node == idx_destino) {
            mejor_camino = path; // Guardamos el camino encontrado
            mejor_tam = len;
            break;
        }

        if (!visitado[current_node]) { // Si el nodo actual no ha sido visitado, lo marcamos como visitado y seguimos buscando
            visitado[current_node] = 1;

            // Itera por cada vecino
            for (int i = 0; i < g->nodos[current_node].num_vecinos; i++) { 
                int connected_node = g->nodos[current_node].vecinos[i].nodo_destino;

                if (!visitado[connected_node]) { // Si no ha sido visitado, añadimos un nuevo camino a la cola
                    int *new_path = (int *)malloc((len + 1) * sizeof(int));
                    memcpy(new_path, path, len * sizeof(int));
                    new_path[len] = connected_node;
                    
                    enqueue(&Q, new_path, len + 1);
                    free(new_path);
                }
            }
        }
        
        free(path); 
    }

    if (mejor_camino != NULL) { // Si se ha encontrado un camino, lo imprimimos
        printf("\n--- ROUTE ---\n");
        imprimir_instrucciones_giros_hash(g, map, mejor_camino, mejor_tam);
        free(mejor_camino);
    } else { // Si no se ha encontrado ningún camino, indicamos que no existe una ruta transitable
        printf("No existe una ruta transitable entre los dos puntos seleccionados.\n");
    }

    while (!queue_is_empty(&Q)) { // Liberamos la memoria
        int *path_res;
        int len_res;
        dequeue(&Q, &path_res, &len_res);
        free(path_res);
    }

    free(visitado);
}

unsigned int calcular_hash(long long id1, long long id2) { // Funcion que calcula el hash de dos ids de nodos
    // Ordenamos los ids 
    long long menor = (id1 < id2) ? id1 : id2;
    long long mayor = (id1 < id2) ? id2 : id1;
    
    unsigned long long h = 1125899906842597ULL; // Elegimos un valor primo grande para usar como base
    h = h * 31 + menor;
    h = h * 31 + mayor;
    
    return (unsigned int)(h % HASH_SIZE);
}

// Insertar una calle en la tabla hash
void hash_insertar(StreetHashMap *map, long long id1, long long id2, const char *name) {
    unsigned int index = calcular_hash(id1, id2);
    
    HashNode *nuevo = (HashNode *)malloc(sizeof(HashNode));
    nuevo->id1 = id1;
    nuevo->id2 = id2;
    strncpy(nuevo->street_name, name, 99);
    nuevo->street_name[99] = '\0';
    
    nuevo->next = map->buckets[index];
    map->buckets[index] = nuevo;
}

// Buscar el nombre de una calle en la tabla hash a partir de dos ids 
const char* hash_buscar(StreetHashMap *map, long long id1, long long id2) {
    unsigned int index = calcular_hash(id1, id2);
    HashNode *actual = map->buckets[index];
    
    long long menor = (id1 < id2) ? id1 : id2;
    long long mayor = (id1 < id2) ? id2 : id1;
    
    while (actual != NULL) {
        long long act_menor = (actual->id1 < actual->id2) ? actual->id1 : actual->id2;
        long long act_mayor = (actual->id1 < actual->id2) ? actual->id2 : actual->id1;
        
        if (act_menor == menor && act_mayor == mayor) {
            return actual->street_name; // Si encontramos la calle, devolvemos su nombre
        }
        actual = actual->next;
    }
    return NULL; // Si no se encuentra la calle, devolvemos NULL
}

void hash_liberar(StreetHashMap *map) { // Libera la memoria de la tabla hash
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *actual = map->buckets[i];
        while (actual != NULL) {
            HashNode *temp = actual;
            actual = actual->next;
            free(temp);
        }
        map->buckets[i] = NULL;
    }
}