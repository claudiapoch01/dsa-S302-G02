#ifndef SAMPLE_LIB_H
#define SAMPLE_LIB_H

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

// --- NUEVO: Estructura para lugares ---
typedef struct Place {
    char name[150];
    double latitud;
    double longitud;
    struct Place *next;
} Place;

// Prototipos existentes
int distancia_levenshtein(char *s1, char *s2);
House* cargar_mapa(char *path, int *total);
void buscar_direccion(House *lista);

// --- NUEVO: Prototipos para lugares ---
Place* cargar_lugares(char *path, int *total);
void buscar_lugar(Place *lista);
void liberar_lugares(Place *lista);
void liberar_lista(House *lista);

#endif