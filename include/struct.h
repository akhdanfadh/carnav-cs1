#define MAX_CROSSING 100 /* Number of crossings=100 */
#define MAX_NAME 50 /* Maximum characters=50 (single-byte characters) */

typedef struct {
    double x, y; /* Position x, y */
} Position; /* Structure representing a position */

typedef struct {
    int id; /* Crossing ID */
    Position pos; /* Structure representing a position */ 
    double wait; /* Average wait time */
    char jname[MAX_NAME]; /* Crossing name (Japanese) */ 
    char ename[MAX_NAME]; /* Crossing name (alphabetical) */
    int points; /* Number of converging roads */
    int next[5]; /* Nearest crossing ID */
    double distance; /* The distance to the reference crossing: add */
} Crossing;

typedef struct {
    int start, goal, now; // The first and final destination, and current
    int move_mode; // Indicate the car is moving or not
    Position pos; // Position of car
    int next_cross; // Position of car on the overall path
    int pos_inroad; // Position of car on one road from the path
    int path[MAX_CROSSING]; // To store the shortest path
    int path_index; // Variable that indicate the road on the path
} Car; /* Structure representing movement of car */

typedef struct {
    Position now; // Position of the cursor now
    Position old; // Position of the previous car
    Position map; // Position of the cursor relative to map
    int changed; // Status if the mouse position has changed
    int wheel; // Store the scroll wheel position
} Mouse;

Car car;
Mouse mouse;
Crossing cross[MAX_CROSSING];