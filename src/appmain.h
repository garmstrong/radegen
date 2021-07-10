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

    void OnMouseMove(float xrel, float yrel);

    void OnMouseWheel(int y);

    bool GenerateLightmaps(NRadeLamp::lmOptions_t lampOptions, std::vector<rade::Light> lights);

    void OnUILightmapsComplete();

    bool OnUIMeshSave(const std::string& filename);

    bool OnUIMeshLoad(const std::string& filename);

    std::vector<rade::Light>& GetLightsRef();

    bool AddLight(rade::Light& newLight);

    bool RemoveLight(int index);

    rade::Camera& GetCamera()
    {
        return m_camera;
    }

    bool ChangeLightPos(rade::Light& light, rade::vector3& pos);

    std::vector<rade::CPolyMesh::lightmapInfo_t>& GetLoadedLightmapInfoRef()
    {
        return m_polyMesh.GetLoadedLightmapInfoRef();
    }

protected:

    void UpdateTransformViaInputs(float deltaTime);

    bool m_appDone = false;
    CDisplayGL m_display;
    rade::Camera m_camera;
    rade::Camera m_cameraUI;
    CUIDisplay m_uiDisplay;

    rade::InputSystem m_inputs;
    bool m_isMouseDown = false;
    float m_lastDeltaTime = 0.033f;
    rade::CPolyMesh m_polyMesh;
    std::vector<CLightmapImg> m_lightMapList;
    std::vector<rade::Light> m_lights;

    rade::CPolyMesh m_logomesh;
};
