#include <stdlib.h>
#include <glut.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <cstdlib> 
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int windowWidth = 1200;
const int windowHeight = 800;
const int gameWidth = 1180;
const int gameHeight = 750;
int lives = 5;
int score = 0;
int minutes = 2; // Initial countdown minutes
int seconds = 45; // Initial countdown seconds
int countdownTimer; // Timer ID
bool timerRunning = true; // Flag to indicate whether the timer is running
float playerX = gameWidth / 2; // Initial X position
float playerY = gameHeight / 2; // Initial Y position
float playerRotation = 0.0; // Initial rotation angle
float originalPlayerSpeed = 10.0;
float playerSpeed = originalPlayerSpeed; // Speed of player movement
bool gameOver = false;
bool winner = false;
struct Collectable {
    float x;
    float y;
    bool isSquare;
};

Collectable collectables[6];
struct PowerUp {
    float x;
    float y;
    bool active;
    int type;
    int duration;// 1 for +5 speed, 2 for +10 speed
};
PowerUp powerUps[2];
struct Obstacle {
    float x;
    float y;
    bool active;
};
Obstacle obstacles[4];

struct CustomObject {
    float x;
    float y;
};
CustomObject goalObject;


float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void renderHealthBar() {
    // Render the health bar
    glColor3f(0.0, 0.2, 0.0); // Green color
    glBegin(GL_QUADS);
    for (int i = 0; i < lives; i++) {
        glVertex2f(10 + i * 30, windowHeight - 30);
        glVertex2f(30 + i * 30, windowHeight - 30);
        glVertex2f(30 + i * 30, windowHeight - 10);
        glVertex2f(10 + i * 30, windowHeight - 10);
    }
    glEnd();
}
void renderScoreCounter() {
    // Render the score counter
    glColor3f(0.0, 0.0, 0.0); // Black color
    glRasterPos2f(windowWidth - 100, windowHeight - 20);
    std::string scoreText = "Score: " + std::to_string(score);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}
void renderTimer() {
    // Render the timer
    glColor3f(0.0, 0.0, 0.0); // Black color
    glRasterPos2f(windowWidth - 200, windowHeight - 20);
    std::string timerText = "Time: " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
    for (char c : timerText) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}
void updateTimer(int value) {
    if (timerRunning) {
        if (seconds == 0) {
            if (minutes == 0) {
                // Timer has reached zero, handle game over logic here
                // For now, we just stop the timer
                timerRunning = false;
            }
            else {
                minutes--;
                seconds = 59;
            }
        }
        else {
            seconds--;
        }
    }
    glutPostRedisplay(); // Request a redisplay
    glutTimerFunc(1000, updateTimer, 0); // Set the timer for the next second
}

void renderWinnerMessage() {
    // Render the game over message
    glColor3f(0.0, 0.0, 1.0); // Red color for game over text
    glRasterPos2f(windowWidth / 2 - 50, windowHeight / 2);
    std::string gameOverText = "WINNER WINNER CHICKEN DINNER!!!!!";
    for (char c : gameOverText) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}
