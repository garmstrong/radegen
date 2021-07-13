#include <functional>
#include "appmain.h"
#include "osutils.h"
#include "polymesh.h"
#include "keycodes.h"
#include "timer.h"

using namespace rade;

CAppMain::CAppMain() :
        m_uiDisplay(*this)
{
}

CAppMain::~CAppMain()
{
    m_polyMesh.ClearLightmaps();

    DeleteLightmaps();

    for (auto& light : m_lights)
    {
        light.label->Reset();
        delete light.label;
    }
    m_lights.clear();
}

void CAppMain::DeleteLightmaps()
{
    // delete any existing lightmaps generated by this tool
    for (CLightmapImg* lm : m_lightMapList)
    {
        delete lm;
    }
    m_lightMapList.clear();
}

bool CAppMain::Init(int videoWidth, int videoHeight)
{
    // init video
    if (!m_display.Init(videoWidth, videoHeight))
    {
        Abort("Display could not be initialised\n");
        return false;
    }

    m_cameraUI.SetOrtho(true);
    m_cameraUI.SetViewport((float)videoWidth, (float)videoHeight);

    // setup camera
    m_camera.SetPosition(glm::vec3(0, 0, 0));
    m_camera.SetViewportAspectRatio(static_cast<float>(videoWidth) / static_cast<float>(videoHeight));
    m_camera.SetFieldOfView(90);
    m_camera.SetNearAndFarPlanes(0.1f, 1500.0f);

    // set the active camera, used for default View and Projection matrix when rendering
    m_display.SetActiveCamera(&m_camera);

    // set viewport and camera dimensions
    OnScreenResize(videoWidth, videoHeight);

    if(!LoadAppShaders())
    {
        return false;
    }

    // use same UI path to load a default mesh at startup
    if (!OnUIMeshLoad(ResourcePath("meshes/default.rbmesh")))
    {
        return false;
    }

    // make a basic quad for the logo
    rade::polymesh logomesh;
    poly3d poly;
    poly.ConstructQuad(323/4, 122/4, 1.0f);
    poly.SetMaterialKey("rade_large");
    poly.SetShaderKey("spriteheat");
    logomesh.AddPoly(poly);

    m_logoObj = m_display.AddMesh(logomesh);
    m_logoObj->SetCamera(&m_camera);
    //m_logoObj->GetTransform().SetPosition(glm::vec3(-880, -480.0f, -1.0f));
    //m_logoObj->GetTransform().OffsetOrientation(0.1f * m_timer.ElapsedTime(), 0.0f);

    return true;
}

bool CAppMain::LoadAppShaders()
{
    if(!m_display.LoadShader(
            "font",
            "data/shaders/font_vert.shader",
            "data/shaders/font_frag.shader"))
    {
        Abort("Failed to create shader\n");
        return false;
    }

    if(!m_display.LoadShader("default",
            "data/shaders/vs.glsl",
            "data/shaders/fs.glsl"))
    {
        Abort("Failed to create shader\n");
        return false;
    }

    if(!m_display.LoadShader("mesh-lightmap",
            "data/shaders/diffuse_spec_vert.shader",
            "data/shaders/diffuse_spec_frag.shader"))
    {
        Abort("Failed to create shader\n");
        return false;
    }

    if(!m_display.LoadShader("sprite",
            "data/shaders/sprite_vert.shader",
            "data/shaders/sprite_frag.shader"))
    {
        Abort("Failed to create shader\n");
        return false;
    }

    if(!m_display.LoadShader(
            "spriteheat",
            "data/shaders/spriteheat_vert.shader",
            "data/shaders/spriteheat_frag.shader"))
    {
        Abort("Failed to create shader\n");
        return false;
    }

    return true;
}

int CAppMain::UpdateTick(float deltaTime)
{
    UpdateCameraInputs(deltaTime);

    return m_appDone;
}

void CAppMain::DrawTick(float deltaTime)
{
    m_lastDeltaTime = deltaTime;
    m_display.Draw(deltaTime);

    m_display.RenderAllMeshes();

    m_display.RenderTextObjects();
    m_uiDisplay.Draw();

}

void CAppMain::UpdateCameraInputs(float deltaTime)
{
    UpdateTransformViaInputs(deltaTime);
}

void CAppMain::Shutdown()
{
    m_display.Shutdown();
}

