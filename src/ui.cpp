#include "ui.h"

void render_ui(int server_fd)
{
    switch (ChatManager::current_state)
    {
    case ChatManager::LOGIN_SCREEN:
        render_login_ui(server_fd);
        break;
    case ChatManager::CHAT_LOBBY:
        render_chat_lobby_ui(server_fd);
        break;
    case ChatManager::CHAT_ROOM:
        render_chat_ui(server_fd);
        break;
    }
}

void render_login_ui(int server_fd)
{
    static char id_input[32] = "";

    ImGui::Begin("Login");
    ImGui::Text("Enter your ID to log in:");

    ImGui::InputText("##IDInput", id_input, sizeof(id_input));
    if (ImGui::Button("Login") || ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
        if (strlen(id_input) > 0)
        {
            std::string send_msg = "ID " + std::string(id_input);
            Network::send_cmd_msg(server_fd, send_msg);
        }
    }
    render_error_popup();
    ImGui::End();
}

void render_chat_lobby_ui(int server_fd)
{
    static char room_name_input[256] = "";
    static bool rooms_loaded = false;

    ImGui::Begin("Chat Lobby");

    if (!rooms_loaded)
    {
        Network::send_cmd_msg(server_fd, "LIST");
        rooms_loaded = true;
    }

    ImGui::Text("Rooms:");
    ImGui::BeginChild("RoomList", ImVec2(0, 300), true);
    {
        for (const auto &room : ChatManager::room_list)
        {
            if (ImGui::Button(room.c_str(), ImVec2(-FLT_MIN, 30)))
            {
                std::string send_msg = "JOIN " + room;
                Network::send_cmd_msg(server_fd, send_msg);
            }
        }
    }
    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::Separator();
    ImGui::Text("Create a new room:");
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
    if (ImGui::InputText("##RoomName", room_name_input, sizeof(room_name_input), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        if (strlen(room_name_input) > 0)
        {
            std::string send_msg = "OPEN " + std::string(room_name_input);
            Network::send_cmd_msg(server_fd, send_msg);
            room_name_input[0] = '\0';
        }
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button("Open Room", ImVec2(100, 0)))
    {
        if (strlen(room_name_input) > 0)
        {
            std::string send_msg = "OPEN " + std::string(room_name_input);
            Network::send_cmd_msg(server_fd, send_msg);
            room_name_input[0] = '\0';
        }
    }

    ImGui::Spacing();

    if (ImGui::Button("Refresh Room List", ImVec2(-FLT_MIN, 30)))
    {
        Network::send_cmd_msg(server_fd, "LIST");
    }

    render_error_popup();
    ImGui::End();
}

void render_chat_ui(int server_fd)
{
    ImGui::Begin(ChatManager::current_room.c_str());

    ImVec2 window_size = ImGui::GetContentRegionAvail();

    float margin = 10.0f;
    float panel_spacing = 5.0f;

    float right_panel_ratio = 0.25f;
    float bottom_panel_ratio = 0.1f;

    float right_panel_width = window_size.x * right_panel_ratio;
    float bottom_panel_height = window_size.y * bottom_panel_ratio;

    ImGui::BeginChild(
        "Main Panel",
        ImVec2(window_size.x - right_panel_width - margin - panel_spacing, window_size.y - bottom_panel_height - margin - panel_spacing),
        true);
    for (const std::string &message : ChatManager::chat_messages)
    {
        ImGui::Text("%s", message.c_str());
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Right Panel", ImVec2(right_panel_width - margin, window_size.y - bottom_panel_height - margin - panel_spacing),
                      true);
    if (ChatManager::room_members.find(ChatManager::current_room) != ChatManager::room_members.end())
    {
        const std::vector<std::string> &members = ChatManager::room_members[ChatManager::current_room];
        for (const std::string &user : members)
        {
            ImGui::Text("%s", user.c_str());
        }
    }
    ImGui::EndChild();

    ImGui::SetCursorPosY(window_size.y - margin);
    ImGui::BeginChild("Bottom Panel", ImVec2(window_size.x - margin - panel_spacing, bottom_panel_height - margin), true);

    static char input_buf[256] = "";
    static bool should_focus_input = false;

    if (should_focus_input)
    {
        ImGui::SetKeyboardFocusHere();
        should_focus_input = false;
    }
    bool is_input_active = ImGui::IsItemActive();

    if (ImGui::InputText("##Input", input_buf, IM_ARRAYSIZE(input_buf), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        if (strlen(input_buf) > 0)
        {
            std::string message = input_buf;
            std::string send_msg = "MSG " + message;
            Network::send_cmd_msg(server_fd, send_msg);
            std::string my_msg = "[" + ChatManager::current_room + "]" + "[ME]:" + message;
            ChatManager::add_chat_message(my_msg);
            memset(input_buf, 0, sizeof(input_buf));
        }

        should_focus_input = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Send", ImVec2(80, 0)))
    {
        if (strlen(input_buf) > 0)
        {
            std::string message = input_buf;
            std::string send_msg = "MSG " + message;
            Network::send_cmd_msg(server_fd, send_msg);
            std::string my_msg = "[" + ChatManager::current_room + "]" + "[ME]:" + message;
            ChatManager::add_chat_message(my_msg);
            memset(input_buf, 0, sizeof(input_buf));
        }

        should_focus_input = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Leave", ImVec2(80, 0)))
    {
        std::string send_msg = "LEAVE";
        Network::send_cmd_msg(server_fd, send_msg);
        send_msg = "LIST";
        Network::send_cmd_msg(server_fd, send_msg);
        ChatManager::chat_messages.clear();
        ChatManager::current_state = ChatManager::CHAT_LOBBY;
    }

    ImGui::EndChild();

    ImGui::End();
}

void render_error_popup()
{
    if (ChatManager::show_err_popup)
    {
        ImGui::OpenPopup("Error");
    }

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%s", ChatManager::err_msg.c_str());

        if (ImGui::Button("OK"))
        {
            ChatManager::show_err_popup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}