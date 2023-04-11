/*********
   CTIS164 - HOMEWORK 2
----------
STUDENT :Emre Cura  
SECTION :02
HOMEWORK:2/Object shooting
----------
PROBLEMS:I couldn't produce more than one target. I also couldn't stop the functions when time is up. After time is up, you can still collect
points and these points affect final points.
----------
ADDITIONAL FEATURES:I put random points for targets. You can collect points by shooting targets. I put difficulty levels. If you make game harder, you can 
earn more points. I made a timer and when time becomes 0, points become final points. I put some instructions.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define TARGET_RADIUS 25

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;
int  winWidth, winHeight;
float yA =0; // current Window width and height
int speed = 15;
int point = 0;
int target_point = 0;
int point_max = 100, point_min = 20;
int finalpoint = 0;
typedef struct {
    float x, y;
} point_t;

typedef struct {
    point_t pos;    // position of the object
    float   angle;  // view angle 
    float   r;
} player_t;

typedef struct {
    point_t pos;
    float angle;
    bool active;
} fire_t;

typedef struct {
    int r, g, b;
} color_t;

typedef struct {
    point_t center;
    color_t color;
    float radius;
    float speed;
} target_t;

#define MAX_FIRE 10// 3 fires at a time.
#define FIRE_RATE 8 // after 8 frames you can throw another one.
#define MAX_TARGET 10

player_t p = { {-470, 5}, 0, 100 };
fire_t   fr[MAX_FIRE];
int fire_rate = 0;
bool state = true;
int count = 2000;
int remain = 0;

target_t target;//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void vertex(point_t P, point_t Tr, double angle) {
    float xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
    float yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
    glVertex2f(xp, yp);
}

void displayBackground() {
    // label
    glColor3f(0, 0, 0);
    glLineWidth(3);
    glColor3ub(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-590, winHeight / 2 - 5);
    glVertex2f(-490, winHeight / 2 - 5);
    glVertex2f(-490, winHeight / 2 - 70);
    glVertex2f(-590, winHeight / 2 - 70);
    glEnd();
    vprint(-580, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "Emre Cura");
    vprint(-580, winHeight / 2 - 40, GLUT_BITMAP_8_BY_13, "222002088");
    vprint(-580, winHeight / 2 - 60, GLUT_BITMAP_9_BY_15, "HW #2");
    vprint(400, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "How to Play");
    vprint(400, winHeight / 2 - 40, GLUT_BITMAP_9_BY_15, "Fire:Space Bar");
    vprint(400, winHeight / 2  - 60, GLUT_BITMAP_9_BY_15, "Move Cannon: Up Down");
    vprint(400, winHeight / 2 - 100, GLUT_BITMAP_9_BY_15, "Change Difficulty");
    vprint(400, winHeight / 2 - 120, GLUT_BITMAP_9_BY_15, "(Initially Medium)");
    vprint(400, winHeight / 2 - 140, GLUT_BITMAP_9_BY_15, "1: Easy");
    vprint(400, winHeight / 2 - 160, GLUT_BITMAP_9_BY_15, "2: Medium");
    vprint(400, winHeight / 2 - 180, GLUT_BITMAP_9_BY_15, "3: Hard");
    vprint(400, winHeight / 2 - 200, GLUT_BITMAP_9_BY_15, "4: Impossible");
    vprint(400, winHeight / 2 - 500, GLUT_BITMAP_9_BY_15, "Score: %d", point);
    vprint(400, winHeight / 2 - 520, GLUT_BITMAP_9_BY_15, "Time: %d", count);
    vprint(400, winHeight / 2 - 540, GLUT_BITMAP_9_BY_15, "Final score is %d", finalpoint);

}

void displayCannon() {
    float angle = p.angle * D2R;

    //barrel components
    glLineWidth(4);
    glColor3ub(128, 139, 150);
    glBegin(GL_QUADS);
    vertex({ -50, -15 }, p.pos, angle);
    vertex({ 50, -15 }, p.pos, angle);
    vertex({ 50, 15 }, p.pos, angle);
    vertex({ -50, 15 }, p.pos, angle);
 
    glBegin(GL_QUADS);
    vertex({ -60, -20 }, p.pos, angle);
    vertex({ -50, -20 }, p.pos, angle);
    vertex({ -50, 20 }, p.pos, angle);
    vertex({ -60, 20 }, p.pos, angle);
    glEnd();

    //transaction between barrel and magazine
    glColor3ub(241, 196, 15);
    glBegin(GL_QUADS);
    vertex({ -80, -40 }, p.pos, angle);
    vertex({ -60, -40 }, p.pos, angle);
    vertex({ -60, 40 }, p.pos, angle);
    vertex({ -80, 40 }, p.pos, angle);
    glEnd();
    glLineWidth(1);

    //magazine
    glColor3ub(160, 64, 0);
    glBegin(GL_TRIANGLES);
    vertex({ -100 , 0 }, p.pos, angle);
    vertex({ -80, -40 }, p.pos, angle);
    vertex({ -80, 40 }, p.pos, angle);
    glEnd();

    glColor3ub(160, 64, 0);
    circle(p.pos.x-93, p.pos.y, 12);

    //wheels
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    vertex({ -90, 50 }, p.pos, angle);
    vertex({ -50, 50 }, p.pos, angle);
    vertex({ -50, 55 }, p.pos, angle);
    vertex({ -90, 55 }, p.pos, angle);
    glEnd();
    glLineWidth(1);

    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    vertex({ -90, -50 }, p.pos, angle);
    vertex({ -50, -50 }, p.pos, angle);
    vertex({ -50, -55 }, p.pos, angle);
    vertex({ -90, -55 }, p.pos, angle);
    glEnd();
    glLineWidth(1);

    //connections between cannon and wheels
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    vertex({ -72, 50 }, p.pos, angle);
    vertex({ -68, 50 }, p.pos, angle);
    vertex({ -68, 40 }, p.pos, angle);
    vertex({ -72, 40 }, p.pos, angle);
    glEnd();
    glLineWidth(1);

    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    vertex({ -72, -50 }, p.pos, angle);
    vertex({ -68, -50 }, p.pos, angle);
    vertex({ -68, -40 }, p.pos, angle);
    vertex({ -72, -40 }, p.pos, angle);
    glEnd();
    glLineWidth(1);
}

void drawFires() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active) {
            glColor3ub(129, 129, 129);
            int yB = yA;
            circle(fr[i].pos.x, fr[i].pos.y + yB, 11);
        }
    }
}

void drawTarget(target_t t) {
    float angle = p.angle * D2R;
    glColor3ub(t.color.r, t.color.g, t.color.b);
    circle(t.center.x, t.center.y, t.radius);
    glColor3f(0, 0, 0);
    vprint(t.center.x-15, t.center.y, GLUT_BITMAP_9_BY_15, "%d",target_point);
}


void endGame() {
    glColor3ub(0, 0, 0);
    glRectf(-600, -400, 600, 400);
}



//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(0, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    displayBackground();
    drawFires();
    displayCannon();
    drawTarget(target);



    glutSwapBuffers();
}

int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active == false) return i;
    }
    return -1;
}

void resetTarget() {
    float xPos = rand()%400-(300);
    target.center = { xPos, -250 };
    target.color.r = rand() % 256;
    target.color.g = rand() % 256;
    target.color.b = rand() % 256;
    target.radius = TARGET_RADIUS;
    target.speed = speed * (rand() % 50 + 30) / 100.0 ;
    target_point = target_point = (rand() % point_max + point_min);
}


//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
        spacebar = true;
    switch (key) {
    case '1': speed = 8; point_max = 60; point_min = 5; break;
    case '2': speed = 15; point_max = 100; point_min = 20; break;
    case '3': speed = 23; point_max = 150; point_min = 55; break;
    case '4': speed = 40; point_max = 250; point_min = 90; break;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
        spacebar = false;

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    /*if (key == GLUT_KEY_UP &&yA<169) {
        yA=yA+5;
    }
    if (key == GLUT_KEY_DOWN&&yA>-240) {
        yA=yA-5;
    }*/
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