void CAppMain::OnKeyDown(int keycode)
{
    m_inputs.Press(keycode);
}

void CAppMain::OnKeyUp(int keycode)
{
    m_inputs.Release(keycode);
}

void CAppMain::OnScreenResize(int width, int height)
{
    m_display.SetViewport(width, height);
    m_camera.SetViewport(static_cast<float>(width), static_cast<float>(height));
}

void CAppMain::OnMouseDown(int buttonId, double x, double y)
{
    if (m_uiDisplay.IsAnyItemActive())
        return;

    m_isMouseDown = true;
}

void CAppMain::OnMouseUp(int buttonId, double x, double y)
{
    if (m_uiDisplay.IsAnyItemActive())
        return;

    m_isMouseDown = false;
}

void CAppMain::OnMouseMove(float xrel, float yrel)
{
    if (m_uiDisplay.IsAnyItemActive())
        return;

    float delta = 10.0f * m_lastDeltaTime;

    if (m_isMouseDown)
    {
        m_camera.OffsetOrientation(0.0f, xrel * delta);
        m_camera.OffsetOrientation(yrel * delta, 0.0f);
    }
}

void CAppMain::OnMouseWheel(int y)
{
    if (m_uiDisplay.IsAnyItemActive())
        return;

    float movementDelta = 900.0f * m_lastDeltaTime;
    if (y == -1)
    {
        movementDelta = -movementDelta;
    }
    m_camera.OffsetPosition(movementDelta * m_camera.ForwardVector());
}

void CAppMain::UpdateTransformViaInputs(float deltaTime)
{
    float moveSpeed = deltaTime * 250.0f; //250 units per second

    using namespace rade::keys;

    if (m_inputs.IsPressed(KB_KEY_W))
    {
        m_camera.OffsetPosition(moveSpeed * m_camera.ForwardVector());
        //transform.OffsetPosition(moveSpeed * transform.ForwardVector());
    }

    if (m_inputs.IsPressed(KB_KEY_S))
    {
        m_camera.OffsetPosition(moveSpeed * -m_camera.ForwardVector());
        //transform.OffsetPosition(moveSpeed * -transform.ForwardVector());
    }

    if (m_inputs.IsPressed(KB_KEY_A))
    {
        m_camera.OffsetPosition(moveSpeed * -m_camera.RightVector());
        //transform.OffsetPosition(moveSpeed * -transform.RightVector());
    }

    if (m_inputs.IsPressed(KB_KEY_D))
    {
        m_camera.OffsetPosition(moveSpeed * m_camera.RightVector());
        //transform.OffsetPosition(moveSpeed * transform.RightVector());
    }

    if (m_inputs.IsPressed(KB_KEY_X))
    {
        m_camera.OffsetPosition(moveSpeed * glm::vec3(0, 1, 0));
        //transform.OffsetPosition(moveSpeed * glm::vec3(0, 1, 0));
    }

    if (m_inputs.IsPressed(KB_KEY_LEFT))
    {
        m_camera.OffsetOrientation(0.0f, -70.0f * deltaTime);
        //transform.OffsetOrientation(0.0f, -70.0f * deltaTime);
    }

    if (m_inputs.IsPressed(KB_KEY_RIGHT))
    {
        m_camera.OffsetOrientation(0.0f, 70.0f * deltaTime);
        //transform.OffsetOrientation(0.0f, 70.0f * deltaTime);
    }

    if (m_inputs.IsPressed(KB_KEY_SPACE))
    {
        m_camera.OffsetPosition(moveSpeed * m_camera.UpVector());
        //transform.OffsetPosition(moveSpeed * transform.UpVector());
    }

    if (m_inputs.IsPressed(KB_KEY_Z))
    {
        m_camera.OffsetPosition(moveSpeed * -m_camera.UpVector());
        //transform.OffsetPosition(moveSpeed * -transform.UpVector());
    }
}

