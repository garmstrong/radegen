#pragma once

#include "polymesh.h"
#include "camera.h"
#include "display_gl.h"
#include "ui_display.h"
#include "meshfile.h"
#include "inputs.h"
#include "lightmapgen.h"
#include "timer.h"

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

    std::vector<rade::polymesh::lightmapInfo_t>& GetLoadedLightmapInfoRef()
    {
        return m_polyMesh.GetLoadedLightmapInfoRef();
    }

protected:

    rade::timer m_timer;

    void UpdateTransformViaInputs(float deltaTime);

    bool m_appDone = false;
    CDisplayGL m_display;
    rade::Camera m_camera;
    rade::Camera m_cameraUI;
    CUIDisplay m_uiDisplay;

    rade::InputSystem m_inputs;
    bool m_isMouseDown = false;
    float m_lastDeltaTime = 0.033f;
    rade::polymesh m_polyMesh;
    std::vector<CLightmapImg*> m_lightMapList;
    std::vector<rade::Light> m_lights;


    IRenderObj *m_logoObj = nullptr;

    IRenderObj *m_mainMesh = nullptr;

    IRenderObj *m_textObj = nullptr;

    bool LoadAppShaders();

    void DeleteLightmaps();

    void DeleteLights();
};
