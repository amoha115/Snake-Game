#include "openGLPart.h"


//glLookAt(p0.x, p0.y, p0.z, pref.x, pref.y, pref.z, u.x, u.y, u.z)
//P0 = snakeBody[(firstIndex + BodySegments) % MAX_SEGMENTS] //lastIndex

//int prev = (i + firstIndex - 1) % (MAX_SEGMENTS);
//d = diff(SnakeBody[lastIndex], SnakeBody[prev])
//Pref = P0 + d/|d| * scaler
//if(d.x != 0){
//v.x = 0
//v.y = 0
//v.z = 1
//}else
//if(d.y != 0){
//v.x = 1
//v.y = 0
//v.z = 0
//}else
//if(d.z != 0){
//v.x = 0
//v.y = 1
//v.z = 0
//}

Point bunnyLocation[NUMBER_OF_BUNNIES];
Point snakeBody[MAX_SEGMENTS];
int bodySegments;
int firstIndex;
GLfloat randomColors[300000];
int lats = 40, longs = 400;
ColorPattern color = SOLID;
bool moving = false, gameover = false;
int blinking = BLINKING_PERIOD;
int eaten = 0;
const int font = (int)GLUT_BITMAP_TIMES_ROMAN_24;//GLUT_BITMAP_9_BY_15;
std::vector<GLfloat> vertices;
std::vector<GLuint> faces;
std::vector<GLfloat> normals;

float x = 0.0, y = 0.0, scaleFactor = 1, rotateX = 0, rotateY = 0, rotateZ = 0; 
float deltaXMove = 0.0, deltaYMove = 0.0; // initially camera doesn't move

typedef GLint vertex3[3];
vertex3 pt[8] = { {-1, -1, -1}, {-1, 1, -1}, {1, -1, -1}, {1, 1, -1},
{-1, -1, 1}, {-1, 1, 1}, {1, -1, 1}, {1, 1, 1} };

void quad(GLint n1, GLint n2, GLint n3, GLint n4)
{
    glBegin(GL_QUADS);
    glVertex3iv(pt[n1]);
    glVertex3iv(pt[n2]);
    glVertex3iv(pt[n3]);
    glVertex3iv(pt[n4]);
    glEnd();
}
void cube()
{
    glColor4f(0.6, 0.6, 0.6, .5);
    quad(2, 0, 1, 3);//FLOOR, Z=0
    glColor4f(0.0, 1.0, 1.0, 0.5);
    quad(5, 1, 0, 4);//X=0
    glColor4f(0.0, 0.0, 1.0, 0.5);//BLUE
    quad(7, 3, 1, 5);//Y=100
    glColor4f(1.0, 1.0, 0.0, 0.5);
    quad(4, 0, 2, 6);//Y=0
    glColor4f(1.0, 0.0, 1.0, .5);//GREEN
    quad(6, 2, 3, 7);//X=100
}
void renderBitmapString(float x, float y, void* font, const char* string) {
    const char* c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}
