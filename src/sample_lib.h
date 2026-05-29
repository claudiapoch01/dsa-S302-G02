#ifndef SAMPLE_LIB_H
#define SAMPLE_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct House {
    char street_name[100]; 
    int house_number; 
    double latitud; 
    double longitud; 
    struct House *next; 
} House;

typedef struct Place {
    char name[150];
    double latitud;
    double longitud;
    struct Place *next;
} Place;

typedef struct Street {
    long long id1; 
    long long id2; 
    double lat1;
    double lon1;
    double lat2;
    double lon2;
    double mid_lat;
    double mid_lon;
    char street_name[100];
    struct Street *next; 
} Street;

typedef struct {
    int nodo_destino;      
    double peso;           
} Adyacencia;

typedef struct {
    long long id;          
    double latitud;
    double longitud;
    Adyacencia *vecinos;   
    int num_vecinos;       
} NodoGrafo;

typedef struct {
    NodoGrafo *nodos;      
    int total_nodos;       
} Grafo;

int distancia_levenshtein(char *s1, char *s2);
House* cargar_mapa(char *path, int *total);
House* add_casa(House *cabeza, char *street, int num, double lat, double lon);
void buscar_direccion(House *lista, double *res_lat, double *res_lon);

Place* cargar_lugares(char *path, int *total);
Place* add_lugar(Place *cabeza, char *name, double lat, double lon);
void buscar_lugar(Place *lista, double *res_lat, double *res_lon);
void liberar_lugares(Place *lista);
void liberar_lista(House *lista);

double haversine(double lat1, double lon1, double lat2, double lon2);
Street* add_street(Street *cabeza, long long id1, double lat1, double lon1, long long id2, double lat2, double lon2, char *name);
Street* cargar_streets(char *path, int *total);

// función antigua, del lab 4:
void buscar_coordenada_antigua(Street *lista_streets, House *lista_casas, double user_lat, double user_lon);
// nueva función, del lab 5
// void buscar_coordenada(Grafo *g, Street *lista_streets, double user_lat, double user_lon);
void buscar_coordenada(Grafo *g, Street *lista_streets, House *lista_casas, double user_lat, double user_lon);

void liberar_streets(Street *lista);

void normalizar_coordenadas_string(char *buffer);
void pedir_coordenadas_seguras(double *lat, double *lon);

void leer_cadena_segura(char *buffer, int size);
int mapa_valido(const char *m);
void quitar_acentos(char *cadena);
void normalizar_nombre(char *dest, size_t dest_size, const char *src);

Grafo construir_grafo(Street *lista_streets);
int buscar_o_insertar_nodo(Grafo *g, long long id, double lat, double lon);
void añadir_adyacencia(NodoGrafo *nodo, int destino, double peso);
void liberar_grafo(Grafo *g);
long long buscar_nodo_mas_cercano(Grafo *g, double lat, double lon); 

int obtener_indice_nodo(Grafo *g, long long id);
void calcular_ruta_dijkstra(Grafo *g, Street *lista_streets, long long origen, long long destino);

#endif