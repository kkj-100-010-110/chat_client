#include "common.h"
#include "ui.h"
#include "socket.h"

std::atomic<bool> running(true);

void FrameBufferSizeCallback(GLFWwindow *window, int width, int height);
void recv_thread(int server_fd);

int main()
{
    std::cout << "START THE PROGRAM" << std::endl;
    std::cout << "INITIALIZE GLFW LIBRARY" << std::endl;
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description);
        std::cout << "ERROR::MAIN::FAILED_TO_INITIALIZE_GLFW_LIB::" << description << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    std::cout << "CREATE GLFW WINDOW" << std::endl;
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        std::cout << "ERROR::MAIN::FAILED_TO_CREATE_GLFW_WINDOW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

    std::cout << "LOAD ALL OPENGL FUNCTION POINTERS" << std::endl;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::MAIN::FAILED_TO_INITIALIZE_GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    Socket sock;
    int server_fd = sock.get_fd();
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1) return 0;
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
    // start thread
    std::thread network_recv_thread(recv_thread, server_fd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    /*
    KOREAN
    - Depending on where you start the program, it should be modified.
    - If you start the program in 'build' directory, it causes an error.
    */
    ImFont *font = io.Fonts->AddFontFromFileTTF("./font/NotoSansKR-Regular.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

    if (!font)
        std::cerr << "Fail to load ./font/NotoSansKR-Regular.ttf" << std::endl;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    io.Fonts->Build();

    ImGui::StyleColorsDark();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        std::string msg = ChatManager::dequeue_message();
        if (!msg.empty())
        {
            Command cmd;
            cmd.command_handler(server_fd, msg);
        }

        render_ui(server_fd);

        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // terminate thread
    running = false;
    if (network_recv_thread.joinable())
        network_recv_thread.join();

    // terminate ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void FrameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    std::cout << "FRAMEBUFFER SIZE CHANGED: " << "(" << width << " x " << height << ")" << std::endl;
    glViewport(0, 0, width, height);
}

void recv_thread(int server_fd)
{
    while (running) {
        Network::receive_message(server_fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "THREAD DONE" << std::endl;
}