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

// Funciones para direcciones
int distancia_levenshtein(char *s1, char *s2);
House* cargar_mapa(char *path, int *total);
House* add_casa(House *cabeza, char *street, int num, double lat, double lon);
void buscar_direccion(House *lista);

// Funciones para lugares
Place* cargar_lugares(char *path, int *total);
Place* add_lugar(Place *cabeza, char *name, double lat, double lon);
void buscar_lugar(Place *lista);
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
#endif