#include "sample_lib.h"
#include <time.h>
#include <stdio.h>

FILE *original_stdout;

void silenciar() {
    fflush(stdout);
    original_stdout = stdout;
    freopen("/dev/null", "w", stdout);
}

void restaurar() {
    fflush(stdout);
    fclose(stdout);
    stdout = original_stdout;
}

double timer_ms(clock_t start, clock_t end) { // devuelve el tiempo en milisesgundos 
    return (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

int main() {
    const char *mapes[] = {"xs_1", "md_1", "xl_1"};
    const char *noms[] = {"XS", "MD", "XL"};
    
    printf("\nMEDIDAS DE RENDIMIENTO PARA EL REPORT\n\n");
    
    for (int m = 0; m < 3; m++) { // Bucle que recorre los 3 mapas (XS, MD, XL)
        printf("--- Map: %s (%s) ---\n", mapes[m], noms[m]);
        
        char path_houses[150], path_places[150], path_streets[150]; // rutas de los archivos de cada mapa
        // Construye las rutas correctas a los archivos
        sprintf(path_houses, "../maps/%s/houses.txt", mapes[m]);
        sprintf(path_places, "../maps/%s/places.txt", mapes[m]);
        sprintf(path_streets, "../maps/%s/streets.txt", mapes[m]);
        
        int num_houses = 0, num_places = 0, num_streets = 0; //contadores de las cargas 
        House *houses = cargar_mapa(path_houses, &num_houses);
        Place *places = cargar_lugares(path_places, &num_places);
        
        StreetHashMap hash_map; // Declara la estructura de la tabla hash para guardar nombres de calles
        memset(&hash_map, 0, sizeof(StreetHashMap)); // Inicializa las tablas hash a 0
        Street *streets = cargar_streets_con_hash(path_streets, &num_streets, &hash_map); //Carga las calles y contruye las tabla hash 
        
        if (!streets) {
            printf("  ERROR: No s'ha pogut carregar %s\n\n", mapes[m]);
            continue;
        }
        
        Grafo g = construir_grafo(streets);
        printf("  Nodes: %d\n", g.total_nodos);
        
        if (g.total_nodos > 0) { // Comprovar que el grafo tiene al menos un nodo
            // Toma la longitud i laitud del primer nodo como punto de prueba
            double lat = g.nodos[0].latitud;
            double lon = g.nodos[0].longitud;
            
            // Silenciar sortida durant les mesures
            silenciar();
            
            // SEQUENCICAL
            clock_t start = clock();
            for (int i = 0; i < 100; i++) { // Bucle de 100 iteraciones para tener una muestra fiable 
                buscar_coordenada_antigua(streets, houses, lat, lon); // Versión del lab4 que busca calles conectadas
            }
            clock_t end = clock(); // tiempo final 
            double seq_avg = timer_ms(start, end) / 100.0; // Calcula el tiempo medio por iteración
            
            // HASH
            start = clock();
            for (int i = 0; i < 100; i++) { // Bucle de 100 iteraciones para tener una muestra fiable 
                buscar_coordenada(&g, &hash_map, lat, lon); // Versión con hash (Lab 5) que busca calles conectadas
            }
            end = clock();//tiempo final 
            double hash_avg = timer_ms(start, end) / 100.0; //Calcula el tiempo medio por iteración
            
            // Restaurar sortida per imprimir resultats
            restaurar();
            
            printf("  Cerca seqüencial: %.4f ms\n", seq_avg); // latencia sequencial 
            printf("  Cerca hash:       %.4f ms\n", hash_avg); //latencia hash 
            printf("  Speedup:          %.2fx\n", seq_avg / hash_avg); // acceleració
            
            // BFS
            if (g.total_nodos >= 2) { // comprueba que almenos haya 2 nodos para poder hacer el bfs
                long long src = g.nodos[0].id; // ID del primer nodo como origen
                long long dst = g.nodos[g.total_nodos - 1].id; // ID del ultimo nodo como destino
                
                silenciar();
                start = clock(); // tiempo incial
                for (int i = 0; i < 50; i++) {
                    calcular_ruta_bfs(&g, &hash_map, src, dst);
                }
                end = clock(); //tiempo final 
                double bfs_avg = timer_ms(start, end) / 50.0; //Calcula el tiempo medio por BFS 
                restaurar();
                
                printf("  BFS:             %.4f ms\n", bfs_avg);
            }
        }
        //liberamos memorias 
        if (houses) liberar_lista(houses);
        if (places) liberar_lugares(places);
        if (streets) liberar_streets(streets);
        hash_liberar(&hash_map);
        liberar_grafo(&g);
        
        printf("\n");
    }
    
    printf("FIN DE LA FUNCION\n");
    return 0;
}