struct GLFWwindow;

struct Window {

    GLFWwindow* nativeWindow = nullptr;
    unsigned int width = 800;
    unsigned int height = 600;

    Window(unsigned int w = 800, unsigned int h = 600)
        : width(w), height(h)
    {
    }
};
extern Window application;

bool CreateWindow();
