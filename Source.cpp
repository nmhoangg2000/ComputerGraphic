#include <windows.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include<string> 
#define M_PI 3.14159265359
#define M_PI_2 M_PI/2
#include <glaux.h>

const int sMax = 6, m = 41, mm = m + 1, direction_parts = 36;
int cur_direction = 0;
double distance = 4.;

unsigned textureId = -1, texFloor = -1;
AUX_RGBImageRec* localTexture = NULL, * localFloor = NULL;
int glWin;

struct Tpos
{
    char x, y;
};
struct Player
{
    int x, y, z;
    int dx, dz;
    bool isGo;
}player, ex;

struct Maps
{
    int x, z;
} map;

char data[m + 2][m + 2], cp[m + 2][m + 2];

float	lightAmb[] = { 0.03, 0.03, 0.03 };
float	lightDif[] = { 0.95, 0.95, 0.95 };
float	lightPos[] = { (int)m / 2,  7,  (int)m / 2 };

void display(void);
void halt(bool f = false);

void drawFloor(GLfloat x1, GLfloat x2, GLfloat z1, GLfloat z2, unsigned texture = texFloor)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0, 0);
    glVertex3f(x1, 0, z2);
    glTexCoord2f(1, 0);
    glVertex3f(x2, 0, z2);
    glTexCoord2f(1, 1);
    glVertex3f(x2, 0, z1);
    glTexCoord2f(0, 1);
    glVertex3f(x1, 0, z1);
    glEnd();

}

void drawBox(GLint j, GLint i, unsigned texture = textureId)
{
    GLfloat x1 = i, x2 = i + 1, y1 = 0, y2 = 1, z1 = j, z2 = j + 1;
    glBindTexture(GL_TEXTURE_2D, texture);

    if ((j == map.z + 1) || (data[j - 1][i] != 'x'))
    {
        glBegin(GL_POLYGON); // Back
        glNormal3f(0.0, 0.0, -1.0);
        glTexCoord2f(0, 0);
        glVertex3f(x2, y1, z1);
        glTexCoord2f(1, 0);
        glVertex3f(x1, y1, z1);
        glTexCoord2f(1, 1);
        glVertex3f(x1, y2, z1);
        glTexCoord2f(0, 1);
        glVertex3f(x2, y2, z1);
        glEnd();
    }
    if ((j == map.z - 1) || (data[j + 1][i] != 'x'))
    {
        glBegin(GL_POLYGON); // Front
        glNormal3f(0.0, 0.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(x1, y1, z2);
        glTexCoord2f(1, 0);
        glVertex3f(x2, y1, z2);
        glTexCoord2f(1, 1);
        glVertex3f(x2, y2, z2);
        glTexCoord2f(0, 1);
        glVertex3f(x1, y2, z2);
        glEnd();
    }
    if ((i > 0) && (data[j][i - 1] != 'x'))
    {
        glBegin(GL_POLYGON); // Left
        glNormal3f(-1.0, 0.0, 0.0);
        glTexCoord2f(0, 0);
        glVertex3f(x1, y1, z1);
        glTexCoord2f(1, 0);
        glVertex3f(x1, y1, z2);
        glTexCoord2f(1, 1);
        glVertex3f(x1, y2, z2);
        glTexCoord2f(0, 1);
        glVertex3f(x1, y2, z1);
        glEnd();
    }
    if ((i < map.x) && (data[j][i + 1] != 'x'))
    {
        glBegin(GL_POLYGON); // Right
        glNormal3f(1.0, 0.0, 0.0);
        glTexCoord2f(0, 0);
        glVertex3f(x2, y1, z2);
        glTexCoord2f(1, 0);
        glVertex3f(x2, y1, z1);
        glTexCoord2f(1, 1);
        glVertex3f(x2, y2, z1);
        glTexCoord2f(0, 1);
        glVertex3f(x2, y2, z2);
        glEnd();
    }
    glBegin(GL_POLYGON); // Top
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0, 0);
    glVertex3f(x1, y2, z2);
    glTexCoord2f(1, 0);
    glVertex3f(x2, y2, z2);
    glTexCoord2f(1, 1);
    glVertex3f(x2, y2, z1);
    glTexCoord2f(0, 1);
    glVertex3f(x1, y2, z1);
    glEnd();
}

