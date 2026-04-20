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

// Funciones de utilidad y lógica
int distancia_levenshtein(char *s1, char *s2);
House* cargar_mapa(char *path, int *total);
void normalizar_nombre(char *dest, const char *src);
void buscar_direccion(House *lista);
void liberar_lista(House *lista);

#endif