Point diff(Point p2, Point p1) {
    Point result = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
    return result;
}
double euclideanDistanceSquare(Point p1, Point p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z);
}
/* ******************************************************* */
/* ******************************************************* */
/* *******************INITIALIZATION********************** */
/* ******************************************************* */
/* ******************************************************* */
static void init(void)
{
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, DNEAR, 2*EYE_Z);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, EYE_Z, //< eye position
        0.0f, 0.0f, 0.0f,  //< aim position
        0.0f, 1.f, 0.0f); //< up direction
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_FOG);
    //glFogi(GL_FOG_MODE, GL_LINEAR);
    //glFogf(GL_FOG_START, DNEAR);
    //glFogf(GL_FOG_END, 2*EYE_Z);
    for (int i = 0; i < 300000;i++)
        randomColors[i] = (double)rand() / RAND_MAX;
    for (int i = 0; i < NUMBER_OF_BUNNIES;i++)
        bunnyLocation[i] = randomBunnyLocation(2, 2, 2, i, i);
    firstIndex = 0;
    bodySegments = 1;
    snakeBody[0] = { 0,0,0 };
    snakeBody[1] = { INITIAL_LENGTH, 0, 0 };
    const char* filename = "bunny-low-res.obj";
    load_bunny_data(filename, vertices, faces, normals);
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
        x, y, EYE_Z,
        x , y , 0.0,
        0.0, 1.0, 0.0);
    
    if (gameover) {
        glColor4f(1, 0, 0, ((double)blinking) / BLINKING_PERIOD);
        renderBitmapString(-0.04 * EYE_Z, 0.1, (void*)font, "GAME OVER");
    }
    if (!moving) {
        glColor4f(0, 0, 1, ((double)blinking) / BLINKING_PERIOD);
        renderBitmapString(-0.09 * EYE_Z, .15 * EYE_Z, (void*)font, "PRESS SPACE TO START");
    }
    glScalef(scaleFactor, scaleFactor, scaleFactor);
    glRotatef(rotateX, 1, 0, 0);
    glRotatef(rotateY, 0, 1, 0);
    glRotatef(rotateZ, 0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    cube();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    conicSnake(THICKNESS, TAIL_THICKNESS, color);
    glPolygonMode(GL_FRONT, GL_LINE);
    for (int i = 0; i < NUMBER_OF_BUNNIES;i++) {
        double brightness = (bunnyLocation[i].z + 1) / 2;
        glColor4f(brightness, .2, .2, .3);
        bunny(BUNNY_SIZE,bunnyLocation[i]);
    }
    glutSwapBuffers();
    glFlush();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, EYE_Z, 0, 0, 0, 0, 1, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    int prev = (firstIndex + bodySegments - 1) % (MAX_SEGMENTS);
    int lastIndex = (firstIndex + bodySegments) % MAX_SEGMENTS; //head position
    int scalar = 2;
    Point p0 = snakeBody[(firstIndex + bodySegments) % MAX_SEGMENTS];
    Point d = diff(snakeBody[lastIndex], snakeBody[prev]);
    double absd = sqrt(pow(d.x, 2) + pow(d.y, 2) + pow(d.z, 2));
    Point pref;
    pref.x = p0.x + (d.x / absd) * scalar;
    pref.y = p0.y + (d.y / absd) * scalar;
    pref.z = p0.z + (d.z / absd) * scalar;
    Point v;
    glMatrixMode(GL_MODELVIEW);
    switch (key) {
    case 'x'://rotate the shape about x-axis
    case 'X':
        rotateX += key > 'a' ? -3. : 3;
        glutPostRedisplay();
        break;
    case 'y'://rotate the shape about y-axis
    case 'Y':
        rotateY += key > 'a' ? -3. : 3;
        glutPostRedisplay();
        break;
    case 'z'://rotate the shape about z-axis
    case 'Z':
        rotateZ += key > 'a' ? -3. : 3;
        glutPostRedisplay();
        break;
    case '+'://scaling up the shape uniformly
        scaleFactor *= 1.05;
        glutPostRedisplay();
        break;
    case '-'://scaling down the shape uniformly
        scaleFactor *= .95;
        glutPostRedisplay();
        break;
    case 'i':
    case 'I':
        glLoadIdentity();
        gluLookAt(0, 0, EYE_Z, 0, 0, 0, 0, 1, 0);
        x = 0.0, y = 0.0, scaleFactor = 1, rotateX = 0, rotateY = 0, rotateZ = 0;
        glutPostRedisplay();
        break;
    case ' ':
        if(!gameover)
            moving = !moving;
        break;
    case '1'://static camera
        glLoadIdentity();
        gluLookAt(0.0f, 0.0f, EYE_Z, //< eye position
            0.0f, 0.0f, 0.0f,  //< aim position
            0.0f, 1.f, 0.0f); //< up direction
        x = 0.0, y = 0.0, scaleFactor = 1, rotateX = 0, rotateY = 0, rotateZ = 0;
        glutPostRedisplay();
        break;
    case '2'://dynamic camera
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        if (d.x != 0) {
            v.x = 0;
            v.y = 0;
            v.z = 1;
          
            gluLookAt(p0.x, p0.y, p0.z, pref.x, pref.y, pref.z, v.x, v.y, v.z);
            glutPostRedisplay();
        }
        else if (d.y != 0) {
            v.x = 1;
            v.y = 0;
            v.z = 0;
           
            gluLookAt(p0.x, p0.y, p0.z, pref.x, pref.y, pref.z, v.x, v.y, v.z);
            glutPostRedisplay();
        }
        else if (d.z != 0) {
            v.x = 0;
            v.y = 1;
            v.z = 0;
            
            gluLookAt(p0.x, p0.y, p0.z, pref.x, pref.y, pref.z, v.x, v.y, v.z);
            glutPostRedisplay();
        }
        
        break;
    case 27:
        exit(0);
        break;
    }
}


