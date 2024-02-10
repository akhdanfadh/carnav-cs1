#include "../include/display.h"
#include "../include/struct.h"
#include "../include/algo.h"

/*** GLFW basic function to draw filled circle ***/
void drawFilledCircle(double x, double y, double radius)
{
	int i, triangleAmount = 36; // Triangles used to draw circle
	double two_pi = 2.0 * M_PI;
	
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // Center of circle
		for(i = 0; i <= triangleAmount; i++) { 
			glVertex2d(x + (radius * cos(i *  two_pi / triangleAmount)), 
			            y + (radius * sin(i * two_pi / triangleAmount)));
		}
	glEnd();
}

/*** GLFW basic function to draw hollow circle ***/
void drawHollowCircle(double x, double y, double radius)
{
	int lineAmount = 36; // Lines used to draw circle
	double two_pi = 2.0 * M_PI;
	
    glLineWidth(2);
	glBegin(GL_LINE_LOOP);
		for(int i = 0; i <= lineAmount; i++) { 
			glVertex2d(x + (radius * cos(i *  two_pi / lineAmount)), 
			            y + (radius * sin(i * two_pi / lineAmount)));
		}
	glEnd();
}

/*** GLFW basic function to draw rectangle ***/
void drawRectangle(double x0, double y0, double x1, double y1)
{
    glBegin(GL_QUADS);
        glVertex2d(x0,y0); glVertex2d(x0,y1);
        glVertex2d(x1,y1); glVertex2d(x1,y0);
    glEnd();
}

/*** GLFW basic function to draw text (string) ***/
void drawString(double x, double y, double scale, char const *text)
{
    glPushMatrix();
        glTranslated(x, y, 0.0);
        glScaled(scale, scale, scale);
        ftglRenderFont(font, text, FTGL_RENDER_ALL);
    glPopMatrix();
}

/*** Draw the car as a pointer ***/
void drawCar(double x0, double y0, double radius)
{
    // Draw the circle in the center
    drawFilledCircle(x0, y0, radius);

    // The pointer appears only if the car is in drive mode
    if (car.move_mode == 1 && car.now != car.next_cross) {
        int lineAmount = 36; double lineArc;
        // Rotation angle to point to the next crossing
        double rotateAngle = rotationAngle(x0, y0, x0, y0 + 2.5 * radius,
                cross[car.next_cross].pos.x, cross[car.next_cross].pos.y);

        // Create rotation matrix only for the pointer, so it won't affect the map
        glPushMatrix();
            glTranslatef(x0,y0, 0);
            glRotatef(rotateAngle, 0, 0, 1);
            glTranslatef(-x0,-y0, 0);

            // Create a "Star Trek"-like pointer
            glBegin(GL_TRIANGLE_FAN);
                glVertex2d(x0, y0 + 2.5 * radius);
                for(int i = 0; i <= lineAmount; i++) { 
                    lineArc = M_PI/4 + i*(M_PI/2)/lineAmount;
                    glVertex2d(x0 + (radius * cos(lineArc)),
                    (y0 + radius/2) + (radius * sin(lineArc)));
            }
            glEnd();
        glPopMatrix();
    }
}

/*** Draw the plain map (without path) ***/
void drawMap(int crossing_number, int selection_state)
{
    int i, j, k;
    double x0, y0, x1, y1, scale = 0.005;
    double radius = 0.05*SCREEN_ZOOM < 0.08 ? 0.05*SCREEN_ZOOM : 0.08;

    // Determine the font size
    if (SCREEN_ZOOM > 1) scale += (SCREEN_ZOOM-1)*0.0041;
    else if (SCREEN_ZOOM < 1) scale -= (1-SCREEN_ZOOM)*0.005;

    // Draw the roads (straight lines to each neighboring crossing)
    for (i = 0; i < crossing_number; i++) {
        x0 = cross[i].pos.x; y0 = cross[i].pos.y;

        glLineWidth(10);
        for (j = 0; j < cross[i].points; j++) {
            k = cross[i].next[j];
            x1 = cross[k].pos.x; y1 = cross[k].pos.y;

            glColor3d(50/255.0, 50/255.0, 50/255.0);
            glBegin(GL_LINES);
                glVertex2d(x0, y0); glVertex2d(x1, y1);
            glEnd();
        }
    }

    // Draw the crossing as a circle
    for (i = 0; i < crossing_number; i++) {
        x0 = cross[i].pos.x; y0 = cross[i].pos.y;

        glColor3d(80/255.0, 80/255.0, 80/255.0);
        drawFilledCircle(x0, y0, radius);
    }

    // Draw crossing name when mouse hover
    if (selection_state != 0) {
        for (i = 0; i < crossing_number; i++) {
            x0 = cross[i].pos.x; y0 = cross[i].pos.y;
            
            if (mouse.map.x > x0-radius && mouse.map.x < x0+radius &&
                mouse.map.y > y0-radius && mouse.map.y < y0+radius) {
                // The scene selection if user input is using mouse
                if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
                    if (selection_state == 1) car.start = i;
                    else if (selection_state == 2 && i != car.start) car.goal = i;
                }
                
                glColor3d(1.0, 1.0, 1.0);
                drawString(x0+radius, y0+radius, scale, cross[i].ename);
            }
        }
    }
}