bool testCollision(fire_t fr, target_t t) {
    float dx = t.center.x - fr.pos.x;
    float dy = t.center.y - fr.pos.y;
    float d = sqrt(dx * dx + dy * dy);
    return d <= t.radius;
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.
    

        if (down)
            p.pos.y -= 5;
        if (up)
            p.pos.y += 5;

        if (count > 0)
            count=count-1;
        if (count == 0) {
            finalpoint = point;
            point!=target_point;
        }
        if (p.pos.y + 120 >= winHeight / 2) p.pos.y -= 5;
        if (p.pos.y - 60 <= -winHeight / 2) p.pos.y += 5;

        if (p.pos.y >= winHeight / 2)
            p.pos.y = -winHeight / 2 - 200;
   

    if (spacebar && fire_rate == 0) {
        int availFire = findAvailableFire();
        if (availFire != -1) {
            fr[availFire].pos = p.pos;
            fr[availFire].angle = p.angle;
            fr[availFire].active = true;
            fire_rate = FIRE_RATE;
        }
    }

    if (fire_rate > 0) fire_rate--;

    // Move all fires that are active.
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active) {
            fr[i].pos.x += 8 * cos(fr[i].angle * D2R);
            fr[i].pos.y += 8 * sin(fr[i].angle * D2R);

            if (fr[i].pos.x > winWidth / 2 || fr[i].pos.x < -winWidth / 2 || fr[i].pos.y>winHeight / 2 || fr[i].pos.y < -winHeight / 2) {
                fr[i].active = false;
            }

            if (testCollision(fr[i], target)) {
                fr[i].active = false;
                resetTarget();
                point += target_point;
            }
        }
    }
    // move target from left to right
    target.center.y += target.speed;
    if (target.center.y > 350) {
        resetTarget();
    }

    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    resetTarget();

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Emre Cura HOMEWORK #2");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}