bool CAppMain::GenerateLightmaps(NRadeLamp::lmOptions_t lampOptions, std::vector<Light> lights)
{
    std::string outFile(ResourcePath("meshes/default.rbmesh"));

    // scale light colour values to RGB 0-255 instead of 0-1
    for (auto& light : lights)
    {
        for (float& i : light.color)
            i = std::min<float>(i * 255, 255);
    }

    DeleteLightmaps();

    CLightmapGen lmGen;

    Log("generating lightmap data..\n");
    Timer timer;

    lmGen.RegisterCallback(
            [this](int pctComplete)
            {
                m_uiDisplay.SetPercentComplete(pctComplete, false);
            });

    lmGen.GenerateLightmaps(lampOptions, m_polyMesh.GetPolyListRef(), lights, &m_lightMapList);

    float elapsedTime = timer.ElapsedTime();
    Log("lightmap generation took %.2f seconds\n", elapsedTime);

    // notify of completion (mesh is now written to file)
    m_uiDisplay.SetPercentComplete(100, true);
    return true;
}

bool CAppMain::OnUIMeshLoad(const std::string& filename)
{
    MeshFile tmpMesh;
    if (!tmpMesh.LoadFromFile(filename))
    {
        Log("Cant find mesh %s\n", filename.c_str());
        return false;
    }

    std::vector<rade::poly3d> polyList;
    tmpMesh.GetAsPolyList(polyList);
    m_polyMesh.Reset();
    m_polyMesh.AddPolyList(polyList);
    m_polyMesh.SetShaderKey("mesh-lightmap");

    DeleteLightmaps();

    // load any lightmaps from the file
    tmpMesh.GetLightMaps(m_lightMapList);

    // copy into the polymesh
    m_polyMesh.LoadLightmaps(m_display.GetMaterialMgr(), m_lightMapList);

    DeleteLightmaps();

    if(m_mainMesh)
    {
        m_display.DeleteMesh(m_mainMesh);
    }

    m_mainMesh = m_display.AddMesh(m_polyMesh);

    for (auto & m_light : m_lights)
    {
        m_light.label->Reset();
        delete m_light.label;
    }
    m_lights.clear();

    std::vector<mesh::SLight>& meshLights = tmpMesh.GetLightsRef();
    for (auto& light : meshLights)
    {
        Light newLight{};
        newLight.name = std::string(light.name);
        newLight.pos = rade::vector3(light.pos);
        newLight.orientation = rade::vector3(light.dir);
        newLight.brightness = light.brightness;
        newLight.radius = light.radius;
        newLight.color[0] = light.color[0];
        newLight.color[1] = light.color[1];
        newLight.color[2] = light.color[2];
        AddLight(newLight);
    }
    return true; //m_meshID > 0;
}

void CAppMain::OnUILightmapsComplete()
{
    // m_lightmaps is now ready to use
    m_polyMesh.LoadLightmaps(m_display.GetMaterialMgr(), m_lightMapList);

    // delete the old one
    if(m_mainMesh)
    {
        m_display.DeleteMesh(m_mainMesh);
    }

    // add a new one
    m_mainMesh = m_display.AddMesh(m_polyMesh);
}

bool CAppMain::OnUIMeshSave(const std::string& filename)
{
    Log("storing compressed lightmap data in meshfile..\n");
    MeshFile outputMeshFile(m_polyMesh.GetPolyListRef());

    for (CLightmapImg* lm : m_lightMapList)
    {
        // add these in the same order as generated so indexes match up
        outputMeshFile.AddLightmapData(lm->m_width, lm->m_height, lm->m_data, lm->m_width * lm->m_height * 4);
    }

    for (auto& light : m_lights)
    {
        outputMeshFile.AddLight(light);
    }

    if (!outputMeshFile.WriteToFile(filename))
    {
        Log("Failed to save %s\n", filename.c_str());
        return false;
    }
    return true;
}

std::vector<Light>& CAppMain::GetLightsRef()
{
    return m_lights;
}

bool CAppMain::RemoveLight(int index)
{
    m_lights.at(index).label->Reset();
    delete m_lights.at(index).label;
    m_lights.erase(m_lights.begin() + index);
    return true;
}

bool CAppMain::AddLight(Light& newLight)
{
    auto* newLabel = new rade::CTextMesh();
    if (!newLabel->Init(newLight.name, &m_display, &m_camera, newLight.pos, 4, "system/font"))
    {
        return false;
    }
    newLabel->SetText(newLight.name);
    newLight.label = newLabel;
    m_lights.emplace_back(newLight);
    return true;
}

bool CAppMain::ChangeLightPos(Light& light, rade::vector3& pos)
{
    light.pos = pos;
    if (light.label)
    {
        light.label->SetPos(pos);
    }
    return true;
}
