#include <GL/glut.h>

// Camera
float camX = 0.0f;
float camY = 3.5f;
float camZ = 11.0f;

// Alignment constants
const float TAN_FLOOR_TOP = -0.425f;
const float WALL_HEIGHT   = 3.5f;
const float WALL_CENTER_Y = TAN_FLOOR_TOP + WALL_HEIGHT / 2.0f;

// ---------- Utility ----------
void drawCube(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// ---------- Ground / Tiles ----------
void drawGround()
{
    // Concrete base
    glColor3f(0.75f, 0.75f, 0.75f);
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);
    drawCube(20.0f, 0.1f, 20.0f);
    glPopMatrix();

    // Tan tile (reference floor)
    glColor3f(0.85f, 0.7f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, -0.45f, 2.0f);
    drawCube(6.0f, 0.05f, 4.0f);
    glPopMatrix();
}

// ---------- Brick Walls ----------
void drawBrickWalls()
{
    glColor3f(0.75f, 0.25f, 0.25f);

    // Left wall (50% wider)
    glPushMatrix();
    glTranslatef(-3.4f, WALL_CENTER_Y, 0.0f);
    drawCube(4.5f, WALL_HEIGHT, 1.5f);
    glPopMatrix();

    // Right wall
    glPushMatrix();
    glTranslatef(2.8f, WALL_CENTER_Y, 0.0f);
    drawCube(3.0f, WALL_HEIGHT, 1.5f);
    glPopMatrix();
}

// ---------- Window Tower ----------
void drawWindowTower()
{
    // Same height as brick walls
    float sectionHeight = WALL_HEIGHT / 3.0f;
    float baseCenterY   = TAN_FLOOR_TOP + sectionHeight / 2.0f;

    // Lower gray
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, baseCenterY, 0.0f);
    drawCube(2.4f, sectionHeight, 1.2f);
    glPopMatrix();

    // Light blue
    glColor3f(0.6f, 0.85f, 0.95f);
    glPushMatrix();
    glTranslatef(0.0f, baseCenterY + sectionHeight, 0.0f);
    drawCube(2.4f, sectionHeight, 1.2f);
    glPopMatrix();

    // Dark blue
    glColor3f(0.15f, 0.25f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, baseCenterY + 2.0f * sectionHeight, 0.0f);
    drawCube(2.4f, sectionHeight, 1.2f);
    glPopMatrix();
}

// ---------- Entrance ----------
void drawEntrance()
{
    float doorCenterY = TAN_FLOOR_TOP + 0.45f;

    // Recess
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, doorCenterY, 0.55f);
    drawCube(1.5f, 0.9f, 0.5f);
    glPopMatrix();

    // Glass doors
    glColor3f(0.75f, 0.9f, 1.0f);

    glPushMatrix();
    glTranslatef(-0.35f, doorCenterY, 0.85f);
    drawCube(0.45f, 0.85f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.35f, doorCenterY, 0.85f);
    drawCube(0.45f, 0.85f, 0.05f);
    glPopMatrix();
}

// ---------- Axis ----------
void drawAxis()
{
    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(4, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 4, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 4);

    glEnd();
}

// ---------- Display ----------
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
        camX, camY, camZ,
        0.0f, 1.5f, 0.0f,
        0.0f, 1.0f, 0.0f
    );

    drawAxis();
    drawGround();
    drawBrickWalls();
    drawWindowTower();
    drawEntrance();

    glutSwapBuffers();
}

// ---------- Init ----------
void init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
}

// ---------- Projection ----------
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// ---------- Main ----------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Brick-to-Floor Aligned Scene");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
