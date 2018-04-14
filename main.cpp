#include <iostream>
using std::cout;
using std::endl;
#include <vector>
using std::vector;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <sstream>
#include "Cmd.h"
#include "Context.h"
#include "Lsystem.h"
#include "Rulerunner.h"

namespace {

int main_menu_id;
GLdouble tx = -0.5, ty = 0, sc = 1;  //!!!!"class"ify this
unsigned int level = 1;
vector<Lsystem> systems;
int curfractal = -1;  //!!! change to optional or add a bool haveCurrentFractal
double p1 = 0;
double thresh = 0.003;
const double THRESHMAX = 1.0;
const double THRESHMIN = .0001;

void display() {
    Consttype vars;

    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT);
    if (curfractal == -1)
        return;
    vars["p1"] = p1;
    Rulerunner runner(systems[size_t(curfractal)], level, thresh, vars);
    while (!runner.done())
        runner.drawnextpoint();
    glutSwapBuffers();
    while (auto jj = glGetError())
        std::cerr << gluErrorString(jj) << endl;
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-0.5, 1.5, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLineWidth(2.0);
    glColor3d(.8, .2, .5);
}

void change_window_title() {
    std::ostringstream os1;
    os1 << "L' Systems Fractal: " << systems[size_t(curfractal)].getname()
        << " Level: " << level;

    glutSetWindowTitle(os1.str().c_str());
}

void readtheconfigfile() {
    try {
        systems = readlsystemfile();
        curfractal = -1;
        for (unsigned int ii = 0; ii < systems.size(); ++ii) {
            if (systems[ii].isactive()) {
                curfractal = int(ii);
                break;
            }
        }
        change_window_title();
        glutPostRedisplay();
    } catch (std::exception &error) {
        //!!! Some errors (like can't read config file) are terminal, others we can proceed
        std::cerr << error.what() << std::endl;
    }
}

void adjust_level(unsigned int newlevel) {
    level = newlevel;
    change_window_title();
    glutPostRedisplay();
}

void handle_frac_menu(int value) {
    curfractal = value;
    adjust_level(0);
    glutPostRedisplay();
}

void handle_main_menu(int value) {
    if (value < 100)
        handle_frac_menu(value);
    else if (value == 103)
        readtheconfigfile();
}

void keyboard(unsigned char dakey, int /*unused*/, int /*unused*/) {
    const GLdouble SCALEAMOUNT = 1.4;
    const double MOVESIZE = .01;
    if (dakey >= '0' && dakey <= '9')
        adjust_level(dakey - '0');
    else
        switch (dakey) {
            case 27:  //ESC
                exit(0);
            case '=':
                glTranslated(-tx, -ty, 0);
                glScaled(SCALEAMOUNT, SCALEAMOUNT, 1);
                sc *= SCALEAMOUNT;
                glTranslated(tx, ty, 0);
                glutPostRedisplay();
                break;
            case '-':
                glTranslated(-tx, -ty, 0);
                glScaled(1.0 / SCALEAMOUNT, 1.0 / SCALEAMOUNT, 1);
                sc /= SCALEAMOUNT;
                glTranslated(tx, ty, 0);
                glutPostRedisplay();
                break;
            case 'l':
                glTranslated(MOVESIZE / sc, 0, 0);
                tx += MOVESIZE / sc;
                glutPostRedisplay();
                break;
            case 'j':
                glTranslated(-MOVESIZE / sc, 0, 0);
                tx -= MOVESIZE / sc;
                glutPostRedisplay();
                break;
            case 'i':
                glTranslated(0, MOVESIZE / sc, 0);
                ty += MOVESIZE / sc;
                glutPostRedisplay();
                break;
            case 'k':
                glTranslated(0, -MOVESIZE / sc, 0);
                ty -= MOVESIZE / sc;
                glutPostRedisplay();
                break;
            case ',':
                adjust_level(level == 0 ? 0 : level - 1);
                break;
            case '.':
                adjust_level(level + 1);
                break;
            case 'u':
                p1 += .005;
                if (p1 > 1.)
                    p1 = 1.;
                glutPostRedisplay();
                break;
            case 'y':
                p1 -= .005;
                if (p1 < 0.)
                    p1 = 0;
                glutPostRedisplay();
                break;
            case '[':
                thresh *= 1.5;
                if (thresh > THRESHMAX)
                    thresh = THRESHMAX;
                glutPostRedisplay();
                break;
            case ']':
                thresh /= 1.5;
                if (thresh < THRESHMIN)
                    thresh = THRESHMIN;
                glutPostRedisplay();
                break;
            case '\\':
                thresh = THRESHMIN;
                adjust_level(200);
                glutPostRedisplay();
                break;
            case 'f':
                glutPostRedisplay();  //!!! Why?
        }
}

void reshape(int w, int h) {
    glViewport(0, 0, GLsizei(w), GLsizei(h));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (w <= h)
        gluOrtho2D(-0.5, 1.5, -1.0 * h / w, 1.0 * h / w);
    else
        gluOrtho2D(-0.5 * w / h, 1.5 * w / h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}
}  //end anonymous namespace

int main(int argc, char **argv) {
    try {
        cout << "Use <, > (no shift necessary) to change fractal level." << endl;
        cout << "Use i,j,k,l to translate." << endl;
        cout << "Use [ and ] to change threshold for dynamic level drawing." << endl;
        cout << "Use u and v to change parameter for paramerized fractals." << endl;
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
        // Technically, our window is always double-buffered.
        // To simulate single buffering, we draw into the front buffer,
        // and don't swap.
        glutInitWindowSize(801, 801);
        glutInitWindowPosition(100, 100);
        glutCreateWindow("L' Systems ");
        init();
        glutDisplayFunc(display);
        glutKeyboardFunc(keyboard);
        glutReshapeFunc(reshape);
        main_menu_id = glutCreateMenu(handle_main_menu);
        readtheconfigfile();
        glutAddMenuEntry("Reread config file", 103);

        for (unsigned int ii = 0; ii < systems.size(); ++ii)
            if (systems[ii].isactive())
                glutAddMenuEntry(systems[ii].getname().c_str(), int(ii));
        glutAttachMenu(GLUT_RIGHT_BUTTON);

        handle_frac_menu(0);
        glutMainLoop();
        return 0;
    } catch (std::exception &error) {
        std::cerr << error.what() << endl;
    }
}