/*** Draw the path if only the start and final destination is chosen ***/
void drawPath(int crossing_number)
{
    int i, j, k, n;
    double x0, y0, x1, y1, scale = 0.005;
    double radius = 0.05*SCREEN_ZOOM < 0.08 ? 0.05*SCREEN_ZOOM : 0.08;

    // Determine the font size
    if (SCREEN_ZOOM > 1) scale += (SCREEN_ZOOM-1)*0.0041;
    else if (SCREEN_ZOOM < 1) scale -= (1-SCREEN_ZOOM)*0.005;

    // Draw the line path
    for (i = 0; i < crossing_number; i++) {
        //Loop only until the final destination (index indicated as -1)
        n = car.path[i]; if (car.path[i] == -1) break;
        x0 = cross[n].pos.x; y0 = cross[n].pos.y;

        glLineWidth(10);
        for (j = 0; j < cross[n].points; j++) {
            k = cross[n].next[j];
            x1 = cross[k].pos.x; y1 = cross[k].pos.y;

            if (valueInArray(k, car.path, crossing_number) == 1) {
                glColor3d(200/255.0, 200/255.0, 200/255.0);
                glBegin(GL_LINES);
                    glVertex2d(x0, y0); glVertex2d(x1, y1);
                glEnd();
            }
        }
    }

    // Draw the crossing circle
    for (i = 0; i < crossing_number; i++) {
        //Loop only until the final destination (index indicated as -1)
        n = car.path[i]; if (car.path[i] == -1) break;
        x0 = cross[n].pos.x; y0 = cross[n].pos.y;

        glColor3d(150/255.0, 150/255.0, 150/255.0);
        drawFilledCircle(x0, y0, radius);
    }

    // Draw crossing name when mouse hover, so that it will not overlapped by path
    for (i = 0; i < crossing_number; i++) {
        x0 = cross[i].pos.x; y0 = cross[i].pos.y;
        
        if (mouse.map.x > x0-radius && mouse.map.x < x0+radius &&
            mouse.map.y > y0-radius && mouse.map.y < y0+radius) {
            glColor3d(1.0, 1.0, 1.0);
            drawString(x0+radius, y0+radius, scale, cross[i].ename);
        }
    }
    // Draw the start and final destination as hollow circle
    glColor3d(187/255.0, 134/255.0, 252/255.0);
    drawHollowCircle(cross[car.goal].pos.x, cross[car.goal].pos.y,
                    0.1*SCREEN_ZOOM < 0.15 ? 0.1*SCREEN_ZOOM : 0.15);
    glColor3d(3/255.0, 218/255.0, 197/255.0);
    drawHollowCircle(cross[car.start].pos.x, cross[car.start].pos.y,
                    0.1*SCREEN_ZOOM < 0.15 ? 0.1*SCREEN_ZOOM : 0.15);

    // Draw the car
    glColor3d(239/255.0, 71/255.0, 111/255.0);
    drawCar(car.pos.x, car.pos.y,
            0.075*SCREEN_ZOOM <= 0.075 ? 0.075*SCREEN_ZOOM : 0.075);
}

