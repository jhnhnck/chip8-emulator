/*!
 * CHIP-8 Emulator.
 * Copyright © 2020 John Hancock. All rights reserved
 */

#include "Emulator.h"
#include "Resource.h"

#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <chrono>
#include <thread>
#include <fmt/core.h>

using namespace std;

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow) {
int main() {
  auto initTime = chrono::steady_clock::now();
  timeBeginPeriod(1);

  GLFWwindow* window;

  /* Initialize the library */
  if (!glewInit() || !glfwInit()) { return -1; }

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 320, "CHIP-8 Emulator", nullptr, nullptr);
  glfwSetWindowAspectRatio(window, 2, 1);
  HWND hwnd = glfwGetWin32Window(window);

  auto hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongA(hwnd, GWL_HINSTANCE));
  HICON icon = LoadIconA(hInstance, MAKEINTRESOURCEA(IDI_ICON1));

  if (!window || !hwnd) {
    glfwTerminate();
    return -3;
  }

  /* Set window icon via native api */
  SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
  SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // Init Emulator
  CPU cpu = initEmulator(ROM_NAME);

  // Init Textures
  GLuint texture;
  glGenTextures(1, &texture);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Wait for clock cycle */
    auto waitPeriod = (chrono::steady_clock::now() - initTime);
    this_thread::sleep_for(chrono::nanoseconds(CYCLE_LENGTH - (waitPeriod.count() % CYCLE_LENGTH)));

    /* Execute CPU Cycles */
    cpuCycle(&cpu);

    /* Render here */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, cpu.bitScreen);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glEnd();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}