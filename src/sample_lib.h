#ifndef SAMPLE_LIB_H
#define SAMPLE_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 1. La estructura para guardar las calles (lista enlazada)
typedef struct House {
    char street_name[100]; 
    int house_number; 
    double latitud; 
    double longitud; 
    struct House *next; 
} House;

// 2. Prototipos de tus funciones (las que irán en functions.c)

// Para Levenshtein
int minimo(int a, int b, int c);
int distancia_levenshtein(char *s1, char *s2);

// Para buscar sugerencias
void buscar_con_sugerencias(char *busqueda, House *lista);

// Para cargar el mapa desde el archivo houses.txt
House* cargar_mapa(char *path);

// (Opcional)
int fact(int n); 

#endif