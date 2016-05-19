
#include "sim.h"
#include "simapp.h"
#include "simserver.h"
#include "glui/glui.h"
#include "debugdraw.h"
#include <grpc++/grpc++.h>
#include "agents/agentfactory.h"
#include "agents/agent.h"
#include <thread>
#include <mutex>

const int framePeriod = 16;
int mainWindow;

#define WIDTH 320
#define HEIGHT 240

int32 width=WIDTH;
int32 height=HEIGHT;
float32 viewZoom = .2f;
int tx, ty, tw, th;
    b2Vec2 extents;
b2Vec2 viewCenter = b2Vec2(0.0f, 4.0f);

// pointer to global sim object
Sim *sim;

DebugDraw draw;

static GLubyte *pixels = NULL;

static const GLenum FORMAT = GL_RGB;
static const GLuint format_nchannels = 3;

std::mutex mtx;
std::condition_variable cv;

static int draw_count=0;
static int observe_count=0;
static int do_draw = 0;

// forward decl.
static void redraw();
static void observe_GL(boxbot::ObservationData *odata);

static void Timer(int)
{
    glutSetWindow(mainWindow);
    glutPostRedisplay();
    // schedule another redraw
    glutTimerFunc(framePeriod, Timer, 0);
}

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

// drawGL is the glut display fn
static void drawGL()
{

//    std::unique_lock<std::mutex> lk(mtx);
//    cv.wait(lk); // we've obtained the lock

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // call this each frame, to function correctly with devices that may recreate
    // the GL Context without us asking for it
    Resize(width, height);

    sim->getWorld()->DrawDebugData();

    glutSwapBuffers(); // get stuff to show up

    // save image
    pixels = (GLubyte *)realloc(pixels, format_nchannels * sizeof(GLubyte) * width * height);
    glReadPixels(0, 0, width, height, FORMAT, GL_UNSIGNED_BYTE, pixels);

    draw_count++;
    do_draw = 0;

    //lk.unlock();
    cv.notify_one(); // unblock redraw() call


#if 0
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
#endif

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


void screenshot()
{
    // need a cleaner way to do this
    screenshot_ppm("fubar.ppm", WIDTH, HEIGHT, FORMAT, format_nchannels, &pixels);
    std::cout << "done\n" << std::endl;
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
    }
}

void setupGlut()
{
    char *myargv [1];
    int myargc=1;
    myargv [0]=strdup ("Myappname");
    glutInit(&myargc, myargv);
    glutInitContextVersion(2, 0);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    char title[32];
    sprintf(title, "Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
    mainWindow = glutCreateWindow(title);
    glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutDisplayFunc(drawGL);
    glutKeyboardFunc(Keyboard);

    uint32 flags = 0;
    flags += 1			* b2Draw::e_shapeBit;
    //flags += 1			* b2Draw::e_jointBit;
    //flags += 1 * b2Draw::e_aabbBit;
    //flags += 1 * b2Draw::e_pairBit;
    //flags += 1 * b2Draw::e_centerOfMassBit;
    //flags += 1 * b2Draw::e_particleBit;

    draw.SetFlags(flags);

    sim->getWorld()->SetDebugDraw(&draw);
}

//
// SIM SERVER CALLBACK FNS
//

void setupSim(const boxbot::ExperimentDef *edef, boxbot::SimParams *sp)
{

    // construct agent
    Agent *agent;
    if (edef->robot().compare("octoarm")==0)
        agent=makeOctoArmAgent(sim->getWorld(), xform(0,4,0));
    else if (edef->robot().compare("walker")==0)
        agent=makeWalkerAgent(sim->getWorld(), xform(0,3,0));
    else if (edef->robot().compare("polyp")==0)
        agent=makePolypAgent(sim->getWorld(), xform(0,0.3, b2_pi/2));
    else
    {
        assert("DID NOT RECOGNIZE ROBOT\n");
        exit(1);
    }

    sim->addAgent(agent);

    // initial observation
    observe_GL(sp->mutable_x());
    sp->set_u_dim(agent->getJoints().size());
}

static void redraw()
{
    // unblock drawGL thread
    //cv_draw.notify_one();

    // block observe lock until draw is done
    //std::unique_lock<std::mutex> lk(mtx);
    //cv.wait(lk);
    // we own mtx lock?? need to do anything to it?

}

static void observe_GL(boxbot::ObservationData *odata)
{
    // wait for GL thread to refresh glReadPixels
    {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk);
    }
    // we own mtx lock

    observe_count++;
    odata->set_width(width);
    odata->set_height(height);
    odata->set_channels(format_nchannels);
    odata->set_data(pixels, format_nchannels * sizeof(GLubyte) * width * height);

}

void run_app()
{
    sim = new Sim();
    // example agent
    Agent *agent=makeOctoArmAgent(sim->getWorld(), xform(0,4,0));
    sim->addAgent(agent);

    setupGlut();
    // timer drives simulation & refresh
    // TODO - use a different Timer func
    //glutTimerFunc(framePeriod, Timer, 0);

    glutMainLoop();

    // clean up sim object
    delete sim;
    free(pixels);
}

void server_thread()
{
    std::string server_address("0.0.0.0:50051");
    RPCSimImpl service(sim, setupSim, redraw, observe_GL);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

static void idle(void) {
    //while (!do_draw) {}
    glutSetWindow(mainWindow);
    glutPostRedisplay();
}

void run_server()
{
    sim = new Sim();
    setupGlut();

    // server thread drives simulation
    std::thread t1(server_thread);
    //glutTimerFunc(framePeriod, Timer, 0);
    glutIdleFunc(idle);
    glutMainLoop();

    // clean up sim object
    delete sim;
    free(pixels);
}
