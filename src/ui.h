#ifndef _UI_H_
#define _UI_H_

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "common.h"
#include "chat_manager.h"
#include "network.h"

void render_ui(int server_fd);
void render_login_ui(int server_fd);
void render_chat_lobby_ui(int server_fd);
void render_chat_ui(int server_fd);
void render_error_popup();

#endif//_UI_H_