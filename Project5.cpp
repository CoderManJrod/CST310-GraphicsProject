#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Camera
float camX = 0.0f;
float camY = 3.5f;
float camZ = 11.0f;

// Alignment constants
const float TAN_FLOOR_TOP = -0.425f;
const float WALL_HEIGHT   = 3.5f;
const float WALL_CENTER_Y = TAN_FLOOR_TOP + WALL_HEIGHT / 2.0f;

// Shader program and uniform locations
unsigned int shaderProgram;
unsigned int VAO, VBO, EBO;
unsigned int modelLoc, viewLoc, projLoc, objectColorLoc, alphaLoc, lightPosLoc, viewPosLoc, lightColorLoc;

// [Vertex/Index arrays, drawCube(), createShaders(), drawGround(), drawBrickWalls(), drawWindowTower(), drawEntrance() remain unchanged]

// Camera movement speed
float moveSpeed = 0.3f;

// WASD + QE camera movement
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'w': // forward along Z
            camZ -= moveSpeed;
            break;
        case 's': // backward along Z
            camZ += moveSpeed;
            break;
        case 'a': // left along X
            camX -= moveSpeed;
            break;
        case 'd': // right along X
            camX += moveSpeed;
            break;
        case 'q': // down
            camY -= moveSpeed;
            break;
        case 'e': // up
            camY += moveSpeed;
            break;
        case 27: // ESC key to exit
            exit(0);
            break;
    }

    // Clamp camera values to avoid flipping
    if (camY < 0.5f) camY = 0.5f;
    if (camY > 20.0f) camY = 20.0f;
    if (camZ < 1.0f) camZ = 1.0f;
    if (camZ > 50.0f) camZ = 50.0f;

    glutPostRedisplay();
}

// Cube mesh data (positions + normals)
float vertices[] = {
    // positions          // normals
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,

     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f
};

unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
};

// Utility to draw scaled cube using current modelview matrix
void drawCube(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    float modelMat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelMat);
    glPopMatrix();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Create and compile shaders
