#ifndef SAMPLE_LIB_H
#define SAMPLE_LIB_H
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estructura para casas
typedef struct House {
    char street_name[100]; 
    int house_number; 
    double latitud; 
    double longitud; 
    struct House *next; 
} House;

// Estructura para lugares
typedef struct Place {
    char name[150];
    double latitud;
    double longitud;
    struct Place *next;
} Place;

// Estructura pera las calles
typedef struct Street {
    long long id1; 
    long long id2; 
    double lat1;
    double lon1;
    double lat2;
    double lon2;
    double mid_lat;
    double mid_lon;
    struct Street *next; 
} Street;

// Estructuras para los grafos: 

//estructura para representar una conexión directa a un nodo vecino
typedef struct {
    int nodo_destino;      //indice en el array del nodo vecino
    double peso;           // Distancia en metros (peso del vertice)
} Adyacencia;

// Estructura para cada intersección (Nodo del Grafo)
typedef struct {
    long long id;          // El id del mapa (id1 o id2 de los ficheros)
    double latitud;
    double longitud;
    Adyacencia *vecinos;   // Array dinámico de las conexiones
    int num_vecinos;       // Cuantos vecinos tiene este nodo
} NodoGrafo;

//Estructura global de todo el Grafo
typedef struct {
    NodoGrafo *nodos;      // Array dinámico de todos los nodos únicos
    int total_nodos;       // Numero total de nodos en el array
} Grafo;

// Funciones para direcciones
int distancia_levenshtein(char *s1, char *s2);
House* cargar_mapa(char *path, int *total);
House* add_casa(House *cabeza, char *street, int num, double lat, double lon);
void buscar_direccion(House *lista, double *res_lat, double *res_lon);

// Funciones para lugares
Place* cargar_lugares(char *path, int *total);
Place* add_lugar(Place *cabeza, char *name, double lat, double lon);
void buscar_lugar(Place *lista, double *res_lat, double *res_lon);
void liberar_lugares(Place *lista);
void liberar_lista(House *lista);

// Funciones para calles
double haversine(double lat1, double lon1, double lat2, double lon2);
Street* add_street(Street *cabeza, long long id1, double lat1, double lon1, long long id2, double lat2, double lon2);
Street* cargar_streets(char *path, int *total);
void buscar_coordenada(Street *lista_streets, House *lista_casas, double user_lat, double user_lon);
void liberar_streets(Street *lista);

//funciones lab 4 para evitar errores al entrar coordenadas
void normalizar_coordenadas_string(char *buffer);
void pedir_coordenadas_seguras(double *lat, double *lon);

// otras funciones 
void leer_cadena_segura(char *buffer, int size);
int mapa_valido(const char *m);
void test_streets_list();

//funciones para el grafo 
Grafo construir_grafo(Street *lista_streets);
int buscar_o_insertar_nodo(Grafo *g, long long id, double lat, double lon);
void añadir_adyacencia(NodoGrafo *nodo, int destino, double peso);
void liberar_grafo(Grafo *g);
long long buscar_nodo_mas_cercano(Grafo *g, double lat, double lon); //devuelve el id 

// Funciones para calcular las rutas con Dijkstra
int obtener_indice_nodo(Grafo *g, long long id);
void calcular_ruta_dijkstra(Grafo *g, House *lista_casas, long long origen, long long destino);

#endif