#include <stdio.h> 
#include <math.h>
#include <unistd.h>
#include <GL/glfw.h>
#include <FTGL/ftgl.h>

#define WINDOW_WIDTH 1280 // Width of the GLFW window
#define WINDOW_HEIGHT 720 // Height of the GLFW window

// Coordinate conversion for projection matrix
#define REAL_SIZE_X 8.0
#define REAL_SIZE_Y 4.5
double ORIGIN_X, ORIGIN_Y;
#define MAX_ZOOM 2.5
#define MIN_ZOOM 0.3
double SCREEN_ZOOM;

// Source for the font
#ifndef FONT_FILENAME
#define FONT_FILENAME "assets/fonts/NotoSansJP-Regular.otf"
#endif
FTGLfont *font; // A pointer that points to the loaded fonts

// Functions to draw object in GLFW
void drawFilledCircle(double x, double y, double radius);
void drawHollowCircle(double x, double y, double radius);
void drawRectangle(double x0, double y0, double x1, double y1);
void drawString(double x, double y, double scale, char const *text);
void drawCar(double x0, double y0, double radius);
void drawMap(int crossing_number, int selection_state);
void drawPath(int crossing_number);

// Functions related to user input and initialization
void chooseByKeyboard(int crossing_number, int* selection_state);
void chooseByMouse(int crossing_number, int* selection_state);
void fontSetup();
void carInit();
void mousePosition(int x, int y);
void mouseToMap2d(double projection_matrix[]);

// Functions related to map and car control
void controlMap();
void controlZoom(int direction);
void carMovement(int *start);