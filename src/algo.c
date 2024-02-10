#include "../include/algo.h"
#include "../include/struct.h"

/*** Read the map file ***/
int readMap(char *filename) 
{
    FILE *fp;
    int crossing_number;

    fp = fopen(filename, "r"); 
    if (fp == NULL) { // File can't be opened
        perror(filename); return -1;
    }

    // To read the number of crossings in the first line
    fscanf(fp, "%d", &crossing_number); 
    if ((crossing_number < 1) || (crossing_number >= MAX_CROSSING)) {
        printf("Illegal data number (%d)\n", crossing_number);
        return -1; 
    }

    // Then read the remaining line and store the data as cross[i]
    for (int i = 0; i < crossing_number; i++) {
        fscanf(fp, "%d,%lf,%lf,%lf,%[^,],%[^,],%d",
                &(cross[i].id), &(cross[i].pos.x), &(cross[i].pos.y),
                &(cross[i].wait), cross[i].jname, cross[i].ename, &(cross[i].points));
        for (int j = 0; j < cross[i].points; j++) fscanf(fp, ",%d", &(cross[i].next[j]));
    }

    fclose(fp);
    return crossing_number;
}

/*** Calculate the angle between two vector (three points) ***/
double rotationAngle(double cx, double cy, double x0, double y0, double x1, double y1)
{
    /** We compute the angle based on the dot product and determinant of both vectors.
     * It is because the dot product is proportional to the cosine of the angle, while
     * the  determinant is proportional to its sine. The vector center is (cx, cy).
     * The result is then multiplied by (180/pi) to make it into degrees. **/

    double dot = (x0-cx)*(x1-cx) + (y0-cy)*(y1-cy);
    double det = (x0-cx)*(y1-cy) - (y0-cy)*(x1-cx);
    return atan2(det, dot) * 180 / M_PI;
}

/*** Algorithm to search for string containing the input ***/
int searchPartial(int crossing_number, int search_result[]) 
{
    char input[256];
    char data[MAX_NAME];
    int f = 0; // Flag set when item is found
    int k = 0; // Index to store the result in an array

    scanf("%s", input);

    for (int i = 0; i < crossing_number; i++) {
        for (int j = 0; cross[i].ename[j]; j++) // Lowercase each crossing name
            data[j] = tolower(cross[i].ename[j]);

        if (strstr(data, input) != NULL) { // If the input is included, will return 0
            search_result[k] = i; k++; // Store the result in this array

            printf("    ID: %d, Crossing Name: %s(%s)\n",
                    cross[i].id, cross[i].jname, cross[i].ename);
            f = 1;
        }
    }

    if (f == 0) printf("'%s' has not been found \n", input);
    return f;
}

/*** The pathfinding algorithm: Dijkstra algorithm ***/
void dijkstra(int crossing_number, int start, int goal)
{
    /** Dijkstra algorithm finds a shortest path tree from a single source node,
     * by building a set of nodes that have minimum distance from the source.
     * 
     * This is done by initializing three values:
     * - an unrealistically large distance value between each nodes
     * - set all crossing to be not visited
     * - set distance of starting node as 0
     * 
     * How it works:
     * 1. The algorithm starts at the starting node and analyzes the graph to find
     *    the shortest path between that node and all the other nodes in the graph.
     * 2. The algorithm keeps track of the currently known shortest distance from each
     *    node to the source node and it updates these values if it finds a shorter path.
     * 3. Once the algorithm has found the shortest path between the source node and
     *    another node, that node is marked as "visited" and added to the path.
     * 4. To select the next node (the one with smallest distance and is not visited),
     *    it is not necessary to be the neighbour of current node. The process continues
     *    until the destination node is visited.
     * **/

    int i, n;
    int current_cross;
    double min_distance; // To search for the crossing with smallest distance
    int visited[MAX_CROSSING]; // Flag set visited as 1 or not visited as 0
    int previous[MAX_CROSSING]; // To store parents, related to algorithm

    // Initialization
    for (i = 0; i < crossing_number; i++) {
        cross[i].distance = 1e100;
        visited[i] = 0;
    }
    cross[car.start].distance = 0;
    // Initialization to store the path result later
    for (i = 0; i < MAX_CROSSING; i++) car.path[i] = -1;

    current_cross = car.start; // Set current position at starting point
    while (current_cross != goal) {
        // Update distances and parents (previous) of unvisited neighbors
        for (i = 0; i < cross[current_cross].points; i++) {
            n = cross[current_cross].next[i];
            min_distance = cross[current_cross].distance + distance(current_cross, n);

            if (min_distance < cross[n].distance && visited[n] != 1) {
                cross[n].distance = min_distance;
                previous[n] = current_cross;
            }
        }

        visited[current_cross] = 1; // Mark current crossing as visited

        min_distance = 1e100;
        for (i = 0; i < crossing_number; i++) { // Select next crossing to visit
            if (cross[i].distance <= min_distance && visited[i] != 1) {
                min_distance = cross[i].distance;
                current_cross = i;
            }
        }
    }

    /** Up to this line, the path has been found **/
    // Trace back the path from destination to origin using parents
    car.path[0] = current_cross; // This first index indicate the destination node
    for (i = 1; current_cross != car.start; i++) {
        car.path[i] = previous[current_cross];
        current_cross = previous[current_cross];
    }
    
    // Reverse the array so it will be start to origin
    n = 0; for (i = 0; car.path[i] >= 0; i++) n++;
    arrayReverseUntil(n, car.path); // n indicate the limit for reverse

    // Display the shortest path in the terminal
    puts("");
    printf("Path has been confirmed.\nNo. | Distance | Intersection\n");
    for (int i = 0; car.path[i] >= 0; i++) printf("%3d | %8.3lf | %s\n",
        i+1, cross[car.path[i]].distance, cross[car.path[i]].ename);
}

/*** Calculate distance between two points ***/
double distance(int a, int b)
{
    return hypot(cross[a].pos.x - cross[b].pos.x, cross[a].pos.y - cross[b].pos.y);
}

/*** Swap between two values ***/
void swap(int *a, int *b)
{
    int temp = *a; *a = *b; *b = temp;
}

/*** Reverse value inside array from beginning until certain limit ***/
void arrayReverseUntil(int until, int array[])
{
    // One by one reverse first and last elements of array[0..n-1]
    for (int i = 0; i < until/2; i++) swap(&array[i], &array[until-i-1]);
}

/*** Search if value is in array ***/
int valueInArray(int value, int array[], int array_length)
{
    int found = 0; // Status of search
    for (int i = 0; i < array_length; i++) {
        if (array[i] == value) {
            found = 1; break;
        }
    }
    return found; // Return 1 if found
}