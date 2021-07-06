#pragma once

#include "lightmapgen.h"

class CAppMain;

class CUIDisplay
{
public:
    CUIDisplay(CAppMain& appMain);

    ~CUIDisplay();

    void SetPercentComplete(int pctComplete);

public:
    void Draw();

    bool IsAnyItemActive();

private:

    int m_pctComplete = 0;

    CAppMain& m_appMain;
    std::vector<CLight> m_lights;

    NRadeLamp::lmOptions_t m_lampOptions = {
            40,     // numSphereRays for AO
            15,     // spheresize for AO
            230,    // lit
            70,     // unlit
            1.2,    // lmDetail - resolution for textures
            false,  // AO
            true    // shadows
    };

    void DrawMenuBar();

    int m_selectedLightIndex = -1;

    void DrawLightInspector();

    void DrawLightsPanel();

    void DrawStatsPanel();

    void DrawLightmapGeneratorPanel();

    bool m_showDemoPanel = false;

    bool ReloadMesh();

    void GenerateLightmaps();

    bool m_doReload = false;

};
