#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <iostream>
using namespace std;

// Function prototypes
void drawCircle(float x, float y, float radius, float r, float g, float b);
void drawText(float x, float y, const char* text);
void displayReactionFormula(int reaction);
void simulateReaction(int reaction);
void drawWaterDrop(float x, float y);
void drawExplosion(float centerX, float centerY, int reactionIndex);
void drawBeaker(float x, float y, const char* label);
void keyboard(unsigned char key, int x, int y);
void displayDashboard();

// Global variables for animation
float h2x = -0.6f, h2y = 0.2f;
float o2x = 0.6f, o2y = 0.2f;
float productX = 0.0f, productY = -0.2f;
bool reactionComplete[3] = { false, false, false };
bool explosionVisible[3] = { false, false, false };
bool waterDropVisible[3] = { false, false, false };
float animationSpeed = 0.005f; // Smoother animation speed
float glowIntensity = 0.5f; // For glowing effect
bool increasingGlow = true;
float waterDropY[3] = { -0.2f, -0.2f, -0.2f };  
float explosionRadius[3] = { 0.0f, 0.0f, 0.0f };
bool gasesReleased[3] = { false, false, false }; // Track if gases have been released
bool gasesClose[3] = { false, false, false }; // Track if gases are close enough for reaction
int currentReaction = -1; // Track the current reaction

// Function to draw a circle
void drawCircle(float x, float y, float radius, float r, float g, float b) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(r, g, b);
    glVertex2f(x, y);
    for (int i = 0; i <= 360; i++) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

// Function to draw glowing circles
void drawGlowingCircle(float x, float y, float radius, float r, float g, float b) {
    for (float i = radius; i <= radius + 0.05f; i += 0.01f) {
        glColor4f(r, g, b, glowIntensity * (1.0f - (i - radius) / 0.05f));
        drawCircle(x, y, i, r, g, b);
    }
}

// Function to draw text
void drawText(float x, float y, const char* text) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Function to draw a water drop
void drawWaterDrop(float x, float y) {
    glColor3f(0.0f, 0.3f, 1.0f);
    drawCircle(x, y, 0.06f, 0.0f, 0.3f, 1.0f);
    drawCircle(x, y + 0.03f, 0.02f, 1.0f, 1.0f, 1.0f);
}

// Function to draw an explosion
void drawExplosion(float centerX, float centerY, int reactionIndex) {
    for (float i = explosionRadius[reactionIndex]; i <= explosionRadius[reactionIndex] + 0.05f; i += 0.02f) {
        glColor4f(1.0f, 0.8f - (i - explosionRadius[reactionIndex]) / 0.1f, 0.0f, 1.0f - (i - explosionRadius[reactionIndex]) / 0.1f);
        drawCircle(centerX, centerY, i, 1.0f, 0.5f, 0.0f);
    }
}

// Function to draw a beaker
void drawBeaker(float x, float y, const char* label) {
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.8f, 0.8f);
    glVertex2f(x - 0.2f, y - 0.5f);
    glVertex2f(x + 0.2f, y - 0.5f);
    glVertex2f(x + 0.15f, y + 0.5f);
    glVertex2f(x - 0.15f, y + 0.5f);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 0.2f, y - 0.5f);
    glVertex2f(x + 0.2f, y - 0.5f);
    glVertex2f(x + 0.15f, y + 0.5f);
    glVertex2f(x - 0.15f, y + 0.5f);
    glEnd();

    drawText(x - 0.1f, y + 0.55f, label);
}

// Function to display reaction formulas
void displayReactionFormula(int reaction) {
    glColor3f(1.0f, 1.0f, 1.0f);
    if (reaction == 0) {
        drawText(-0.4f, 0.85f, "Chemical Reaction Simulation");
        drawText(-0.5f, 0.75f, "Reaction: 2H2 + O2 -> 2H2O");
        drawText(-0.5f, 0.65f, "Hydrogen + Oxygen -> Water");
    }
    else if (reaction == 1) {
        drawText(-0.4f, 0.85f, "Chemical Reaction Simulation");
        drawText(-0.5f, 0.75f, "Reaction: C + O2 -> CO2");
        drawText(-0.5f, 0.65f, "Carbon + Oxygen -> Carbon Dioxide");
    }
    else if (reaction == 2) {
        drawText(-0.4f, 0.85f, "Chemical Reaction Simulation");
        drawText(-0.5f, 0.75f, "Reaction: CH4 + 2O2 -> CO2 + 2H2O");
        drawText(-0.5f, 0.65f, "Methane + Oxygen -> Carbon Dioxide + Water");
    }
}

