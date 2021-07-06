#include <thread>

#include "ui_display.h"
#include "imgui.h"
#include "appmain.h"
#include "osutils.h"

CUIDisplay::CUIDisplay(CAppMain& appMain) :
        m_appMain(appMain)
{
    CLight light;
    light.pos = CPoint3D(-170, 80, -100);
    light.color[0] = 1.0f;
    light.color[1] = 1.0f;
    light.color[2] = 1.0f;
    light.radius = 400;
    light.brightness = 0.1f;
    light.name = "light1";
    m_lights.push_back(light);

    CLight light2;
    light2.pos = CPoint3D(190, -30, 135);
    light2.color[0] = 0;
    light2.color[1] = 1.0f;
    light2.color[2] = 0;
    light2.radius = 400;
    light2.brightness = 0.1f;
    light2.name = "light2";
    m_lights.push_back(light2);

    CLight light3;
    light3.pos = CPoint3D(-165, -12, 110);
    light3.color[0] = 1.0f;
    light3.color[1] = 0;
    light3.color[2] = 0;
    light3.radius = 400;
    light3.brightness = 0.3f;
    light3.name = "light3";
    m_lights.push_back(light3);
}

CUIDisplay::~CUIDisplay()
= default;

void CUIDisplay::DrawStatsPanel()
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

    if(m_pctComplete >= 100)
    {
        m_pctComplete = 0;
    }
    //ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void CUIDisplay::DrawLightmapGeneratorPanel()
{
    ImGui::Begin("Lightmap Generator");

    ImGui::Text("Settings");
    ImGui::Checkbox("Ambient Occlusion", &m_lampOptions.createAO);
    ImGui::Checkbox("Shadows", &m_lampOptions.createShadows);
    ImGui::SliderFloat("Texture Size", &m_lampOptions.lmDetail, 0.6, 1.8);
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
        //m_appMain.ProcessLightmaps(m_lampOptions, m_lights);
        //std::thread(&CAppMain::GenerateLightmaps, m_appMain).detach();
        //std::thread(&CUIDisplay::GenerateLightmaps, this).detach();
        std::thread( [this] { this->GenerateLightmaps(); } ).detach();
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
        m_lampOptions = {
                40,     // numSphereRays for AO
                15,     // spheresize for AO
                230,    // lit
                70,     // unlit
                1.2,    // lmDetail - resolution for textures
                false,  // AO
                true    // shadows
        };
    }
    ImGui::End();
}

void CUIDisplay::DrawLightsPanel()
{
    ImGui::Begin("Lights");

    if (ImGui::BeginListBox(""))
    {
        for (int n = 0; n < m_lights.size(); n++)
        {
            const bool is_selected = (m_selectedLightIndex == n);
            if (ImGui::Selectable(m_lights.at(n).name.c_str(), is_selected))
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
        CLight newLight;
        newLight.name = "light" + std::to_string(m_lights.size() + 1);
        newLight.radius = 400;
        newLight.brightness = 0.1f;
        m_lights.push_back(newLight);
    }

    if (m_selectedLightIndex != -1)
    {
        ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            m_lights.erase(m_lights.begin() + m_selectedLightIndex);
            m_selectedLightIndex = -1;
        }
    }

    ImGui::End();
}

void CUIDisplay::DrawLightInspector()
{
    ImGui::Begin("Light Properties");
    {
        if (m_selectedLightIndex != -1)
        {
            CLight& light = m_lights.at(m_selectedLightIndex);

            ImGui::Text("Name: %s", light.name.c_str());

            float pos[3];
            light.pos.ToFloat3(pos);

            float rot[3];
            light.orientation.ToFloat3(rot);

            if (ImGui::InputFloat3("Position", pos))
            {
                light.pos.Set(pos[0], pos[1], pos[2]);
            }

            if (ImGui::InputFloat3("Rotation", rot))
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
            if (ImGui::MenuItem("Reload"))
            {
                ReloadMesh();
            }

            if (ImGui::MenuItem("Open..", "CTRL+O"))
            {
                open = true;
            }
            if (ImGui::MenuItem("Close"))
            {
                //m_appMain->CloseProject(m_proj);
            }
            if (ImGui::MenuItem("Save", "CTRL+S"))
            {
                //if (!m_proj->Save())
                {
                    //    rade::os::Log("Error saving project.. is one open\n");
                }
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

//        if (file_dialog.showFileDialog("Save File As",
//                imgui_addons::ImGuiFileBrowser::DialogMode::SAVE,
//                ImVec2(700, 310),
//                ".rproj"))
//        {
//            //m_appMain->SaveProject(m_proj, file_dialog.selected_path);
//        }
//
//        if (file_dialog.showFileDialog("Open Project",
//                imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
//                ImVec2(700, 310),
//                ".rproj"))
//        {
//            //m_appMain->OpenProject(m_proj, file_dialog.selected_path);
//        }
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

    // handle the mesh reload within main thread which has display context
    if(m_doReload)
    {
        m_doReload = false;
        ReloadMesh();
    }

    ImGui::Render();
}

bool CUIDisplay::IsAnyItemActive()
{
    return ImGui::IsAnyItemActive();
}

bool CUIDisplay::ReloadMesh()
{
    if(!m_appMain.LoadMesh(OS::ResourcePath("meshes/default.rbmesh")))
    {
        OS::Log("ReloadMesh failed\n");
        return false;
    }
    return true;
}

void CUIDisplay::GenerateLightmaps()
{
    m_appMain.ProcessLightmaps(m_lampOptions, m_lights);
    //std::thread(&CAppMain::ProcessLightmaps, m_appMain, m_lampOptions, m_lights).join();

}

void CUIDisplay::SetPercentComplete(int pctComplete)
{
    if(pctComplete == 1000)
    {
        m_pctComplete = 0;
        m_doReload = true;
        return;
    }

    m_pctComplete = pctComplete;
}