void createShaders() {
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
        "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
        "in vec3 Normal;\n"
        "in vec3 FragPos;\n"
        "uniform vec3 lightPos;\n"
        "uniform vec3 viewPos;\n"
        "uniform vec3 lightColor;\n"
        "uniform vec3 objectColor;\n"
        "uniform float alpha;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    float ambientStrength = 0.35;\n"
        "    vec3 ambient = ambientStrength * lightColor;\n"
        "    vec3 norm = normalize(Normal);\n"
        "    vec3 lightDir = normalize(lightPos - FragPos);\n"
        "    float diff = max(dot(norm, lightDir), 0.0);\n"
        "    vec3 diffuse = diff * lightColor;\n"
        "    float specularStrength = 0.8;\n"
        "    vec3 viewDir = normalize(viewPos - FragPos);\n"
        "    vec3 reflectDir = reflect(-lightDir, norm);\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);\n"
        "    vec3 specular = specularStrength * spec * lightColor;\n"
        "    vec3 result = (ambient + diffuse + specular) * objectColor;\n"
        "    FragColor = vec4(result, alpha);\n"
        "}\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation failed: %s\n", infoLog);
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader program linking failed: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// ---------- Ground ----------
void drawGround() {
    // Concrete base
    glUniform3f(objectColorLoc, 0.75f, 0.75f, 0.75f);
    glUniform1f(alphaLoc, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);
    drawCube(20.0f, 0.1f, 20.0f);
    glPopMatrix();

    // Tan tiled area
    glPushMatrix();
    glTranslatef(0.0f, -0.45f, 2.0f);
    float width = 6.0f;
    float depth = 4.0f;
    float height = 0.05f;

    // Mortar
    glUniform3f(objectColorLoc, 0.7f, 0.7f, 0.7f);
    drawCube(width, height, depth);

    // Tiles with slight color variation
    float tileW = 0.5f;
    float tileD = 0.5f;
    int cols = (int)(width / tileW);
    int rows = (int)(depth / tileD);
    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows; r++) {
            float tx = c * tileW - width / 2.0f + tileW / 2.0f;
            float tz = r * tileD - depth / 2.0f + tileD / 2.0f;
            float var = ((rand() % 10) - 5) / 100.0f;
            glUniform3f(objectColorLoc, 0.85f + var, 0.7f + var, 0.5f + var);
            glPushMatrix();
            glTranslatef(tx, 0.001f, tz);
            drawCube(tileW * 0.98f, height, tileD * 0.98f);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

// ---------- Brick Walls (detailed bricks) ----------
void drawBrickWall(float x_pos, float width, float height, float depth) {
    glPushMatrix();
    glTranslatef(x_pos, WALL_CENTER_Y, 0.0f);

    // Mortar
    glUniform3f(objectColorLoc, 0.6f, 0.6f, 0.6f);
    drawCube(width, height, depth);

    // Bricks
    float brickW = 0.4f;
    float brickH = 0.15f;
    int rows = (int)(height / brickH) + 1;
    int cols = (int)(width / brickW) + 1;
    for (int r = 0; r < rows; r++) {
        float offset = (r % 2 == 0) ? 0.0f : brickW / 2.0f;
        for (int c = 0; c < cols; c++) {
            float bx = c * brickW + offset - width / 2.0f;
            if (bx + brickW / 2.0f > width / 2.0f) continue;
            float by = r * brickH - height / 2.0f;
            float var = ((rand() % 20) - 10) / 100.0f;
            glUniform3f(objectColorLoc, 0.75f + var, 0.25f + var, 0.25f + var);
            glPushMatrix();
            glTranslatef(bx, by, 0.01f);
            drawCube(brickW * 0.98f, brickH * 0.98f, depth * 0.99f);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

void drawBrickWalls() {
    drawBrickWall(-3.4f, 4.5f, WALL_HEIGHT, 1.5f);   // Left (wider)
    drawBrickWall(2.8f, 3.0f, WALL_HEIGHT, 1.5f);    // Right
}

// ---------- Window Tower (grid pattern: thin black verticals, thin white horizontals) ----------
void drawWindowTower() {
    float doorTopY = TAN_FLOOR_TOP + 0.9f; // top of the entrance

    float totalHeight   = WALL_HEIGHT - 0.9f; // glass extends from top of door to tower top
    float towerWidth    = 2.4f;
    float towerDepth    = 1.2f;             
    float glassThickness = 0.04f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- Main glass volume ---
    glUniform3f(objectColorLoc, 0.55f, 0.80f, 0.95f);  // blueish glass
    glUniform1f(alphaLoc, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, doorTopY + totalHeight / 2.0f, 0.0f); // start at top of doors
    drawCube(towerWidth, totalHeight, glassThickness);
    glPopMatrix();

    // --- Horizontal white lines ---
    int numHorizontals = 6;
    float hThickness = 0.03f;
    float spacing = totalHeight / (numHorizontals + 1);

    glUniform1f(alphaLoc, 1.0f);
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);  
    for (int i = 1; i <= numHorizontals; i++) {
        float y = doorTopY + i * spacing;
        glPushMatrix();
        glTranslatef(0.0f, y, 0.01f);
        drawCube(towerWidth * 0.98f, hThickness, glassThickness + 0.01f);
        glPopMatrix();
    }

    // --- Vertical black lines ---
    int numVerticals = 2;
    float vThickness = 0.04f;

    glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f); 
    for (int i = 0; i < numVerticals; i++) {
        float x = -towerWidth/4 + i * (towerWidth/2);
        glPushMatrix();
        glTranslatef(x, doorTopY + totalHeight/2.0f, 0.02f); 
        drawCube(vThickness, totalHeight, glassThickness + 0.02f);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
}

// ---------- Entrance ----------
void drawEntrance() {
    float doorCenterY = TAN_FLOOR_TOP + 0.45f;

    // Recess
    glUniform3f(objectColorLoc, 0.05f, 0.05f, 0.05f);
    glUniform1f(alphaLoc, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, doorCenterY, 0.55f);
    drawCube(1.5f, 0.9f, 0.5f);
    glPopMatrix();

    // Glass doors
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniform3f(objectColorLoc, 0.75f, 0.9f, 1.0f);
    glUniform1f(alphaLoc, 0.8f);

    glPushMatrix();
    glTranslatef(-0.35f, doorCenterY, 0.85f);
    drawCube(0.45f, 0.85f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.35f, doorCenterY, 0.85f);
    drawCube(0.45f, 0.85f, 0.05f);
    glPopMatrix();
    glDisable(GL_BLEND);

    // Middle divider
    glUniform3f(objectColorLoc, 0.1f, 0.1f, 0.1f);
    glUniform1f(alphaLoc, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, doorCenterY, 0.85f);
    drawCube(0.05f, 0.85f, 0.05f);
    glPopMatrix();

    // Door handles (using glutSolidSphere)
    glUseProgram(0);
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(-0.35f + 0.15f, doorCenterY, 0.9f);
    glutSolidSphere(0.05, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.35f - 0.15f, doorCenterY, 0.9f);
    glutSolidSphere(0.05, 20, 20);
    glPopMatrix();
    glUseProgram(shaderProgram);
}

// Display callback
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camX, camY, camZ, 0.0f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);

    glUseProgram(shaderProgram);

    float viewMat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, viewMat);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMat);

    glMatrixMode(GL_PROJECTION);
    float projMat[16];
    glGetFloatv(GL_PROJECTION_MATRIX, projMat);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projMat);
    glMatrixMode(GL_MODELVIEW);

    // Lighting
    float lightPos[3] = {5.0f, 8.0f, 8.0f};
    float lightCol[3] = {1.0f, 1.0f, 1.0f};
    float viewP[3] = {camX, camY, camZ};
    glUniform3fv(lightPosLoc, 1, lightPos);
    glUniform3fv(lightColorLoc, 1, lightCol);
    glUniform3fv(viewPosLoc, 1, viewP);

    drawGround();
    drawBrickWalls();
    drawWindowTower();
    drawEntrance();

    glutSwapBuffers();
}

// Initialization
void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.9f, 0.95f, 1.0f, 1.0f);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(1);
    }

    createShaders();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    modelLoc      = glGetUniformLocation(shaderProgram, "model");
    viewLoc       = glGetUniformLocation(shaderProgram, "view");
    projLoc       = glGetUniformLocation(shaderProgram, "projection");
    objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    alphaLoc      = glGetUniformLocation(shaderProgram, "alpha");
    lightPosLoc   = glGetUniformLocation(shaderProgram, "lightPos");
    viewPosLoc    = glGetUniformLocation(shaderProgram, "viewPos");
    lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
}

// Reshape
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// Main
int main(int argc, char** argv) {
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Project 5, Use WASD to move around.");

    // Add zoom callbacks
    init();
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