// Function to simulate reactions
void simulateReaction(int reaction) {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen
    displayReactionFormula(reaction); // Draw the formula text

    // Background gradient
    glBegin(GL_QUADS);
    if (reaction == 0) { // H2 + O2 -> H2O
        glColor3f(0.1f, 0.1f, 0.3f); // Dark blue
        glVertex2f(-1.0f, -1.0f);
        glColor3f(0.2f, 0.4f, 0.8f); // Light blue
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glColor3f(0.1f, 0.1f, 0.3f); // Dark blue
        glVertex2f(1.0f, -1.0f);
    } else if (reaction == 1) { // C + O2 -> CO2
        glColor3f(1.0f, 0.0f, 1.0f); // Magenta
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
    } else if (reaction == 2) { // CH4 + 2O2 -> CO2 + 2H2O
        glColor3f(0.0f, 1.0f, 0.0f); // Green
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
    }
    glEnd();

    // Draw the beakers
    if (reaction == 0) { // H2 + O2
        drawBeaker(-0.5f, 0.0f, "H2");
        drawBeaker(0.5f, 0.0f, "O2");
    } else if (reaction == 1) { // C + O2
        drawBeaker(-0.5f, 0.0f, "C");
        drawBeaker(0.5f, 0.0f, "O2");
    } else if (reaction == 2) { // CH4 + 2O2
        drawBeaker(-0.5f, 0.0f, "CH4");
        drawBeaker(0.5f, 0.0f, "O2");
    }

    // Reaction logic
    if (reaction == 0) { // H2 + O2 -> H2O
        if (gasesReleased[0] && !gasesClose[0]) {
            if (h2y < 0.2f) {
                h2y += animationSpeed;
                o2y += animationSpeed;
            } else {
                if (h2x < -0.1f) {
                    h2x += animationSpeed;
                    o2x -= animationSpeed;
                } else {
                    gasesClose[0] = true;
                }
            }
        }

        if (gasesReleased[0]) {
            drawGlowingCircle(h2x, h2y, 0.05f, 0.0f, 1.0f, 1.0f); // H2
            drawText(h2x - 0.05f, h2y + 0.1f, "H2");
            drawGlowingCircle(h2x - 0.2f, h2y, 0.05f, 0.0f, 1.0f, 1.0f); // H2
            drawText(h2x - 0.25f, h2y + 0.1f, "H2");
            drawGlowingCircle(o2x, o2y, 0.07f, 1.0f, 0.0f, 0.0f); // O2
            drawText(o2x - 0.05f, o2y + 0.1f, "O2");
        }

        if (explosionVisible[0]) {
            explosionRadius[0] += 0.02f;
            if (explosionRadius[0] > 0.3f) {
                explosionVisible[0] = false;
                reactionComplete[0] = true;
                waterDropVisible[0] = true;
            }
            drawExplosion(0.0f, 0.2f, 0);
        }

        if (reactionComplete[0]) {
            drawGlowingCircle(productX, productY, 0.06f, 0.0f, 0.0f, 1.0f); // H2O
            drawText(productX - 0.05f, productY + 0.1f, "H2O");
            drawGlowingCircle(productX + 0.2f, productY, 0.06f, 0.0f, 0.0f, 1.0f); // H2O
            drawText(productX + 0.15f, productY + 0.1f, "H2O");
            drawText(-0.2f, -0.7f, "Reaction Complete!");

            if (waterDropVisible[0]) {
                if (waterDropY[0] > -0.4f) {
                    waterDropY[0] -= animationSpeed * 0.5f;
                }
                drawWaterDrop(0.0f, waterDropY[0]);
            }
        }
    } else if (reaction == 1) { // C + O2 -> CO2
        if (gasesReleased[1] && !gasesClose[1]) {
            if (h2y < 0.2f) {
                h2y += animationSpeed;
                o2y += animationSpeed;
            } else {
                if (h2x < -0.1f) {
                    h2x += animationSpeed;
                    o2x -= animationSpeed;
                } else {
                    gasesClose[1] = true;
                }
            }
        }

        if (gasesReleased[1]) {
            drawGlowingCircle(h2x, h2y, 0.05f, 0.2f, 0.2f, 0.2f); // Carbon
            drawText(h2x - 0.05f, h2y + 0.1f, "C");
            drawGlowingCircle(o2x, o2y, 0.07f, 1.0f, 0.0f, 0.0f); // O2
            drawText(o2x - 0.05f, o2y + 0.1f, "O2");
        }

        if (explosionVisible[1]) {
            explosionRadius[1] += 0.02f;
            if (explosionRadius[1] > 0.3f) {
                explosionVisible[1] = false;
                reactionComplete[1] = true;
            }
            drawExplosion(0.0f, 0.2f, 1);
        }

        if (reactionComplete[1]) {
            drawGlowingCircle(productX, productY, 0.06f, 0.0f, 1.0f, 0.0f); // CO2
            drawText(productX - 0.05f, productY + 0.1f, "CO2");
            drawText(-0.2f, -0.7f, "Reaction Complete!");
        }
    } else if (reaction == 2) { // CH4 + 2O2 -> CO2 + 2H2O
        if (gasesReleased[2] && !gasesClose[2]) {
            if (h2y < 0.2f) {
                h2y += animationSpeed;
                o2y += animationSpeed;
            } else {
                if (h2x < -0.1f) {
                    h2x += animationSpeed;
                    o2x -= animationSpeed;
                } else {
                    gasesClose[2] = true;
                }
            }
        }

        if (gasesReleased[2]) {
            drawGlowingCircle(h2x, h2y, 0.05f, 0.0f, 0.0f, 1.0f); // CH4 (blue color)
            drawText(h2x - 0.05f, h2y + 0.1f, "CH4");
            drawGlowingCircle(o2x, o2y, 0.07f, 1.0f, 0.0f, 0.0f); // O2
            drawText(o2x - 0.05f, o2y + 0.1f, "O2");
        }

        if (explosionVisible[2]) {
            explosionRadius[2] += 0.02f;
            if (explosionRadius[2] > 0.3f) {
                explosionVisible[2] = false;
                reactionComplete[2] = true;
            }
            drawExplosion(0.0f, 0.2f, 2);
        }

        if (reactionComplete[2]) {
            drawGlowingCircle(productX, productY, 0.06f, 0.0f, 0.0f, 1.0f); // CO2
            drawText(productX - 0.05f, productY + 0.1f, "CO2");
            drawGlowingCircle(productX + 0.2f, productY, 0.06f, 0.0f, 0.0f, 1.0f); // H2O
            drawText(productX + 0.15f, productY + 0.1f, "H2O");
            drawText(-0.2f, -0.7f, "Reaction Complete!");
        }
    }

    glutSwapBuffers();
}


