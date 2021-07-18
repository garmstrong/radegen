#pragma once

#include <meshfile.h>
#include "lightmapgen.h"
#include "ImGuiFileBrowser.h"
#include "polymesh.h"

class CAppMain;

class CUIDisplay
{
public:
    CUIDisplay(CAppMain& appMain);

    ~CUIDisplay();

    void SetPercentComplete(int pctComplete, bool complete);

public:
    void Draw();

    bool IsAnyItemActive();

private:

    int m_pctComplete = 0;

    CAppMain& m_appMain;

    CLightmapGen::lmoptions_t m_lampDefaults = {
            40,     // numSphereRays for AO
            6.5f,   // spheresize for AO
            230,    // lit
            10,     // unlit
            1.2f,   // lmDetail - resolution for textures
            false,  // AO
            true,   // shadows
            1,      // blur
            true,   // genereate sun
            { 0.2f, 0.2f, 0.6f },  // sun colour
            { 0.1f, 0.6f, 0.3f }   // sun dir
    };

    CLightmapGen::lmoptions_t m_lampOptions = {
            40,     // numSphereRays for AO
            6.5f,   // spheresize for AO
            230,    // lit
            10,     // unlit
            1.2f,   // lmDetail - resolution for textures
            false,  // AO
            true,   // shadows
            1,      // blur
            true,   // genereate sun
            { 0.2f, 0.2f, 0.6f },  // sun colour
            { 0.1f, 0.6f, 0.3f }   // sun dir
    };

    void DrawMenuBar();

    int m_selectedLightIndex = -1;

    void DrawLightInspector();

    void DrawLightsPanel();

    void DrawStatsPanel() const;

    void DrawLightmapGeneratorPanel();

    bool m_showDemoPanel = false;

    //bool ReloadMesh();

    void GenerateLightmaps();

    bool m_doReload = false;

    imgui_addons::ImGuiFileBrowser m_fileDialog;

    std::string m_meshFilename = "data/meshes/default.rbmesh";

    //bool LoadMesh();
    std::string GetNextLightName(std::vector<rade::Light>& lights);

    void DrawLMTexturePanel(std::vector<rade::polymesh::lightmapInfo_t>& lmaps);
};
