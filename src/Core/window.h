struct GLFWwindow;

struct window {
    GLFWwindow* nativeWindow = 0;
    unsigned int width = 800;
    unsigned int height = 600;
};
extern window application;

bool window_create();
bool window_sould_close();
void window_update();
void window_free();
