#include "openGLPart.h"

Point snakeBody[MAX_SEGMENTS];
int bodySegments;
GLfloat randomColors[300000];
int lats = 40, longs = 400;
bool panning = false;
double clickedX, clickedY;
ColorPattern color = CHECKERED;

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
    glOrtho(-1, 1, -1, 1, 1, 20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, EYE_Z, //< eye position
        0.0f, 0.0f, 0.0f,  //< aim position
        0.0f, 1.f, 0.0f); //< up direction

    for (int i = 0; i < 300000;i++)
        randomColors[i] = (double)rand() / RAND_MAX;
    bodySegments = 15;
    snakeBody[0] = { -0.1,0,0 };
    snakeBody[1] = { -0.5, 0, 0 };
    snakeBody[2] = { -0.5,0, 0.5 };
    snakeBody[3] = { -0.5,0.5, 0.5 };
    snakeBody[4] = { 0,0.5, 0.5 };
    snakeBody[5] = { 0,0, 0.5 };
    snakeBody[6] = { 0,0, 0 };
    snakeBody[7] = { 0,0.5, 0 };
    snakeBody[8] = { -0.7,0.5, 0 };
    snakeBody[9] = { -0.7,-0.4, 0 };
    snakeBody[10] = { 0,-0.4, 0 };
    snakeBody[11] = { 0,-0.27, 0 };
    snakeBody[12] = { -0.6,-.27, 0 };
    snakeBody[13] = { -0.6,-.27, 1 };
    snakeBody[14] = { -0.6,0, 1 };
    snakeBody[15] = { 0,0, 1 };
}
Point diff(Point p2, Point p1) {
    Point result = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
    return result;
}

