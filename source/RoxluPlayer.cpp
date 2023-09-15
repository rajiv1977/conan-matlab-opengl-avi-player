#include "RoxluPlayer.h"

using namespace YUV420P;

RoxluPlayer::RoxluPlayer()
    : vid_w(0)
    , vid_h(0)
    , win_w(0)
    , win_h(0)
    , vao(0)
    , y_tex(0)
    , u_tex(0)
    , v_tex(0)
    , vert(0)
    , frag(0)
    , prog(0)
    , u_pos(-1)
    , textures_created(false)
    , shader_created(false)
    , y_pixels(NULL)
    , u_pixels(NULL)
    , v_pixels(NULL)
{
}

RoxluPlayer::~RoxluPlayer()
{
    // Clean-up the graphics resources
    cleanupFrame(y_tex);
    cleanupFrame(u_tex);
    cleanupFrame(v_tex);

    glDeleteShader(vert);
    glDeleteShader(frag);
    glDeleteProgram(prog);
    glDeleteVertexArrays(1, &vao);

    y_pixels = NULL;
    u_pixels = NULL;
    v_pixels = NULL;
}

void RoxluPlayer::draw(int x, int y, int w, int h)
{
    if (!textures_created)
    {
        std::cout << "WARNING: draw() invoked before setup complete related to texture." << std::endl;
        return;
    }
    assert(textures_created == true);

    if (!shader_created)
    {
        std::cout << "WARNING: draw() invoked before setup complete related to shader." << std::endl;
        return;
    }
    assert(shader_created == true);

    if (w == 0)
    {
        w = vid_w;
    }

    if (h == 0)
    {
        h = vid_h;
    }

    glBindVertexArray(vao);
    glUseProgram(prog);

    glUniform4f(u_pos, x, y, w, h);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, y_tex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, u_tex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, v_tex);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void RoxluPlayer::resize(int winW, int winH)
{
    assert(winW > 0 && winH > 0);

    win_w = winW;
    win_h = winH;

    pm.identity();
    pm.ortho(0, win_w, win_h, 0, 0.0, 100.0f);

    glUseProgram(prog);
    glUniformMatrix4fv(glGetUniformLocation(prog, "u_pm"), 1, GL_FALSE, (const GLfloat*) &pm.m);
}

bool RoxluPlayer::setup(int vidW, int vidH)
{

    vid_w = vidW;
    vid_h = vidH;

    if (!vid_w || !vid_h)
    {
        std::cout << "Invalid texture size." << std::endl;
        return false;
    }

    if (y_pixels || u_pixels || v_pixels)
    {
        std::cout << "Already setup the RoxluPlayer." << std::endl;
        return false;
    }

    y_pixels = new uint8_t[vid_w * vid_h];
    u_pixels = new uint8_t[int((vid_w * 0.5) * (vid_h * 0.5))];
    v_pixels = new uint8_t[int((vid_w * 0.5) * (vid_h * 0.5))];

    if (!setupTextures())
    {
        std::cout << "Invalid Texture setup." << std::endl;
        return false;
    }

    if (!setupShader())
    {
        std::cout << "Invalid Shader setup." << std::endl;
        return false;
    }

    glGenVertexArrays(1, &vao);

    return true;
}

bool RoxluPlayer::setupShader()
{

    if (shader_created)
    {
        std::cout << "Already created the shader." << std::endl;
        return false;
    }

    vert = rx_create_shader(GL_VERTEX_SHADER, YUV420P_VS);
    frag = rx_create_shader(GL_FRAGMENT_SHADER, YUV420P_FS);
    prog = rx_create_program(vert, frag);

    glLinkProgram(prog);
    rx_print_shader_link_info(prog);

    glUseProgram(prog);
    glUniform1i(glGetUniformLocation(prog, "y_tex"), 0);
    glUniform1i(glGetUniformLocation(prog, "u_tex"), 1);
    glUniform1i(glGetUniformLocation(prog, "v_tex"), 2);

    u_pos = glGetUniformLocation(prog, "draw_pos");

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    resize(viewport[2], viewport[3]);

    shader_created = true;
    return true;
}

bool RoxluPlayer::setupTextures()
{

    if (textures_created)
    {
        std::cout << "Textures already created." << std::endl;
        return false;
    }

    glGenTextures(1, &y_tex);
    glBindTexture(GL_TEXTURE_2D, y_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, vid_w, vid_h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &u_tex);
    glBindTexture(GL_TEXTURE_2D, u_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, vid_w / 2, vid_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &v_tex);
    glBindTexture(GL_TEXTURE_2D, v_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, vid_w / 2, vid_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textures_created = true;
    return true;
}

void RoxluPlayer::setYPixels(uint8_t* pixels, int stride)
{
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, y_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w, vid_h, GL_RED, GL_UNSIGNED_BYTE, pixels);
}

void RoxluPlayer::setUPixels(uint8_t* pixels, int stride)
{
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, u_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w / 2, vid_h / 2, GL_RED, GL_UNSIGNED_BYTE, pixels);
}

void RoxluPlayer::setVPixels(uint8_t* pixels, int stride)
{
    assert(textures_created == true);

    glBindTexture(GL_TEXTURE_2D, v_tex);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, vid_w / 2, vid_h / 2, GL_RED, GL_UNSIGNED_BYTE, pixels);
}

void RoxluPlayer::cleanupFrame(GLuint& imageTexture)
{
    // Delete the texture
    glDeleteTextures(1, &imageTexture);
}