/*** Workflow function to choose path via keyboard ***/
void chooseByKeyboard(int crossing_number, int* selection_state)
{
    /** First is to get input for starting and destination point. Then search
     * the shortest path using dijkstra algorithm and initialize what needs to be
     * initialized. **/

    int f, c;
    int search_result[crossing_number]; // Array to store the partial search result

    // Get the starting point
    puts("");
    for (int i = 0; i < crossing_number; i++) search_result[i] = -1;
    do {
        printf("Search the starting point -> ");
        f = searchPartial(crossing_number, search_result);
    } while (f == 0);
    do {
        printf(" -> Input the ID: ");
        while(scanf("%d", &car.start) == 0) { // If input isn't integer
            printf("Invalid input.\n -> Input the ID: ");
            while((c=getchar()) != '\n' && c != EOF); // Clear the input buffer
        }
        f = valueInArray(car.start, search_result, crossing_number);
        if (f == 0 || car.start == -1) // If input isn't in search result
            printf("ID: %d is not in search value.\n", car.start);
    } while (f == 0 || car.start == -1);

    // Get the destination point
    puts("");
    for (int i = 0; i < crossing_number; i++) search_result[i] = -1;
    do {
        printf("Search the destination point -> ");
        f = searchPartial(crossing_number, search_result);
    } while (f == 0);
    do {
        printf(" -> Input the ID: ");
        while(scanf("%d", &car.goal) == 0) { // If input isn't integer
            printf("Invalid input.\n -> Input the ID: ");
            while((c=getchar()) != '\n' && c != EOF); // Clear the input buffer
        }
        f = valueInArray(car.goal, search_result, crossing_number);
        if (f == 0 || car.goal == -1) // If input isn't in search result
            printf("ID: %d is not in search value.\n", car.goal);
    } while (f == 0 || car.goal == -1);

    // Continue the process for algorithm and necessary initialization
    dijkstra(crossing_number, car.start, car.goal);
    carInit();
    ORIGIN_X = car.pos.x; ORIGIN_Y = car.pos.y;
    *selection_state = 0;
}

/*** Workflow function to choose path via mouse ***/
void chooseByMouse(int crossing_number, int* selection_state)
{
    /** First it to draw the plain map to select the starting and destination point.
     * User is needed to click in one of the crossings for both points with the
     * first one is the start, followed by the destination. All these process are
     * indicated using selection_state variable (0 for keyboard input/finish,
     * 1 for start selection, 2 for destination selection). Once user finished the
     * input, the dijkstra algorithm takes process and then display the path in the
     * terminal. **/

    // Draw the plain map and wait for mouse input
    drawMap(crossing_number, *selection_state);

    // Once starting point is determined, display it in terminal and map
    if (car.start != -1) {
        if (*selection_state == 1) {
            puts("");
            printf("Starting point: %s (ID: %d)\n", cross[car.start].ename, car.start);
        }
        glColor3d(3/255.0, 218/255.0, 197/255.0);
        drawHollowCircle(cross[car.start].pos.x, cross[car.start].pos.y,
                        0.1*SCREEN_ZOOM < 0.15 ? 0.1*SCREEN_ZOOM : 0.15);

        *selection_state = 2; // Move to the next state
    }

    // Once destination point is determined, display it in terminal and map
    if (car.goal != -1) {
        printf("Destination point: %s (ID: %d)\n", cross[car.goal].ename, car.goal);
        glColor3d(187/255.0, 134/255.0, 252/255.0);
        drawHollowCircle(cross[car.goal].pos.x, cross[car.goal].pos.y,
                        0.1*SCREEN_ZOOM < 0.15 ? 0.1*SCREEN_ZOOM : 0.15);

        // Continue the process for algorithm and necessaryinitialization
        dijkstra(crossing_number, car.start, car.goal);
        carInit();
        *selection_state = 0;
    }
}

/*** GLFW initialization font setup ***/
void fontSetup()
{
    font = ftglCreateExtrudeFont(FONT_FILENAME);
    if (font == NULL) {
        perror(FONT_FILENAME);
        fprintf(stderr, "Could not load the font\n");
        exit(1);
    }

    ftglSetFontFaceSize(font, 24, 24);
    ftglSetFontDepth(font, 0.01);
    ftglSetFontOutset(font, 0, 0.1);
    ftglSetFontCharMap(font, ft_encoding_unicode);
}

/*** Necessary initialization car variable setup ***/
void carInit()
{
    car.next_cross = car.start;
    car.pos_inroad = 0;
    car.pos.x = cross[car.start].pos.x;
    car.pos.y = cross[car.start].pos.y;
    car.path_index = 1;
    car.now = car.start;
}