void animate()
{
    if ((player.x == ex.x) && (player.z == ex.z))
    {
        halt(true);
    };
    if (player.isGo == true)
    {
        if (player.dx > 0)	player.dx += 1; else
            if (player.dz > 0)	player.dz += 1; else
                if (player.dx < 0)	player.dx -= 1; else
                    if (player.dz < 0)	player.dz -= 1;
        if ((player.dx >= sMax) || (player.dz >= sMax))
        {
            player.isGo = false;
            if (player.dx > 0)	player.x += 1;
            if (player.dz > 0)	player.z += 1;
            player.dx = 0; player.dz = 0;
        }
        else
            if ((player.dx <= -sMax) || (player.dz <= -sMax))
            {
                player.isGo = false;
                if (player.dx < 0)	player.x -= 1;
                if (player.dz < 0)	player.z -= 1;
                player.dx = 0; player.dz = 0;
            }
    }
    glutPostRedisplay();
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    //glLightfv    ( GL_LIGHT0, GL_POSITION, lightPos );
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    player.dx = 0; player.dz = 0; player.isGo = false;
    glEnable(GL_COLOR_MATERIAL);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(player.x + (1.0 * player.dx / sMax) + 0.5f + 3 * cos(M_PI_2 + cur_direction / double(direction_parts) * 2. * M_PI), player.y + distance, player.z + (1.0 * player.dz / sMax) + 0.5f + 3 * sin(M_PI_2 + cur_direction / double(direction_parts) * 2. * M_PI),
        player.x + (1.0 * player.dx / sMax) + 0.5f, player.y + 0.5f, player.z + (1.0 * player.dz / sMax) + 0.5f,
        0, 1, 0);

    for (int i = 0; i < map.x; i++)
        for (int j = 0; j < map.z; j++)
            if (data[j][i] == 'x')
            {
                drawBox(j, i);
            }
            else {
                drawFloor(i, i + 1, j, j + 1);
            }

    glPushMatrix();
    glTranslatef(player.x + (1.0 * player.dx / sMax) + 0.5f, player.y + 0.5f, player.z + (1.0 * player.dz / sMax) + 0.5f);
    glColor3d(1, 0, 0);
    glutSolidSphere(0.5, 100, 100);
    glColor3d(1, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(ex.x + 0.5f, ex.y + 0.5f, ex.z + 0.5f);
    glColor4d(1, 1, 0., 0.4);
    glutSolidSphere(0.5, 100, 100);
    glColor3d(1, 1, 1);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 60.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 25, 0, 0, 0, 0, 1, 0);
}

void keyBoardListener(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 'Q' || key == 27) halt(false);
    if (key == 'a' || key == 'A') { cur_direction--; if (cur_direction < 0) cur_direction += direction_parts; }
    if (key == 's' || key == 'S') { cur_direction++; if (cur_direction == direction_parts) cur_direction = 0; }
    if ((key == 'z' || key == 'Z') && (distance < 58.)) distance += 0.25;
    if ((key == 'x' || key == 'X') && (distance > 3.)) distance -= 0.25;
    if (key == 'r' || key == 'R') { cur_direction = 0; distance = 4.; }
}

const int move[4][2] = { {-1,0},{0,-1},{1,0},{0,1} };
const int move_key[4] = { GLUT_KEY_UP, GLUT_KEY_LEFT, GLUT_KEY_DOWN, GLUT_KEY_RIGHT };
bool good_move(int z, int x) {
    return (0 <= z && 0 <= x && z < map.z&& x < map.x&& data[z][x] != 'x');
}

void playerMovement(int key, int x, int y)
{
    if (player.isGo) return;
    int dir = int(((direction_parts - cur_direction - 1) / double(direction_parts)) * 4. + 0.5);
    for (int i = 0; i < 4; i++)
        if (key == move_key[i]) {
            int newz = player.z + move[(dir + i) % 4][0];
            int newx = player.x + move[(dir + i) % 4][1];
            if (good_move(newz, newx)) {
                player.isGo = true;
                player.dz += move[(dir + i) % 4][0];
                player.dx += move[(dir + i) % 4][1];
            }
        }
}

void generateMap(void);

int main(int argc, char** argv)
{
    generateMap();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);

    glWin = glutCreateWindow("THE MAZE");
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyBoardListener);
    glutSpecialFunc(playerMovement);
    glutIdleFunc(animate);

    localTexture = auxDIBImageLoad(L"1.bmp");
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, localTexture->sizeX, localTexture->sizeY, GL_RGB, GL_UNSIGNED_BYTE, localTexture->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    localFloor = auxDIBImageLoad(L"2.bmp");
    glGenTextures(1, &texFloor);
    glBindTexture(GL_TEXTURE_2D, texFloor);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, localFloor->sizeX, localFloor->sizeY, GL_RGB, GL_UNSIGNED_BYTE, localFloor->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glutFullScreen();
    glutMainLoop();
    return 0;
}

void generateMap()
{
    int i, j;
    map.x = m + 2; map.z = m + 2;
    srand((unsigned)time(NULL));
    for (j = 0; j < map.z; j++)
        for (i = 0; i < map.x; i++)
            data[j][i] = 'x';
    data[1][1] = '.';

    srand(time(0));

    /* Carve the maze. */
    for (int y = 1; y < map.z; y += 2) {
        for (int x = 1; x < map.x; x += 2) {

            int x1, y1;
            int x2, y2;
            int dx, dy;
            int dir, count;

            dir = rand() % 4;
            count = 0;
            while (count < 4) {
                dx = 0; dy = 0;
                switch (dir) {
                case 0:  dx = 1;  break;
                case 1:  dy = 1;  break;
                case 2:  dx = -1; break;
                default: dy = -1; break;
                }
                x1 = x + dx;
                y1 = y + dy;
                x2 = x1 + dx;
                y2 = y1 + dy;
                if (x2 > 0 && x2 < map.z && y2 > 0 && y2 < map.x
                    && data[y1][x1] == 'x' && data[y2][x2] == 'x') {
                    data[y1][x1] = '.';
                    data[y2][x2] = '.';
                    x = x2; y = y2;
                    dir = rand() % 4;
                    count = 0;
                }
                else {
                    dir = (dir + 1) % 4;
                    count += 1;
                }
            }
        }
    }

    data[0][1] = '.';
    data[map.z - 1][map.x - 2] = '.';

    player.x = 1;
    player.z = 0;

    ex.x = map.z - 2;
    ex.z = map.x - 1;

}

void halt(bool f)
{
    glutDestroyWindow(glWin);
    exit(0);
}