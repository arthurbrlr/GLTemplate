#include "core.h"
#include "core/log.h"
#include "core/window.h"
#include <GL/glew.h>

internal void ApplicationUpdate()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}