float distance(float x1, float y1, float x2, float y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
void checkCollisions() {
    // Check for collisions with collectables
    for (int i = 0; i < 6; i++) {
        float collectableDist = distance(playerX, playerY, collectables[i].x, collectables[i].y);
        if (collectableDist < 50.0) { // Adjust the threshold as needed
            score++; // Increase the score when a collectable is collected
            // Remove the collectable from the screen by resetting its position
            collectables[i].x = static_cast<float>(rand() % gameWidth - 10);
            collectables[i].y = static_cast<float> (rand() % gameHeight - 10);
        }
    }

    // Check for collisions with power-ups
    for (int i = 0; i < 2; i++) {
        float powerUpDist = distance(playerX, playerY, powerUps[i].x, powerUps[i].y);
        if (powerUpDist < 50.0) { // Adjust the threshold as needed
            // Activate the power-up and set its duration
            powerUps[i].x = static_cast<float>(rand() % gameWidth - 10);
            powerUps[i].y = static_cast<float> (rand() % gameHeight - 10);
            powerUps[i].active = true;
            powerUps[i].duration = 50; // Set the duration as needed

            // Apply the effect of the power-up
            if (powerUps[i].type == 1) {
                // Increase player speed by 5
                playerSpeed += 10;
            }
            else if (powerUps[i].type == 2) {
                // Increase player speed by 10
                playerSpeed += 40;
            }

        }

    }
    for (int i = 0; i < 4; i++) {
        float obstacleDist = distance(playerX, playerY, obstacles[i].x, obstacles[i].y);
        if (obstacleDist < 50.0) { // Adjust the threshold as needed
            lives--; // Example: Decrease player health
            playerX = gameWidth / 2;
            playerY = gameHeight / 2;

        }
    }
    if (distance(playerX, playerY, goalObject.x, goalObject.y) < 75.0) {
        winner = true;
        gameOver = true;
    }

}
void renderPlayer() {
    // Save the current modelview matrix
    glPushMatrix();

    // Translate to the player's position
    glTranslatef(playerX, playerY, 0.0);

    // Rotate the player according to playerRotation
    glRotatef(playerRotation, 0.0, 0.0, 1.0);

    // Draw the player's shapes stacked on top of each other
    glColor3f(0.0, 0.0, 1.0); // Blue color

    // Rectangle (rotated by 90 degrees)
    glRotatef(90.0, 0.0, 0.0, 1.0); // Rotate by 90 degrees
    glBegin(GL_QUADS);
    glVertex2f(-30, -15);
    glVertex2f(-30, 15);
    glVertex2f(-15, 15);
    glVertex2f(-15, -15);
    glEnd();

    // Circle
    int numSegments = 50;
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0 * M_PI * i / numSegments;
        float x = 0 + 15 * cos(theta);
        float y = -30 + 15 * sin(theta);
        glVertex2f(x, y);
    }
    glEnd();

    // Triangle
    glBegin(GL_TRIANGLES);
    glVertex2f(-15, 0);
    glVertex2f(15, 0);
    glVertex2f(0, 15);
    glEnd();

    // Square
    glBegin(GL_QUADS);
    glVertex2f(-15, -15);
    glVertex2f(15, -15);
    glVertex2f(15, 0);
    glVertex2f(-15, 0);
    glEnd();

    // Restore the previous modelview matrix
    glPopMatrix();

}
bool checkCollisionWithObstacles(float x, float y) {
    for (int i = 0; i < 4; i++) {
        float obstacleDist = distance(x, y, obstacles[i].x, obstacles[i].y);
        if (obstacleDist < 100.0) {
            return true; // Position collides with an obstacle
        }
    }
    return false; // Position does not collide with any obstacle
}
float globalCollectableRotationAngle = 0.0f;
float globalCollectableRotationSpeed = 30.0f;

void updateCollectableRotations() {
    // Update the global rotation angle based on the rotation speed
    globalCollectableRotationAngle += globalCollectableRotationSpeed;
}

void renderBatmanLogoCollectable(float x, float y, float rotationAngle) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);

    // Apply the rotation transformation
    glRotatef(rotationAngle, 0.0, 0.0, 1.0);

    // Draw the basic structure of the Batman logo
    glColor3f(0.0, 1.0, 0.0); // Black color for the logo
    glBegin(GL_TRIANGLES);
    // First part of the logo
    glVertex2f(0, 20);
    glVertex2f(10, 0);
    glVertex2f(20, 20);

    // Second part of the logo
    glVertex2f(0, 20);
    glVertex2f(-10, 0);
    glVertex2f(-20, 20);
    glEnd();

    // Draw two circles for the Batman logo
    int numSegments = 100;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0, 1.0, 0.0); // Black color
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0 * M_PI * i / numSegments;
        float bx = 10 * cos(theta);
        float by = 15 * sin(theta);
        glVertex2f(bx, by);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0, 1.0, 0.0); // Black color
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0 * M_PI * i / numSegments;
        float bx = -10 * cos(theta);
        float by = 15 * sin(theta);
        glVertex2f(bx, by);
    }
    glEnd();

    glPopMatrix();
}

void renderCollectables() {
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glTranslatef(collectables[i].x, collectables[i].y, 0.0);

        renderBatmanLogoCollectable(0, 0, globalCollectableRotationAngle);

        glPopMatrix();
    }

}
void initCollectables() {
    srand(static_cast<unsigned>(time(nullptr)));
    int collectablesGenerated = 0;
    for (int i = 0; i < 7; i++) {
        // Initialize collectable position randomly
        float x;
        float y;
        do {
            x = randomFloat(200, gameWidth);
            y = randomFloat(200, gameHeight);
            // Check if the power-up position is not on top of any obstacle
        } while (checkCollisionWithObstacles(x, y));

        collectables[i].x = x;
        collectables[i].y = y;
        collectablesGenerated++;

        // Check if the collectable position is not on top of any obstacle

    }
}