void pressSpecialKey(int key, int xx, int yy)
{
    int current = (bodySegments + firstIndex - 1) % (MAX_SEGMENTS);
    int next = (current + 1) % (MAX_SEGMENTS);
    Point p = diff(snakeBody[next], snakeBody[current]);
    double len = (p.x < 0 ? -p.x : p.x) + (p.y < 0 ? -p.y : p.y) + (p.z < 0 ? -p.z : p.z);
    switch (key) {
    case GLUT_KEY_UP: 
        if (len >= 6 * THICKNESS && moving) {
            bodySegments++;
            if (p.x != 0) {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] = 
                { snakeBody[next].x, snakeBody[next].y + SPEED, snakeBody[next].z };
            }
            else if (p.y != 0) {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x, snakeBody[next].y, snakeBody[next].z + SPEED };
            }
            else {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x, snakeBody[next].y + SPEED, snakeBody[next].z};
            }
        }
        break;
    case GLUT_KEY_DOWN:  
        if (len >= 6 * THICKNESS && moving) {
            bodySegments++;
            if (p.x != 0) {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x, snakeBody[next].y - SPEED, snakeBody[next].z };
            }
            else if (p.y != 0) {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x, snakeBody[next].y, snakeBody[next].z - SPEED };
            }
            else {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x, snakeBody[next].y - SPEED, snakeBody[next].z };
            }
        }
        break;
    case GLUT_KEY_RIGHT: 
        if (len >= 6 * THICKNESS && moving) {
            bodySegments++;
            if (p.x != 0) {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x, snakeBody[next].y, snakeBody[next].z + SPEED };
            }
            else if (p.y != 0) {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x + SPEED, snakeBody[next].y, snakeBody[next].z };
            }
            else {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x + SPEED, snakeBody[next].y, snakeBody[next].z };
            }
        }
        break;
    case GLUT_KEY_LEFT: 
        if (len >= 6 * THICKNESS && moving) {
            bodySegments++;
            if (p.x != 0) {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x, snakeBody[next].y, snakeBody[next].z - SPEED };
            }
            else if (p.y != 0) {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x - SPEED, snakeBody[next].y, snakeBody[next].z };
            }
            else {
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS] =
                { snakeBody[next].x - SPEED, snakeBody[next].y, snakeBody[next].z };
            }
        }
        break;
    case GLUT_KEY_HOME: deltaYMove = 1.0; break;
    case GLUT_KEY_END: deltaYMove = -1.0; break;
    case GLUT_KEY_PAGE_UP: deltaXMove = 1.0; break;
    case GLUT_KEY_INSERT: deltaXMove = -1.0; break;
    }
}

void releaseSpecialKey(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_HOME: deltaYMove = 0.0; break;
    case GLUT_KEY_END: deltaYMove = 0.0; break;
    case GLUT_KEY_PAGE_UP: deltaXMove = 0.0; break;
    case GLUT_KEY_INSERT: deltaXMove = 0.0; break;
    }
}


