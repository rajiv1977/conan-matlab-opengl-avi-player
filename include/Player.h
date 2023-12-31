#pragma once

#include <iostream>
#include <stdlib.h>

#if defined(__linux) || defined(_WIN32)
#include <GL/glew.h>
#endif

// #define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#define ROXLU_USE_MATH
#define ROXLU_USE_OPENGL
#define ROXLU_IMPLEMENTATION

#include "tinylib.h"
#include "Decoder.h"
#include "RoxluPlayer.h"

void button_callback(GLFWwindow* win, int bt, int action, int mods);
void cursor_callback(GLFWwindow* win, double x, double y);
void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow* win, unsigned int key);
void error_callback(int err, const char* desc);
void resize_callback(GLFWwindow* window, int width, int height);
void frame_callback(AVFrame* frame, AVPacket* pkt, void* user);
void initialize_playback(AVFrame* frame, AVPacket* pkt);
void toLower(std::string& data);

H264::Decoder*        decoder_ptr          = NULL;
YUV420P::RoxluPlayer* player_ptr           = NULL;
bool                  playback_initialized = false;

namespace OpenGL
{
namespace Avi
{
class Player
{
  public:
    GLFWwindow* win = NULL;

    Player() = default;
    Player(std::string& fileName, float& freq)
    {
        std::size_t found = fileName.find_last_of(".");
        std::string str   = fileName.substr(found + 1, fileName.size());
        toLower(str);
        if (!(str.compare("avi") == 0 || str.compare("tavi") == 0))
        {
            std::string errorMsg = ".avi format only.";
            printf(".avi format only. \n");
        }

        glfwSetErrorCallback(error_callback);

        if (!glfwInit())
        {
            std::cout << "Error: cannot setup glfw." << std::endl;
        }

        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        int         w   = 1516;
        int         h   = 853;

        // Get the co�rdinates of the window's top-left corner (relative to the top-left of the screen).
        int windowTopLeftX = 0;
        int windowTopLeftY = 0;

        // Get cursor co�rdinates relative to the window's top-left corner.
        double relativeCursorX = 0.0;
        double relativeCursorY = 0.0;

        win = glfwCreateWindow(w, h, "OpenGL Avi Player", NULL, NULL);
        if (!win)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwSetFramebufferSizeCallback(win, resize_callback);
        glfwSetKeyCallback(win, key_callback);
        glfwSetCharCallback(win, char_callback);
        glfwSetCursorPosCallback(win, cursor_callback);
        glfwSetMouseButtonCallback(win, button_callback);
        glfwMakeContextCurrent(win);
        glfwSwapInterval(1);

#if defined(__linux) || defined(_WIN32)
        if (glewInit() != 0)
        {
            printf("Error: cannot initialize glew.\n");
            ::exit(EXIT_FAILURE);
        }
#endif

        // ----------------------------------------------------------------
        // THIS IS WHERE YOU START CALLING OPENGL FUNCTIONS, NOT EARLIER!!
        // ----------------------------------------------------------------
        H264::Decoder decoder(frame_callback, NULL);

        YUV420P::RoxluPlayer player;

        player_ptr  = &player;
        decoder_ptr = &decoder;

        printf("Loading video file: %s\n", fileName);
        if (!decoder.load(fileName, freq))
        {
            printf("Error loading video file.\n");
            ::Sleep(5000);
            ::exit(EXIT_FAILURE);
        }

        while (!glfwWindowShouldClose(win))
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            decoder.readFrame();
            player.draw(0, 0, w, h);

            glfwSwapBuffers(win);
            glfwPollEvents();
        }

        glfwTerminate();

        ::Sleep(5000);
    }
    ~Player() { win = NULL; };
};
} // namespace Avi
} // namespace OpenGL

void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods)
{

    if (action != GLFW_PRESS)
    {
        return;
    }

    switch (key)
    {
        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(win, GL_TRUE);
            break;
        }
    };
}

void frame_callback(AVFrame* frame, AVPacket* pkt, void* user)
{

    if (!playback_initialized)
    {
        initialize_playback(frame, pkt);
        playback_initialized = true;
    }

    if (player_ptr)
    {
        player_ptr->setYPixels(frame->data[0], frame->linesize[0]);
        player_ptr->setUPixels(frame->data[1], frame->linesize[1]);
        player_ptr->setVPixels(frame->data[2], frame->linesize[2]);
    }
}

void initialize_playback(AVFrame* frame, AVPacket* pkt)
{

    if (frame->format != AV_PIX_FMT_YUV420P)
    {
        printf("This code only support YUV420P data.\n");
        ::exit(EXIT_FAILURE);
    }

    if (!player_ptr)
    {
        printf("player_ptr not found.\n");
        ::exit(EXIT_FAILURE);
    }

    if (!player_ptr->setup(frame->width, frame->height))
    {
        printf("Cannot setup the yuv420 player.\n");
        ::exit(EXIT_FAILURE);
    }
}

void error_callback(int err, const char* desc)
{
    printf("GLFW error: %s (%d)\n", desc, err);
}

void resize_callback(GLFWwindow* window, int width, int height)
{
    if (player_ptr)
    {
        player_ptr->resize(width, height);
    }
}

void button_callback(GLFWwindow* win, int bt, int action, int mods)
{
    double x, y;
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        glfwGetCursorPos(win, &x, &y);
    }
}

void cursor_callback(GLFWwindow* win, double x, double y)
{
    // Get cursor co�rdinates relative to the window's top-left corner.
    double relativeCursorX = 0.0;
    double relativeCursorY = 0.0;
    glfwGetCursorPos(win, &relativeCursorX, &relativeCursorY);

    // Get the co�rdinates of the window's top-left corner (relative to the top-left of the screen).
    int windowTopLeftX = 0;
    int windowTopLeftY = 0;
    glfwGetWindowPos(win, &windowTopLeftX, &windowTopLeftY);

    // Get the absolute co�rdinates of the cursor by combining the window and relative cursor co�rdinates.
    x = static_cast<double>(windowTopLeftX) + relativeCursorX;
    y = static_cast<double>(windowTopLeftY) + relativeCursorY;
}

void char_callback(GLFWwindow* win, unsigned int key)
{
}

void toLower(std::string& data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
}