void renderPowerUp(int type, float x, float y, float rotationAngle) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(rotationAngle, 0.0, 0.0, 1.0); // Rotate around the Z-axis
    glColor3f(0.0, 0.0, 0.5); // Dark blue color for power-ups

    if (type == 1) {
        // Render the first type of power-up (consisting of four triangles)
        glBegin(GL_TRIANGLES);

        // Triangle 1
        glVertex2f(-10, -10);
        glVertex2f(0, 10);
        glVertex2f(10, -10);

        // Triangle 2
        glVertex2f(-10, -10);
        glVertex2f(0, 0);
        glVertex2f(10, -10);

        // Triangle 3
        glVertex2f(-10, -20);
        glVertex2f(0, -5);
        glVertex2f(10, -10);

        // Triangle 4
        glVertex2f(-10, -30);
        glVertex2f(0, -10);
        glVertex2f(10, -30);

        glEnd();
    }
    else if (type == 2) {
        // Render the second type of power-up (consisting of four different triangles)
        glBegin(GL_TRIANGLES);
        // Triangle 1
        glVertex2f(-15, -10);  // Scale by a factor of 2
        glVertex2f(0, 10);    // Scale by a factor of 2
        glVertex2f(15, -10);   // Scale by a factor of 2

        // Triangle 2
        glVertex2f(-15, -40);  // Scale by a factor of 2
        glVertex2f(0, 0);
        glVertex2f(15, -40);   // Scale by a factor of 2

        // Triangle 3
        glVertex2f(-15, -60);  // Scale by a factor of 2
        glVertex2f(0, -20);   // Scale by a factor of 2
        glVertex2f(15, -60);   // Scale by a factor of 2

        // Triangle 4
        glVertex2f(-15, -60);  // Scale by a factor of 2
        glVertex2f(0, -40);   // Scale by a factor of 2
        glVertex2f(15, -60);   // Scale by a factor of 2

        glEnd();
    }

    glPopMatrix();
}
void initPowerUps() {
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < 2; i++) {
        float x;
        float y;
        // Initialize power-up position randomly
        do {
            x = randomFloat(200, gameWidth);
            y = randomFloat(200, gameHeight);
            // Check if the power-up position is not on top of any obstacle
        } while (checkCollisionWithObstacles(x, y));
        powerUps[i].x = x;
        powerUps[i].y = y;
        powerUps[i].active = false;
        powerUps[i].type = (i + 1); // Assign power-up types (1 and 2)
        powerUps[i].duration = 0;   // Set initial duration to 0
    }

}
void updatePowerUpTimer() {
    for (int i = 0; i < 2; i++) {
        if (powerUps[i].active) {
            powerUps[i].duration--;

            if (powerUps[i].duration <= 0) {
                // Deactivate the power-up when its duration reaches zero
                powerUps[i].active = false;
                // Reset player speed to its original value (you need to keep track of the originalPlayerSpeed)
                playerSpeed = originalPlayerSpeed;
            }
        }
    }
}