void update(void)
{
        x += deltaXMove  * 0.003*scaleFactor;
        y += deltaYMove * 0.003*scaleFactor;
        blinking = (blinking + 1) % BLINKING_PERIOD;
        if (moving && !gameover) {
            Point dTail = diff(snakeBody[(1 + firstIndex) % (MAX_SEGMENTS)], snakeBody[firstIndex]);
            Point dHead = diff(snakeBody[(bodySegments + firstIndex) % (MAX_SEGMENTS)],
                snakeBody[(firstIndex + bodySegments - 1) % (MAX_SEGMENTS)]);
            double val = abs(dTail.x + dTail.y + dTail.z);
            if (val <= 2 * THICKNESS) {
                if (dHead.x != 0)
                    snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].x += dHead.x > 0 ? SPEED * (M_PI-2) / 2 : -SPEED * (M_PI - 2) / 2;
                else if (dHead.y != 0)
                    snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].y += dHead.y > 0 ? SPEED * (M_PI - 2) / 2 : -SPEED * (M_PI - 2) / 2;
                else
                    snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].z += dHead.z > 0 ? SPEED * (M_PI - 2) / 2 : -SPEED * (M_PI - 2) / 2;
            }
            if (val <= SPEED) {
                bodySegments--;
                firstIndex = (firstIndex + 1) % (MAX_SEGMENTS);
                dTail = diff(snakeBody[(1 + firstIndex) % (MAX_SEGMENTS)], snakeBody[firstIndex]);
                if (dTail.x != 0)
                    snakeBody[firstIndex].x += dTail.x > 0 ? (M_PI - 2) * THICKNESS : -(M_PI - 2) * THICKNESS;
                else if (dTail.y != 0)
                    snakeBody[firstIndex].y += dTail.y > 0 ? (M_PI - 2) * THICKNESS : -(M_PI - 2) * THICKNESS;
                else
                    snakeBody[firstIndex].z += dTail.z > 0 ? (M_PI - 2) * THICKNESS : -(M_PI - 2) * THICKNESS;
            }
            else if (eaten == 0)
                if (dTail.x != 0)
                    snakeBody[firstIndex].x += dTail.x > 0 ? SPEED : -SPEED;
                else if (dTail.y != 0)
                    snakeBody[firstIndex].y += dTail.y > 0 ? SPEED : -SPEED;
                else
                    snakeBody[firstIndex].z += dTail.z > 0 ? SPEED : -SPEED;
            else
                eaten--;
            if (dHead.x != 0)
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].x += dHead.x > 0 ? SPEED : -SPEED;
            else if (dHead.y != 0)
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].y += dHead.y > 0 ? SPEED : -SPEED;
            else
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].z += dHead.z > 0 ? SPEED : -SPEED;
            
            if (abs(snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].x) >= 1 ||
                abs(snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].y) >= 1 ||
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].z >=  EYE_Z - DNEAR ||
                snakeBody[(bodySegments + firstIndex) % MAX_SEGMENTS].z <= -1  ||
                snakeCrossesItself()) {
                gameover = true;
            }
            int ate = eatsBunny();
            if (ate >= 0) {
                eaten = GROW_WHEN_EAT;
                bunnyLocation[ate] = randomBunnyLocation(2, 2, 2, NUMBER_OF_BUNNIES, ate);
            }
        }

    glutPostRedisplay(); // redisplay everything
}
int openGLMain()
{
    int argc = 1;
    char* argv[1] = { (char*)"3D Snakes" };
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(300, 200);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow(argv[0]);
    init();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutSpecialFunc(pressSpecialKey); 
    glutSpecialUpFunc(releaseSpecialKey); 
    glutIdleFunc(update); // incremental update 
    glutMainLoop();
    return 0;
}





