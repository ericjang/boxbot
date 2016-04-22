/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
* Copyright (c) 2013 Google, Inc.
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <stdio.h>

#include "glui/glui.h"
#include "test/greetertest.h"
//#include "GL/freeglut.h"

//#include <png.h>

const int framePeriod = 16;
int mainWindow;

#include "sim.h"

#define WIDTH 640
#define HEIGHT 540

Sim *sim;

// This is used to control the frame rate (60Hz).
// this is called repeatedly
static void Timer(int)
{
    glutSetWindow(mainWindow);
    glutPostRedisplay();
    glutTimerFunc(framePeriod, Timer, 0);
}

int32 width=WIDTH;
int32 height=HEIGHT;
float32 viewZoom = .2f;
int tx, ty, tw, th;
    b2Vec2 extents;
b2Vec2 viewCenter = b2Vec2(0.0f, 4.0f);

static void Resize(int32 w, int32 h)
{
    width = w;
    height = h;

    tx = 0;
    ty = 0;
    tw = glutGet(GLUT_WINDOW_WIDTH);
    th = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(tx, ty, tw, th);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float32 ratio = th ? float32(tw) / float32(th) : 1;

    extents = ratio >= 1 ? b2Vec2(ratio * 25.0f, 25.0f) : b2Vec2(25.0f, 25.0f / ratio);
    extents *= viewZoom;

    b2Vec2 lower = viewCenter - extents;
    b2Vec2 upper = viewCenter + extents;

    // L/R/B/T
    LoadOrtho2DMatrix(lower.x, upper.x, lower.y, upper.y);

}

// this only runs once? what gives?
static void SimulationLoop()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // call this each frame, to function correctly with devices that may recreate
    // the GL Context without us asking for it
    Resize(width, height);

    sim->step();


    glutSwapBuffers(); // get stuff to show up

    // print world step time stats every 600 frames
    static int s_printCount = 0;
    static b2Stat st;
    st.Record(0); // stepTimeOut

    const int STAT_PRINT_INTERVAL = 600;
    if ( 1 && st.GetCount() == STAT_PRINT_INTERVAL )
    {
        printf("World Step Time samples %i-%i: %fmin %fmax %favg (ms)\n",
            s_printCount*STAT_PRINT_INTERVAL,
            (s_printCount+1)*STAT_PRINT_INTERVAL-1,
            st.GetMin(), st.GetMax(), st.GetMean());
        st.Clear();
        s_printCount++;
    }

}

/*
 * TODO : refer to http://stackoverflow.com/questions/3191978/how-to-use-glut-opengl-to-render-to-a-file
Take screenshot with glReadPixels and save to a file in PPM format.
-   filename: file path to save to, without extension
-   width: screen width in pixels
-   height: screen height in pixels
-   format: glReadPixelsFormat
-   format_nchannels: number of channels per pixel (e.g. R, G, B, A)
    This is implied by format, but we haven't found a built-in way to get this
    information automatically without hard-coding a large switch.
-   pixels: intermediate buffer to avoid repeated mallocs across multiple calls.
    Contents of this buffer do not matter. May be NULL, in which case it is initialized.
    You must `free` it when you won't be calling this function anymore.
*/
static GLubyte *pixels = NULL;
static void screenshot_ppm(const char *filename, unsigned int width, unsigned int height,
         GLenum format, unsigned int format_nchannels, GLubyte **pixels) {
    size_t i, j, k, cur;
    FILE *f = fopen(filename, "w");
    fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
    *pixels = (GLubyte *)realloc(*pixels, format_nchannels * sizeof(GLubyte) * width * height);
    glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            cur = format_nchannels * ((height - i - 1) * width + j);
            fprintf(f, "%3d %3d %3d ", (*pixels)[cur], (*pixels)[cur + 1], (*pixels)[cur + 2]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

static const GLenum FORMAT = GL_RGBA;
static const GLuint format_nchannels = 4;

void screenshot()
{
    // need a cleaner way to do this
    screenshot_ppm("fubar.ppm", WIDTH, HEIGHT, FORMAT, format_nchannels, &pixels);
}

static void Keyboard(unsigned char key, int x, int y)
{
    B2_NOT_USED(x);
    B2_NOT_USED(y);
    switch (key)
    {
        case 's':
            // save PNG image
            screenshot();
            break;
//        default:
//            break;
//            if (sim) sim->onKey(key);
//
    }
}

#if 0
// eventually I will need ability to render headless (image buffers piped over network
// to python StreamingDataSet or something

static int offscreen = 1; // set 1 to render headless to images
static GLuint fbo; // framebuffer
static GLuint rbo_color; // color renderbuffer (instead of fbo->texture)

void initBuffers()
{
    /*  Framebuffer */
   glGenFramebuffers(1, &fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, fbo);

   /* Color renderbuffer. */
   glGenRenderbuffers(1, &rbo_color);
   glBindRenderbuffer(GL_RENDERBUFFER, rbo_color);
   /* Storage must be one of: */
   /* GL_RGBA4, GL_RGB565, GL_RGB5_A1, GL_DEPTH_COMPONENT16, GL_STENCIL_INDEX8. */
   glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, WIDTH, HEIGHT);
   glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);

   // optional - more render buffers here

   glReadBuffer(GL_COLOR_ATTACHMENT0);

}

void freeBuffers()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo_color);
    glDeleteRenderbuffers(1, &rbo_depth);
}
#endif

// This is a simple example of building and running a simulation
// using Box2D. Here we create a large ground box and a small dynamic
// box.
// There are no graphics for this example. Box2D is meant to be used
// with your rendering engine in your game engine.
int main(int argc, char** argv)
{
    B2_NOT_USED(argc);
    B2_NOT_USED(argv);

    // TODO
//    const char simname[]="sim_examples/env1_octoarm.prototxt";

    sim = new Sim();

    int32 width = WIDTH;
    int32 height = HEIGHT;

    glutInit(&argc, argv);
    glutInitContextVersion(2, 0);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    char title[32];
    sprintf(title, "Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
    mainWindow = glutCreateWindow(title);
    //glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    glutDisplayFunc(SimulationLoop);

    //GLUI_Master.set_glutKeyboardFunc(Keyboard);
    glutKeyboardFunc(Keyboard);

    // Use a timer to control the frame rate.
    glutTimerFunc(framePeriod, Timer, 0);

    //glClearColor(1.f, 0.f, 0.f, 1.f);

    glutMainLoop();

    // clean up sim object
    delete sim;
    free(pixels);
    //freeBuffers();

    return 0;
}