void renderObstacles() {
    glColor3f(0.0, 0.0, 0.0); // Black color for obstacles
    for (int i = 0; i < 4; i++) {
        if (obstacles[i].active) {
            glPushMatrix();
            glTranslatef(obstacles[i].x, obstacles[i].y, 0.0);

            // Render the first black circle
            glBegin(GL_TRIANGLE_FAN); // Use GL_TRIANGLE_FAN to render circles
            for (int j = 0; j < 360; j++) {
                float angle = j * 3.14159265f / 180.0f;
                float x = cos(angle) * 15; // Adjust the radius as needed
                float y = sin(angle) * 15; // Adjust the radius as needed
                glVertex2f(x, y);
            }
            glEnd();

            // Render the second black circle
            glTranslatef(30.0, 0.0, 0.0); // Move to the right to render the second circle
            glBegin(GL_TRIANGLE_FAN);
            for (int j = 0; j < 360; j++) {
                float angle = j * 3.14159265f / 180.0f;
                float x = cos(angle) * 15; // Adjust the radius as needed
                float y = sin(angle) * 15; // Adjust the radius as needed
                glVertex2f(x, y);
            }
            glEnd();

            glPopMatrix();
        }
    }
}
void initObstacles() {
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < 4; i++) {
        obstacles[i].x = randomFloat(0, gameWidth);
        obstacles[i].y = randomFloat(0, gameHeight);
        obstacles[i].active = true; // Set them as active initially.
    }
}
void drawSquare() {
    glBegin(GL_QUADS);
    glVertex2f(-10, -10);
    glVertex2f(10, -10);
    glVertex2f(10, 10);
    glVertex2f(-10, 10);
    glEnd();
}
void drawTriangle() {
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 10);
    glVertex2f(-10, -10);
    glVertex2f(10, -10);
    glEnd();
}
void drawCircle() {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159265f / 180.0f;
        float x = cos(angle) * 10;
        float y = sin(angle) * 10;
        glVertex2f(x, y);
    }
    glEnd();
}
void RenderGoalObject(const CustomObject& obj) {
    glPushMatrix();
    glTranslatef(obj.x, obj.y, 0.0);

    // Draw 5 different shapes
    glColor3f(1.0, 0.0, 0.0); // Red color
    drawSquare();

    glTranslatef(20, 0, 0); // Move to the next position
    glColor3f(1.0, 0.0, 0.0); // Green color
    drawTriangle();

    glTranslatef(20, 0, 0); // Move to the next position
    glColor3f(1.0, 0.0, 0.0); // Blue color
    drawCircle();

    glTranslatef(20, 0, 0); // Move to the next position
    glColor3f(1.0, 0.0, 0.0); // Yellow color
    drawSquare();

    glTranslatef(20, 0, 0); // Move to the next position
    glColor3f(1.0, 0.0, 0.0); // Cyan color
    drawTriangle();

    glPopMatrix();
}
void initGoalObject() {
    goalObject.x = 100; // Adjust the range as needed
    goalObject.y = 100;// Adjust the range as needed
}


void renderGameOverMessage() {
    // Render the game over message
    glColor3f(1.0, 0.0, 0.0); // Red color for game over text
    glRasterPos2f(windowWidth / 2 - 50, windowHeight / 2);
    std::string gameOverText = "Game Over, you lost";
    for (char c : gameOverText) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}



void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': // Move player up
        playerY += playerSpeed;
        playerRotation = 0.0;
        break;
    case 's': // Move player down
        playerY -= playerSpeed;
        playerRotation = 180.0;
        break;
    case 'a': // Move player left
        playerX -= playerSpeed;
        playerRotation = 90.0;
        break;
    case 'd': // Move player right
        playerX += playerSpeed;
        playerRotation = -90.0;
        break;
    }

    // Ensure the player stays within the window bounds
    if (playerX < 0) playerX = 0;
    if (playerX > gameWidth) {
        playerX = gameWidth;lives--;
        playerX = gameWidth / 2;
        playerY = gameHeight / 2;
    }
    if (playerY < 0) playerY = 0;
    if (playerY > gameHeight) {
        playerY = gameHeight;
        lives--;
        playerX = gameWidth / 2;
        playerY = gameHeight / 2;
    }

    glutPostRedisplay();
}
void Display() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(0.0, 0.0, 0.0); // Black color for the border
    glBegin(GL_LINE_LOOP);
    glVertex2f(4, 4);  // Top-left corner of the border
    glVertex2f(gameWidth + 4, 4); // Top-right corner of the border
    glVertex2f(gameWidth + 4, gameHeight + 4); // Bottom-right corner of the border
    glVertex2f(4, gameHeight + 4); // Bottom-left corner of the border
    glEnd();
    glEnable(GL_TEXTURE_2D); // Enable texture mapping
    glDisable(GL_TEXTURE_2D); // Disable texture mapping

    if (!gameOver) {
        renderHealthBar();
        renderScoreCounter();
        renderTimer();
        renderCollectables();
        renderObstacles();
        renderPlayer();
        updateCollectableRotations();
        RenderGoalObject(goalObject);
        for (int i = 0; i < 2; i++) {
            renderPowerUp(powerUps[i].type, powerUps[i].x, powerUps[i].y, globalCollectableRotationAngle);
        }
        updatePowerUpTimer();
        checkCollisions();
        if ((minutes == 0 && seconds == 0) || lives == 0) {
            gameOver = true;
        }
    }
    else
    {
        if (winner) { renderWinnerMessage(); }
        else { renderGameOverMessage(); }
    }

    glFlush();
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL Window");
    glutDisplayFunc(Display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, updateTimer, 0);
    initObstacles();
    initCollectables();
    initPowerUps();
    initGoalObject();
    glutMainLoop();

    return 0;
}