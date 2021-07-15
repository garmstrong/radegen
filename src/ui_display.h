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

    CLightmapGen::lmoptions_t m_lampOptions = {
            40,     // numSphereRays for AO
            15.0f,  // spheresize for AO
            230,    // lit
            10,     // unlit
            1.2f,   // lmDetail - resolution for textures
            false,  // AO
            true,   // shadows
            2,      // blur
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
