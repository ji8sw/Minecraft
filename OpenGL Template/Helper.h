#pragma once
#ifndef HELPER
#define HELPER
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum ExitCodes {
	Success = 0,
	Error = 1
};

class imGUI {
public:
    void InitializeImGUI(GLFWwindow* Window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& IO = ImGui::GetIO(); (void)IO;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(Window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void StartFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void StopImGUI()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};
inline imGUI ImGUIManager;

inline const char* ReadFile(const char* Path)
{
    std::ifstream File(Path);
    if (!File.is_open()) 
		throw std::runtime_error(std::format("Could not open file: {}", Path).c_str());
    std::stringstream Buffer;
    Buffer << File.rdbuf();
    File.close();
	return Buffer.str().c_str();;
}

unsigned int CreateTexture(const char* Path, int Type = GL_RGBA, int TextureRepeat = GL_REPEAT, int MipmapSmoothing = GL_LINEAR_MIPMAP_NEAREST, int Smoothing = GL_NEAREST, bool FlipVertically = true);

struct Texture {
    std::string FilePath;
    unsigned int GLUID;
    unsigned int TUID;
    bool Created = false;
    bool Create();
};

enum BlockCatagory {
    NormalPlaced,
    NormalNatural,
    Air
};

struct BlockBase {
    std::string Name;
    BlockCatagory Catagory = BlockCatagory::NormalNatural;

    Texture TopFaceTexture;
    Texture BottomFaceTexture;
    Texture FrontFaceTexture;
    Texture BackFaceTexture;
    Texture LeftFaceTexture;
    Texture RightFaceTexture;

    void BindTextures(unsigned int ShaderProgram);

    bool CreateTextures(unsigned int ShaderProgram);

    void FillTextures(std::string Paths[4]);

    void FillTexturesStandardBlock(std::string Paths[2]); // create 3 textures: top, bottom, sides

    void FillTexturesStandardBlockByPaths(std::string Paths[2]); // create 3 textures: top, bottom, sides

    void FillTexturesFromBlock(BlockBase ToCopy); // copy textures from another block
};

struct Block {
    BlockBase Base;
    bool DiscardTop = false;
    bool DiscardBottom = false;
    bool DiscardFront = false;
    bool DiscardBack = false;
    bool DiscardLeft = false;
    bool DiscardRight = false;
    glm::mat4 Matrix;
};

extern const char* StandardVertexShaderSource;

extern const char* StandardFragmentShaderSource;

#endif