void snake(double r, ColorPattern color) {
    Point d, nextD;
    double len;
    for (int i = 1; i <= bodySegments;i++) {
        glPushMatrix();
        glTranslatef(snakeBody[i - 1].x, snakeBody[i - 1].y, snakeBody[i - 1].z);
        d = diff(snakeBody[i], snakeBody[i - 1]);
        
        if (d.y != 0) {
            len = d.y;
            glRotatef(90, 0, 0, 1);
        }
        else if (d.z != 0) {
            len = d.z;
            glRotatef(-90, 0, 1, 0);
        }
        else
            len = d.x;

        if (i == 1)
        {
            //draw tail
            glTranslatef(-r*1.93, 0, 0);
            cone(lats, longs, r, 0, 7 * r, color);
            glTranslatef(r*1.93, 0, 0);
        }

        if (abs(len) >= 4 * r) {
            double temp = abs(len) - 4 * r+r/10;
            glTranslatef(len > 0 ? 1.95 * r : -temp - 1.95 * r, 0, 0);
            cylinder(lats, longs*temp, r, temp, color);
            glTranslatef(len > 0 ? temp : 0, 0, 0);
        }
        if (i == bodySegments)
        {
            //draw the head here!
            ellipsoid(2 * r, r, r, lats, longs, color);
            //eyes
            //eyeball
            glColor4f(1.0, 1.0, 1.0, 0);
            glTranslatef((4/3)*r, r/3, r);
            eyes(.25 * r, lats, longs);
            glTranslatef(-(4/3)*r, -r/3, -r);
            glTranslatef((4 / 3) * r, r / 3, -r);
            eyes(.25 * r, lats, longs);
            glTranslatef(-(4 / 3) * r, -r / 3, r);
            //pupil
            glColor4f(1.0, 0.5, 0, 0);
            glTranslatef((4 / 3) * r, r / 3, r);
            eyes(.1 * r, lats, longs);
            glTranslatef(0, 0, -2*r);
            eyes(.1 * r, lats, longs);
            glTranslatef(-(4 / 3) * r, -r / 3, r);
        }
        else {//turn
            nextD = diff(snakeBody[i + 1], snakeBody[i]);
            if (d.x == 0 && nextD.x == 0) {//in y-z plane
                if (d.z == 0) {//y-->z
                    glTranslatef(0, 0, nextD.z > 0 ? 2 * r : -2 * r);
                    glRotatef(90, 1, 0, 0);
                    double torusAngle = nextD.z > 0 ? -M_PI / 2 : M_PI / 2;
                    torus(lats, longs * M_PI * r, torusAngle, d.y > 0 ? 0 : 2 * torusAngle, r, 2 * r, color);
                }
                else {//z-->y
                    glTranslatef(0, nextD.y > 0 ? 2 * r : -2 * r, 0);
                    glRotatef(-90, 0, 0, 1);
                    double torusAngle = d.z > 0 ? M_PI / 2 : -M_PI / 2;
                    torus(lats, longs * M_PI * r, torusAngle, nextD.y > 0 ? 0 : 2 * torusAngle, r, 2 * r, color);
                }
            }
            else if (d.y == 0 && nextD.y == 0) {//in z-x plane 
                if (d.x == 0) {//z-->x
                    glTranslatef(0, 0, nextD.x > 0 ? -2 * r : 2 * r);
                    glRotatef(-90,1, 0, 0);
                    double torusAngle = nextD.x > 0 ? -M_PI / 2 : M_PI / 2;
                    torus(lats, longs * M_PI * r, torusAngle, d.z > 0 ? 0 : 2 * torusAngle, r, 2 * r, color);
                }
                else {//x-->z 
                    glTranslatef(0, 0, nextD.z > 0 ? 2 * r : -2 * r);
                    glRotatef(-90, 1, 0, 0);
                    double torusAngle = nextD.z > 0 ? M_PI / 2 : -M_PI / 2;
                    torus(lats, longs * M_PI * r, torusAngle, d.x > 0 ? 0 : 2 * torusAngle, r, 2 * r, color);
                }
            }
            else {//in x-y plane
                if (d.y == 0) {//x-->y
                    glTranslatef(0, nextD.y > 0 ? 2 * r : -2 * r, 0);
                    double torusAngle = nextD.y > 0 ? -M_PI / 2 : M_PI / 2;
                    torus(lats, longs * M_PI * r, torusAngle, d.x > 0 ? 0 : 2 * torusAngle, r, 2 * r, color);
                }
                else {//y-->x
                    glTranslatef(0, nextD.x > 0 ? -2 * r : 2 * r, 0);
                    double torusAngle = nextD.x > 0 ? M_PI / 2 : -M_PI / 2;
                    torus(lats, longs * M_PI * r, torusAngle, d.y > 0 ? 0 : 2 * torusAngle, r, 2 * r, color);
                }
            }
        }
        glPopMatrix();
    }
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    snake(THICKNESS, color);
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
    glMatrixMode(GL_MODELVIEW);
    switch (key) {
    case 'x'://rotate the shape about x-axis
    case 'X':
        glRotatef(key > 'a' ? -3. : 3, 1.0, 0.0, 0.0);
        glutPostRedisplay();
        break;
    case 'y'://rotate the shape about y-axis
    case 'Y':
        glRotatef(key > 'a' ? -3. : 3., 0.0, 1.0, 0.0);
        glutPostRedisplay();
        break;
    case 'z'://rotate the shape about z-axis
    case 'Z':
        glRotatef(key > 'a' ? -3. : 3., 0.0, 0.0, 1.0);
        glutPostRedisplay();
        break;
    case 'l'://scaling up the shape uniformly
    case 'L':
        glScalef(1.02, 1.02, 1.02);
        glutPostRedisplay();
        break;
    case 's'://scaling down the shape uniformly
    case 'S':
        glScalef(0.98, 0.98, 0.98);
        glutPostRedisplay();
        break;
    case GLUT_KEY_UP:
        lats++;
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        lats = lats < 2 ? lats : lats - 1;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        longs++;
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        longs = longs < 2 ? longs : longs - 1;
        glutPostRedisplay();
        break;
    case 'i':
    case 'I':
        glLoadIdentity();
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    }
}
void keyboard2  (int key, int x, int y)
{
    glMatrixMode(GL_MODELVIEW);
    switch (key) {
    case GLUT_KEY_UP:
        lats++;
        glutPostRedisplay();
        break;
    case GLUT_KEY_DOWN:
        lats = lats < 6 ? lats : lats - 1;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        longs++;
        glutPostRedisplay();
        break;
    case GLUT_KEY_LEFT:
        longs = longs < 6 ? longs : longs - 1;
        glutPostRedisplay();
        break;
    }
}
void onMouseClick(int button, int state, int x, int y)
{
    clickedX = x;
    clickedY = y;
    y = glutGet(GLUT_WINDOW_HEIGHT) - y;
    if (button != GLUT_LEFT_BUTTON)
        return;
    if (state == GLUT_DOWN) {
        panning = true;
    }
    else {
        panning = false;
    }
}
void onMouseMove(int x, int y) {
    if (!panning)
        return;
    y = glutGet(GLUT_WINDOW_HEIGHT) - y;
    double normalizedX = -(x - clickedX) / glutGet(GLUT_WINDOW_WIDTH)*3;
    double normalizedY = -(y - clickedY)  / glutGet(GLUT_WINDOW_HEIGHT)*3;
    
    GLfloat m[16], m2[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    glLoadIdentity();
    gluLookAt(normalizedX, normalizedY, EYE_Z, //< eye position
        normalizedX, normalizedY, 0.0f,  //< aim position
        0.0f, 1.f, 0.0f); //< up direction
    glGetFloatv(GL_MODELVIEW_MATRIX, m2);
    for (int i = 12; i < 16; i++)// update the last column of previous modelview matrix
        m[i] = m2[i];
    glLoadMatrixf(m);
    glutPostRedisplay();
}
int openGLMain()
{
    int argc = 1;
    char* argv[1] = { (char*)"3D Snakes" };
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(300, 200);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow(argv[0]);
    init();
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard2);
    glutDisplayFunc(display);
    glutMouseFunc(onMouseClick);
    glutMotionFunc(onMouseMove);
    glutMainLoop();
    return 0;
}
