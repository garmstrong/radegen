#pragma once

#include "polymesh.h"
#include "camera.h"
#include "display_gl.h"
#include "ui_display.h"
#include "meshfile.h"
#include "inputs.h"
#include "lightmapgen.h"

class CAppMain
{
public:
    CAppMain();

    ~CAppMain();

    bool Init(int videoWidth, int videoHeight);

    int UpdateTick(float deltaTime);

    void DrawTick(float deltaTime);

    void UpdateCameraInputs(float deltaTime);

    void Shutdown();

    void OnKeyDown(int keycode);

    void OnKeyUp(int keycode);

    void OnScreenResize(int width, int height);

    void OnMouseDown(int buttonId, double x, double y);

    void OnMouseUp(int buttonId, double x, double y);

    void OnMouseMove(double xrel, double yrel);

    void OnMouseWheel(int y);

    void OnUIButtonPressed();

    bool ProcessLightmaps(NRadeLamp::lmOptions_t lampOptions, std::vector<CLight> lights);

    //uint32_t LoadMesh(CMeshFile& meshFile);
    uint32_t UploadNewMesh(CPolyMesh& polyMesh, std::vector<CLightmapImg>& lightmaps);

    void OnUILightmapsComplete();
    bool OnUIMeshSave(const std::string& filename);
    bool OnUIMeshLoad(const std::string& filename);

protected:

    bool m_appDone{};
    bool m_captureMouse{};

    CDisplayGL m_display;
    Camera m_camera;
    CUIDisplay m_uiDisplay;
    uint32_t m_meshID = 0;

    void UpdateTransformViaInputs(float deltaTime);

    CInputSystem m_inputs;

    bool m_isMouseDown = false;

    float m_lastDeltaTime = 0.033f;

    CPolyMesh m_polyMesh;
    std::vector<CLightmapImg> m_lightMapList;

};

