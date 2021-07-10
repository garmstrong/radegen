#include <thread>

#include "ui_display.h"
#include "imgui.h"
#include "appmain.h"
#include "osutils.h"

CUIDisplay::CUIDisplay(CAppMain& appMain) :
        m_appMain(appMain)
{

}

CUIDisplay::~CUIDisplay()
= default;

void CUIDisplay::DrawStatsPanel() const
{
    ImGui::Begin("Stats");

    if(m_pctComplete >0 || m_pctComplete >= 100)
    {
        ImGui::ProgressBar(m_pctComplete/100.0f, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Progress Bar");
    }
    else
    {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();
}

void CUIDisplay::DrawLightmapGeneratorPanel()
{
    ImGui::Begin("Lightmap Generator");

    ImGui::Text("Settings");
    ImGui::Checkbox("Ambient Occlusion", &m_lampOptions.createAO);
    ImGui::Checkbox("Shadows", &m_lampOptions.createShadows);
    ImGui::SliderFloat("Texture Size", &m_lampOptions.lmDetail, 0.6f, 1.8f);
    ImGui::Separator();

    ImGui::Text("AO Settings");
    ImGui::SliderInt("Sphere Rays", &m_lampOptions.numSphereRays, 20, 80);
    ImGui::SliderFloat("Sphere Size", &m_lampOptions.sphereSize, 5, 100);
    ImGui::Separator();

    ImGui::Text("Shadow Settings");
    ImGui::SliderInt("Lit intensity", &m_lampOptions.shadowLit, 128, 255);
    ImGui::SliderInt("Unlit intensity", &m_lampOptions.shadowUnlit, 0, 127);
    ImGui::Separator();

    if (ImGui::Button("Generate"))
    {
        std::thread( [this] { this->GenerateLightmaps(); } ).detach();
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
        m_lampOptions = {
                40,     // numSphereRays for AO
                15.0f,  // spheresize for AO
                230,    // lit
                70,     // unlit
                1.2f,   // lmDetail - resolution for textures
                false,  // AO
                true    // shadows
        };
    }
    ImGui::End();
}

void CUIDisplay::DrawLMTexturePanel(std::vector<rade::CPolyMesh::lightmapInfo_t>& lmaps)
{
    ImGui::Begin("Textures");
    for(auto& lmInfo : lmaps)
    {
        float my_tex_w = (float)lmInfo.width;
        float my_tex_h = (float)lmInfo.height;
        {
            ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
            ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
            ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
            ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
            ImGui::Image(reinterpret_cast<void*>(lmInfo.texID), ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
        }
    }
    ImGui::End();
}

void CUIDisplay::DrawLightsPanel()
{
    ImGui::Begin("Lights");

    std::vector<rade::Light>& lights = m_appMain.GetLightsRef();

    if (ImGui::BeginListBox(""))
    {
        for (int n = 0; n < lights.size(); n++)
        {
            const bool is_selected = (m_selectedLightIndex == n);
            if (ImGui::Selectable(lights.at(n).name.c_str(), is_selected))
            {
                m_selectedLightIndex = n;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Add"))
    {
        std::string lightName = GetNextLightName(lights);

        rade::Light newLight;
        newLight.name = lightName;
        newLight.radius = 400;
        newLight.brightness = 0.1f;
        m_appMain.AddLight(newLight);
    }

    if (m_selectedLightIndex != -1)
    {
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            m_appMain.RemoveLight(m_selectedLightIndex);
            //lights.erase(lights.begin() + m_selectedLightIndex);
            m_selectedLightIndex = -1;
        }
    }

    ImGui::End();
}

std::string CUIDisplay::GetNextLightName(std::vector<rade::Light>& lights)
{
    std::string newName;
    bool nameFound = false;
    int counter = 1;
    do
    {
        newName = "light" + std::to_string(counter);

        bool alreadyUsed = false;
        for(auto& light : lights)
        {
            if(light.name == newName)
            {
                alreadyUsed = true;
                break;
            }
        }
        if(!alreadyUsed)
        {
            nameFound = true;
        }
        else
        {
            counter++;
        }
    }while(!nameFound);
    return newName;
}

void CUIDisplay::DrawLightInspector()
{
    std::vector<rade::Light>& lights = m_appMain.GetLightsRef();

    ImGui::Begin("Light Properties");
    {
        if (m_selectedLightIndex != -1)
        {
            rade::Light& light = lights.at(m_selectedLightIndex);

            ImGui::Text("Name: %s", light.name.c_str());

            float pos[3];
            light.pos.ToFloat3(pos);

            float rot[3];
            light.orientation.ToFloat3(rot);

            if (ImGui::DragFloat3("Position", pos))
            {
                rade::vector3 newPos(pos);
                m_appMain.ChangeLightPos(light, newPos);
            }

            if (ImGui::Button("Set from camera"))
            {
                rade::vector3 camPos = m_appMain.GetCamera().GetPosition();
                m_appMain.ChangeLightPos(light, camPos);
            }

            if (ImGui::DragFloat3("Rotation", rot))
            {
                light.orientation.Set(rot[0], rot[1], rot[2]);
            }

            ImGui::ColorEdit3("Color", light.color);
            ImGui::SliderFloat("Brightness", &light.brightness, 0.0f, 1.0f);
            ImGui::SliderFloat("Radius", &light.radius, 5.0f, 500.0f);
        }
    }



    ImGui::End();
}

void CUIDisplay::DrawMenuBar()
{
    bool open = false;
    bool saveAs = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "CTRL+O"))
            {
                open = true;
            }
            if (ImGui::MenuItem("Save", "CTRL+S"))
            {
                m_appMain.OnUIMeshSave(m_meshFilename);
            }
            if (ImGui::MenuItem("Save As..", "CTRL+S"))
            {
                saveAs = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("Toggle UI Demo Window"))
            {
                m_showDemoPanel = !m_showDemoPanel;
            }
            ImGui::EndMenu();
        }

        if (saveAs)
        {
            ImGui::OpenPopup("Save File As");
        }

        if (open)
        {
            ImGui::OpenPopup("Open Project");
        }

        if (m_fileDialog.showFileDialog("Save File As",
                imgui_addons::ImGuiFileBrowser::DialogMode::SAVE,
                ImVec2(700, 310),
                ".rbmesh"))
        {
            if(m_appMain.OnUIMeshSave(m_fileDialog.selected_path))
            {
                m_meshFilename = m_fileDialog.selected_path;
            }
        }

        if (m_fileDialog.showFileDialog("Open Project",
                imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
                ImVec2(700, 310),
                ".rbmesh"))
        {
            if(rade::FileExists(m_fileDialog.selected_path))
            {
                m_meshFilename = m_fileDialog.selected_path;
                m_selectedLightIndex = -1;
                m_appMain.OnUIMeshLoad(m_meshFilename);
            }
            else
            {
                rade::Log("No such mesh file %s\n", m_fileDialog.selected_path.c_str());
            }
        }
        ImGui::EndMainMenuBar();
    }
}

void CUIDisplay::Draw()
{
    // Our state
    static bool show_demo_window = false;
    ImGui::NewFrame();

    DrawMenuBar();
    DrawLightInspector();
    DrawLightsPanel();
    DrawStatsPanel();
    DrawLightmapGeneratorPanel();

    if (m_showDemoPanel)
    {
        ImGui::ShowDemoWindow();
    }

    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    DrawLMTexturePanel(m_appMain.GetLoadedLightmapInfoRef());

    // handle the mesh reload within main thread which has display context
    if(m_doReload)
    {
        m_doReload = false;
        m_appMain.OnUILightmapsComplete();
    }

    ImGui::Render();
}

bool CUIDisplay::IsAnyItemActive()
{
    return ImGui::IsAnyItemActive();
}

void CUIDisplay::GenerateLightmaps()
{
    // copy the lights and options into the function
    m_appMain.GenerateLightmaps(m_lampOptions, m_appMain.GetLightsRef());
}

void CUIDisplay::SetPercentComplete(int pctComplete, bool complete)
{
    if (!complete)
    {
        m_pctComplete = pctComplete;
    }
    else
    {
        m_pctComplete = 0;
        // flag to reload back on main thread (which has video/GL context)
        m_doReload = true;
        return;
    }
}

