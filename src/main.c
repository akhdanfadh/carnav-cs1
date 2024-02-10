#include <stdio.h>
#include "../include/struct.h"
#include "../include/algo.h"
#include "../include/display.h"

int main(void)
{
    int crossing_number; // Anount of crossing from file
    int width, height; // Used later in GLFWGetWindowSize
    int input_mode; // Store the input mode (keyboard/mouse)
    int selection_state = 1; // Indicate the state of program

    // Initialization for the algorithm
    car.start = -1; car.goal = -1;
    mouse.wheel = 0; SCREEN_ZOOM = 1.0;
    ORIGIN_X = 0.0; ORIGIN_Y = 0.0;

    // Read the map file
    crossing_number = readMap("data/map.dat");
    if (crossing_number <= 0) return 1; // Failed to read
    printf("Loaded %d crossings.\n", crossing_number);

    // Asking user for the input mode
    puts("");
    printf("How do you want to choose the path?\n");
    printf("1. Keyboard (Text and search)\n2. Mouse\n");
    do {
        printf("(1/2) -> ");
        while(scanf("%d", &input_mode) == 0) { // If input isn't integer
            printf("Invalid input.\n(1/2) -> "); int c;
            while((c=getchar()) != '\n' && c != EOF); // Clear the input buffer
        }
        if (input_mode != 1 && input_mode != 2) // If input isn't 1 or 2
            printf("Please input either 1 or 2.\n");
    } while (input_mode != 1 && input_mode != 2);

    // If the input mode is keyboard
    if (input_mode == 1) chooseByKeyboard(crossing_number, &selection_state);

    // Initialize the graphic environment and open a window with no resize
    glfwInit();
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);

    // Configure the font and callback function
    fontSetup();
    glfwSetMousePosCallback(&mousePosition);
    glfwSetMouseWheelCallback(&controlZoom);
    
    for (;;)
    {
        // End program if the user pushes Esc or closes the window
        if (glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)) break;

        // Project the space with this range to the viewport
        double projection_matrix[6] = {
            ORIGIN_X + REAL_SIZE_X * -0.5 * SCREEN_ZOOM,
            ORIGIN_X + REAL_SIZE_X * 0.5 * SCREEN_ZOOM,
            ORIGIN_Y + REAL_SIZE_Y * -0.5 * SCREEN_ZOOM,
            ORIGIN_Y + REAL_SIZE_Y * 0.5 * SCREEN_ZOOM,
            -10.0, 10.0
        };

        // Set up a projection matrix
        glMatrixMode(GL_PROJECTION); 
        glLoadIdentity();
        glOrtho(projection_matrix[0], projection_matrix[1],
                projection_matrix[2], projection_matrix[3],
                projection_matrix[4], projection_matrix[5]);

        // Set up a model view conversion matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        controlMap(); // Map control using arrow or mouse right button
        glfwGetWindowSize(&width, &height); // Obtain the current window size
        glViewport(0, 0, width, height); // Use the whole window as a viewport

        // Fill the back buffer with black
        glClearColor(18/255.0, 18/255.0, 18/255.0, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mouseToMap2d(projection_matrix); // Convert mouse position relative to map

        // Choose start and final destination using mouse
        if (input_mode == 2 && selection_state != 0)
            chooseByMouse(crossing_number, &selection_state);

        // Display the path and car movement after path initialization
        else if (selection_state == 0) {
            // If the current position is not final, user can move the car
            if (car.now != car.goal) carMovement(&car.now);
            else if (car.now == car.goal) car.move_mode = 0;

            drawMap(crossing_number, selection_state);
            drawPath(crossing_number);
        }

        glfwSwapBuffers(); // Swap the front buffer with the back buffer
        usleep(50 * 1000); // Wait for about 50 millisecond
    }

    glfwTerminate(); // Close the window
    return 0; 
}