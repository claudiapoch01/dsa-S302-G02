# Report

## 1. Runtime Complexity Analysis

### 1.1 Initializing the Intersections Map
* **Best Case:** O(...) - *Explica brevemente por qué*
* **Average Case:** O(...)
* **Worst Case:** O(...)

### 1.2 Finding Coordinates by Street/Place Name
* **Best Case:** O(...)
* **Average Case:** O(...)
* **Worst Case:** O(...)

### 1.3 Path-Finding Algorithm
* **Best Case:** O(...)
* **Average Case:** O(...)
* **Worst Case:** O(...)

---

## 2. Experimental Results and Plots

### 2.1 Latency: Sequential Search vs. Intersections Map (By Map Size)

#### Raw Data
| Map Size (Elements/Nodes) | Sequential Search Latency (ms) | Intersections Map Latency (ms) |
|---------------------------|--------------------------------|---------------------------------|
| Small (e.g., 100)         | ...                            | ...                             |
| Medium (e.g., 1000)       | ...                            | ...                             |
| Large (e.g., 10000)       | ...                            | ...                             |

#### Plot
![Latency vs Map Size for Connected Streets](ruta_a_tu_grafica_1.png)

#### Explanation
*Escribe aquí por qué el mapa de intersecciones (Lab 5) es mucho más rápido que buscar secuencialmente en una lista (Lab 4) a medida que el mapa se hace más grande.*

### 2.2 Path-Finding Latency: Sequential vs. Map (By Map Size)

#### Raw Data
| Map Size | Sequential Path Latency (ms) | Map Path Latency (ms) |
|----------|------------------------------|-----------------------|
| Small    | ...                          | ...                   |
| Medium   | ...                          | ...                   |
| Large    | ...                          | ...                   |

#### Plot
![Path-Finding Latency vs Map Size](ruta_a_tu_grafica_2.png)

#### Explanation
*Explica cómo influye el tamaño del mapa en tu algoritmo de búsqueda de caminos (seguramente un BFS o Dijkstra) cuando usa la lista del Lab 4 vs el mapa del Lab 5.*

### 2.3 Path-Finding Latency: Effect of Distance (Same Map)

#### Raw Data
| Distance (meters/nodes) | Sequential Latency (ms) | Map Latency (ms) |
|-------------------------|-------------------------|------------------|
| Short                   | ...                     | ...              |
| Medium                  | ...                     | ...              |
| Long                    | ...                     | ...              |

#### Plot
![Path-Finding Latency vs Distance](ruta_a_tu_grafica_3.png)

#### Explanation and Curve Fitting
*Explica cómo cambia el tiempo según la distancia. Aquí debes "ajustar una curva" (curve fit). Si tu algoritmo de caminos teórico es $O(V + E)$, justifica si tu gráfica se parece a una línea recta (lineal) o a otra forma geométrica basándote en la pregunta 1.3.*

---

## 3. Algorithm and Data Structure Improvements

### 3.1 Visited Data Structure in BFS
* **Proposed Structure:** *¿Qué estructura usarías? (Pista: Un `HashSet` o `Set` en lugar de una lista `List`).*
* **Justification:** *¿Por qué es mejor?*
* **Runtime Complexity:**
  * **Current (with List):** O(...) para buscar si ya fue visitado.
  * **Improved (with New Structure):** O(...)
* **Trade-offs / Downsides:** *¿Gasta más memoria? ¿Hay algún contra?*

### 3.2 Finding Street Segment by Latitude and Longitude
* **Proposed Structure/Algorithm:** *¿Qué usarías para no recorrer todos los segmentos uno a uno? (Pista: Un R-Tree, Quadtree o KD-Tree, que son para datos espaciales).*
* **Justification:** *Explica cómo ayuda a "recortar" el mapa y buscar solo cerca.*
* **Runtime Complexity:**
  * **Current:** O(N) - Tienes que mirar todos los segmentos.
  * **Improved:** O(log N) - Búsqueda en árbol.
* **Trade-offs / Downsides:** *Complejidad de implementar, memoria para guardar el árbol, etc.*
