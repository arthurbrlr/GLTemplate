CC = g++
SRCDIR := .
SRCS   := $(shell find $(SRCDIR) -name "*.cpp")
OBJ_FILES = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
LIBS = -Ltrdp/GLFW/lib -lglfw3 -Ltrdp/GLEW/lib -lGLEW
FLAGS = -std=c++17 -stdlib=libc++ -Itrdp/GLFW/include -Itrdp/GLEW/include

ifeq ($(mode),debug)
 FLAGS += -D _DEBUG -g
 EXEC = bin/debug/run
 OBJDIR := ./bin-int/debug
else
 mode = release
 EXEC = bin/release/run
 OBJDIR := ./bin-int/release
endif

ifeq ($(OS),MACOS)
 LIBS += -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
endif

all: $(EXEC)

clean :
	$(shell find bin-int -name "*.o" -delete)

$(EXEC) : $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(EXEC) $(LIBS)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CC) $(FLAGS) $(INCLUDES)  -o $@ -c $<

$(OBJDIR)/%.o: %.cc | $(OBJDIR)
	$(CC) $(FLAGS) $(INCLUDES)  -o $@ -c $<

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	gcc $(FLAGS) $(INCLUDES) -o $@ -c $<

$(OBJDIR):
	mkdir -p $@