#include "sample_lib.h"
#include <sys/stat.h>

void createleak(){
    char *foo = malloc(20 * sizeof(char)); 
    printf("Allocated leaking string: %s", foo); 

}

typedef struct House{
    char  street_name[100]; 
    int house_number; 
    double latitud; 
    double longitud; 
    struct House *next; 
} House; 

int main(){
char map_name[10]; 
char path[150]; 
char search_street[100]; 
int search_nummber; 
char origin_position[20]; 

// Primer demanem el mapa a l'usuari 
    printf("Enter map name (e.g. 'xs_2' or 'xl_1'):");
    scanf("%s", map_name);  

    //Obrim el fitxer de houses.txt 
    sprintf(path, "maps/%s/houses.txt", map_name); 
    FILE *f = fopen(path,"r"); 
    
    if (f == NULL){
        printf("Error per obrir el fitxer %s\n", path); 
        return 1; 
    }

    printf("Com vols introduir l'origen? (adreça, coordenada, lloc): ");
    fgets(origin_position, sizeof(origin_position), stdin);
    
    // Treure el salt de línia final
    origin_position[strcspn(origin_position, "\n")] = '\0';
    
    // Convertir a minúscules per comparar sense distingir majúscules
    for (int i = 0; origin_position[i]; i++) {
        origin_position[i] = tolower(origin_position[i]);
    }
    
    switch (origin_position[0]) {  // Comparem només la primera lletra per simplificar
        case 'a':  // adreça
            printf("Has triat adreça. Demanaré carrer i número...\n");
            // Aquí crides a la funció que demana carrer i número
            break;
            
        case 'c':  // coordenada
            printf("Not implemented yet\n");
            break;
            
        case 'l':  // lloc
            printf("Not implemented yet\n");
            break;
            
        default:
            printf("Opció no vàlida\n");
            break;
    }
    
    return 0;

}