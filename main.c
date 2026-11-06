// mkxp-nx - Hito 1: prueba de contexto OpenGL en Switch (SDL2 + Mesa/EGL)
//
// Objetivo: saber que version de OpenGL / OpenGL ES puede crear la Switch con SDL2,
// cuanta RAM tenemos segun como se lance el .nro, y dejar todo en un log.
// mkxp-z necesita OpenGL 3.3/4.1 (o ES 2.0 con adaptaciones), asi que este dato decide el plan.
//
// Log: sdmc:/switch/mkxp-nx/log.txt

#include <switch.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>

#define LOG_DIR  "sdmc:/switch/mkxp-nx"
#define LOG_PATH LOG_DIR "/log.txt"

static FILE *g_log = NULL;
static char g_summary[16][200];
static int g_nsum = 0;
static PadState g_pad;

static void logmsg(const char *fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    if (g_log) {
        fputs(buf, g_log);
        fputc('\n', g_log);
        fflush(g_log);
    }
}

static void summary(const char *fmt, ...)
{
    if (g_nsum >= 16)
        return;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(g_summary[g_nsum], sizeof g_summary[0], fmt, ap);
    va_end(ap);
    logmsg("%s", g_summary[g_nsum]);
    g_nsum++;
}

static int plus_pressed(void)
{
    padUpdate(&g_pad);
    return (padGetButtonsDown(&g_pad) & HidNpadButton_Plus) != 0;
}

typedef const unsigned char *(*glGetString_t)(unsigned int);
typedef void (*glClearColor_t)(float, float, float, float);
typedef void (*glClear_t)(unsigned int);

typedef struct {
    const char *name;
    int profile;
    int major;
    int minor;
} Cfg;

static const Cfg g_cfgs[] = {
    {"OpenGL 4.1 core", SDL_GL_CONTEXT_PROFILE_CORE, 4, 1},
    {"OpenGL 3.3 core", SDL_GL_CONTEXT_PROFILE_CORE, 3, 3},
    {"OpenGL ES 3.0",   SDL_GL_CONTEXT_PROFILE_ES,   3, 0},
    {"OpenGL ES 2.0",   SDL_GL_CONTEXT_PROFILE_ES,   2, 0},
};

static const char *glstr(glGetString_t gs, unsigned int name)
{
    const unsigned char *s = gs ? gs(name) : NULL;
    return s ? (const char *)s : "(null)";
}

static int try_cfg(const Cfg *c)
{
    SDL_GL_ResetAttributes();
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, c->profile);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, c->major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, c->minor);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window *w = SDL_CreateWindow("mkxp-nx", SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                     SDL_WINDOW_OPENGL);
    if (!w) {
        summary("%s: FALLO al crear ventana (%s)", c->name, SDL_GetError());
        return 0;
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(w);
    if (!ctx) {
        summary("%s: FALLO al crear contexto (%s)", c->name, SDL_GetError());
        SDL_DestroyWindow(w);
        return 0;
    }

    glGetString_t gs = (glGetString_t)SDL_GL_GetProcAddress("glGetString");
    glClearColor_t cc = (glClearColor_t)SDL_GL_GetProcAddress("glClearColor");
    glClear_t cl = (glClear_t)SDL_GL_GetProcAddress("glClear");

    summary("%s: OK", c->name);
    logmsg("  GL_VENDOR   = %s", glstr(gs, 0x1F00));
    logmsg("  GL_RENDERER = %s", glstr(gs, 0x1F01));
    logmsg("  GL_VERSION  = %s", glstr(gs, 0x1F02));
    logmsg("  GLSL        = %s", glstr(gs, 0x8B8C));

    // Unos 60 fotogramas de color cambiante para comprobar que se dibuja y se presenta.
    if (cc && cl) {
        for (int i = 0; i < 60; i++) {
            float t = i / 59.0f;
            cc(t, 0.2f, 1.0f - t, 1.0f);
            cl(0x00004000); // GL_COLOR_BUFFER_BIT
            SDL_GL_SwapWindow(w);
        }
        logmsg("  dibujo + SwapWindow: OK (60 fotogramas)");
    } else {
        logmsg("  no se pudieron resolver glClearColor/glClear");
    }

    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(w);
    return 1;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mkdir("sdmc:/switch", 0777);
    mkdir(LOG_DIR, 0777);
    g_log = fopen(LOG_PATH, "w");

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&g_pad);

    u64 total = 0, used = 0;
    svcGetInfo(&total, InfoType_TotalMemorySize, CUR_PROCESS_HANDLE, 0);
    svcGetInfo(&used, InfoType_UsedMemorySize, CUR_PROCESS_HANDLE, 0);
    summary("Memoria total del proceso: %llu MB (en uso: %llu MB)",
            (unsigned long long)(total >> 20), (unsigned long long)(used >> 20));
    {
        int t = (int)appletGetAppletType();
        summary("Tipo de applet: %d (%s)", t,
                t == AppletType_Application || t == AppletType_SystemApplication
                    ? "juego/titulo completo: RAM completa"
                    : "applet (Album u otro): poca RAM, dato no representativo");
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        summary("SDL_Init FALLO: %s", SDL_GetError());
    } else {
        int ok = 0;
        for (size_t i = 0; i < sizeof g_cfgs / sizeof g_cfgs[0]; i++)
            ok += try_cfg(&g_cfgs[i]);
        summary("Configuraciones que funcionan: %d de %d", ok,
                (int)(sizeof g_cfgs / sizeof g_cfgs[0]));
        SDL_Quit();
    }

    consoleInit(NULL);
    printf("mkxp-nx - Hito 1\n\n");
    for (int i = 0; i < g_nsum; i++)
        printf("%s\n", g_summary[i]);
    printf("\nLog completo: " LOG_PATH "\n");
    printf("Pulsa + para salir\n");
    while (appletMainLoop()) {
        if (plus_pressed())
            break;
        consoleUpdate(NULL);
    }
    consoleExit(NULL);

    if (g_log)
        fclose(g_log);
    return 0;
}