/*** GLFW callback function to read mouse position relative window ***/
void mousePosition(int x, int y)
{   
    mouse.old.x = mouse.now.x;
    mouse.old.y = mouse.now.y;
    mouse.now.x = x; mouse.now.y = y;
    mouse.changed = 1;
}

/*** Change the mouse position relative to current map projection ***/
void mouseToMap2d(double projection_matrix[])
{
    double length = projection_matrix[1] - projection_matrix[0];
    double width = projection_matrix[3] - projection_matrix[2];

    mouse.map.x = projection_matrix[0] + (mouse.now.x / WINDOW_WIDTH) * length;
    mouse.map.y = projection_matrix[3] - (mouse.now.y / WINDOW_HEIGHT) * width;
}

/*** Translate/move the map projection using user input ***/
void controlMap()
{
    double change_arrow = 0.25; // Amount of change using keyboard arrow
    double change_mouse = 0.15; // Amount of change using drag mouse right button

    // Logical condition for the keyboard arrow
    if (glfwGetKey(GLFW_KEY_UP)) ORIGIN_Y += change_arrow;
    else if (glfwGetKey(GLFW_KEY_DOWN)) ORIGIN_Y -= change_arrow;
    else if (glfwGetKey(GLFW_KEY_RIGHT)) ORIGIN_X += change_arrow;
    else if (glfwGetKey(GLFW_KEY_LEFT)) ORIGIN_X -= change_arrow;

    // Logical condition for the mouse right button
    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) && mouse.changed == 1) {
        if (mouse.now.x < mouse.old.x) {
            ORIGIN_X += change_mouse;
            if (mouse.now.y < mouse.old.y) ORIGIN_Y -= change_mouse;
            else if (mouse.now.y > mouse.old.y) ORIGIN_Y += change_mouse;
        }
        else if (mouse.now.x > mouse.old.x) {
            ORIGIN_X -= change_mouse;
            if (mouse.now.y < mouse.old.y) ORIGIN_Y -= change_mouse;
            else if (mouse.now.y > mouse.old.y) ORIGIN_Y += change_mouse;
        }
        else if (mouse.now.y < mouse.old.y) {
            ORIGIN_Y -= change_mouse;
            if (mouse.now.x < mouse.old.x) ORIGIN_X += change_mouse;
            else if (mouse.now.x > mouse.old.x) ORIGIN_X -= change_mouse;
        }
        else if (mouse.now.y > mouse.old.y) {
            ORIGIN_Y += change_mouse;
            if (mouse.now.x < mouse.old.x) ORIGIN_X += change_mouse;
            else if (mouse.now.x > mouse.old.x) ORIGIN_X -= change_mouse;
        }
    }

    mouse.changed = 0; // Needed so that above function only work once for each input
}

/*** Scale/zoom the map projection using user input ***/
void controlZoom(int direction)
{
    if (direction < mouse.wheel && SCREEN_ZOOM < MAX_ZOOM) {
        SCREEN_ZOOM += 0.1;
        mouse.wheel = direction;
    }
    else if (direction > mouse.wheel && SCREEN_ZOOM > MIN_ZOOM) {
        SCREEN_ZOOM -= 0.1;
        mouse.wheel = direction;
    }
    else mouse.wheel = direction;
}

/*** Function to change the car position to follow the path ***/
void carMovement(int *now)
{
    // Store the next crossing to follow the resulting path
    car.next_cross = car.path[car.path_index];

    // Set (x0, y0), (x1, y1) as the current and next crossings
    double x0 = cross[*now].pos.x, x1 = cross[car.next_cross].pos.x;
    double y0 = cross[*now].pos.y, y1 = cross[car.next_cross].pos.y;

    // Indicate the steps for car to move in each line/road
    double distance = hypot(x1 - x0, y1 - y0);
    int steps = (int)(distance / 0.05);

    // The car move if user press the spacebar
    if (glfwGetKey(GLFW_KEY_SPACE)) {
        car.move_mode = 1;

        // Move forward on the road and update the coordinate
        car.pos_inroad++;
        car.pos.x = x0 + (x1 - x0) / steps * car.pos_inroad;
        car.pos.y = y0 + (y1 - y0) / steps * car.pos_inroad;
    }
    
    // After reaching the crossing, enter the next road
    if (car.pos_inroad >= steps) {
        *now = car.next_cross; // Swap the current position
        car.pos_inroad = 0; // Reset the index of the n-th steps on the road
        car.path_index++; // Increment the index of the car path
    }
}