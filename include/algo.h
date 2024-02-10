#include <stdio.h> 
#include <math.h>
#include <string.h>
#include <ctype.h>

// Algorithm related to the display
int readMap(char *filename);
double rotationAngle(double cx, double cy, double x0, double y0, double x1, double y1);

// Algorithm about partial search and path optimization
int searchPartial(int crossing_number, int search_result[]);
void dijkstra(int crossing_number, int start, int goal);

// Necessary algorithm
double distance(int a, int b);
void swap(int *a, int *b);
void arrayReverseUntil(int until, int path[]);
int valueInArray(int value, int array[], int array_length);