// Timer function for animation
void timer(int) {
    if (increasingGlow) {
        glowIntensity += 0.01f;
        if (glowIntensity >= 1.0f) {
            increasingGlow = false;
        }
    }
    else {
        glowIntensity -= 0.01f;
        if (glowIntensity <= 0.5f) {
            increasingGlow = true;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // 16 ms for smoother 60 FPS
}

// Keyboard input handling
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1': // Select reaction 1 (H2 + O2 -> H2O)
        currentReaction = 0;
        break;
    case '2': // Select reaction 2 (C + O2 -> CO2)
        currentReaction = 1;
        break;
    case '3': // Select reaction 3 (CH4 + 2O2 -> CO2 + 2H2O)
        currentReaction = 2;
        break;
    case 'q': // Release gases for the current reaction (1st reaction)
        if (currentReaction == 0 && !gasesReleased[0]) {
            gasesReleased[0] = true;
        }
        break;
    case 'w': // Trigger reaction for the current reaction (1st reaction)
        if (currentReaction == 0 && gasesClose[0] && !reactionComplete[0]) {
            explosionVisible[0] = true;
        }
        break;
    case 'e': // Restart the simulation for the current reaction (1st reaction)
        if (currentReaction == 0) {
            gasesReleased[0] = false;
            reactionComplete[0] = false;
            explosionVisible[0] = false;
            explosionRadius[0] = 0.0f;
        }
        break;
    case 'a': // Release gases for the current reaction (2nd reaction)
        if (currentReaction == 1 && !gasesReleased[1]) {
            gasesReleased[1] = true;
        }
        break;
    case 's': // Trigger reaction for the current reaction (2nd reaction)
        if (currentReaction == 1 && gasesClose[1] && !reactionComplete[1]) {
            explosionVisible[1] = true;
        }
        break;
    case 't': // Restart the simulation for the current reaction (2nd reaction)
        if (currentReaction == 1) {
            gasesReleased[1] = false;
            reactionComplete[1] = false;
            explosionVisible[1] = false;
            explosionRadius[1] = 0.0f;
        }
        break;
    case 'z': // Release gases for the current reaction (3rd reaction)
        if (currentReaction == 2 && !gasesReleased[2]) {
            gasesReleased[2] = true;
        }
        break;
    case 'x': // Trigger reaction for the current reaction (3rd reaction)
        if (currentReaction == 2 && gasesClose[2] && !reactionComplete[2]) {
            explosionVisible[2] = true;
        }
        break;
    case 'c': // Restart the simulation for the current reaction (3rd reaction)
        if (currentReaction == 2) {
            gasesReleased[2] = false;
            reactionComplete[2] = false;
            explosionVisible[2] = false;
            explosionRadius[2] = 0.0f;
        }
        break;
    case 'd': // Go to dashboard
        currentReaction = -1; // Reset to dashboard
        break;
    case 'p': // Return to previous reaction
        currentReaction = -1; // Reset to dashboard
        break;
    }
}

// Function to display the dashboard
void displayDashboard() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.5f, 0.0f); // Orange background for the dashboard
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);
    glEnd();

    drawText(-0.4f, 0.75f, "Chemical Reaction Dashboard");
    drawText(-0.5f, 0.55f, "Press 1: H2 + O2 -> H2O");
    drawText(-0.5f, 0.45f, "Press 2: C + O2 -> CO2");
    drawText(-0.5f, 0.35f, "Press 3: CH4 + 2O2 -> CO2 + 2H2O");
   
    drawText(-0.5f, 0.15f, "Press d: Go to Dashboard");
    drawText(-0.5f, 0.05f, "Press p: Return to Previous");
    glutSwapBuffers();
}

// Display function
void display() {
    if (currentReaction == -1) {
        displayDashboard();
    } else {
        simulateReaction(currentReaction);
    }
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Chemical Reaction Simulation Dashboard");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    return 0;
} 