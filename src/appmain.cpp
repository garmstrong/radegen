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
    DeleteLights();
}

void CAppMain::DeleteLights()
{
    for (auto& light : m_lights)
    {
        m_display.DeleteTextMesh(light.renderObj);
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
    m_camera.GetTransform().SetTranslation(rade::vector3(0, 0, 0));
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
    poly.ConstructQuad(323/3, 122/3, 1.0f);
    poly.SetMaterialKey("rade_large");
    poly.SetShaderKey("sprite");
    logomesh.AddPoly(poly);

    m_logoObj = m_display.AddPolyMesh(logomesh);
    m_logoObj->SetCamera(&m_camera);
    m_logoObj->GetTransform().SetTranslation(rade::vector3(-880, -480.0f, -1.0f));
    m_logoObj->GetTransform().SetTranslation(rade::vector3(0.0f, -80.0f, 0.0f));
    m_logoObj->GetTransform().SetRotation(rade::vector3(0.0f, 0.0f, 0.0f));
    m_logoObj->GetTransform().SetScale(rade::vector3(2.0f, 1.0, 1.0f));

    rade::textmesh textMesh;
    if(!textMesh.Init("system/font", 16))
    {
        return false;
    }
    m_textObj = m_display.AddTextMesh(textMesh);
    m_textObj->SetCamera(&m_cameraUI);
    m_textObj->GetTransform().SetTranslation(rade::vector3(0, 0.0f, 0.0f));
    m_textObj->GetTransform().SetScale(rade::vector3(1.7f, 1.0f, 1.0f));
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

int CAppMain::UpdateTick(double deltaTime)
{
    UpdateCameraInputs(deltaTime);

    return m_appDone;
}

void CAppMain::DrawTick(double deltaTime)
{
    m_lastDeltaTime = deltaTime;
    m_display.Draw(deltaTime);

    m_display.RenderAllMeshes();

    m_display.RenderTextObjects();


    m_textObj->SetText("deltatime: %f", m_lastDeltaTime);

//    static float x = 0.0f;
//    x+= 10.0f * m_lastDeltaTime;
//    m_mainMesh->GetTransform().SetRotation( vector3(x, 0.0f, 0.0f) );
//    m_mainMesh->GetTransform().SetScale( {1.0f, 0.5f, 1.0f} );

    m_uiDisplay.Draw();
}

void CAppMain::UpdateCameraInputs(double deltaTime)
{
    //UpdateTransformViaInputs(deltaTime);
    UpdateTransformViaInputs(m_camera.GetTransform());
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

    float delta = 0.2f * m_lastDeltaTime;

    if (m_isMouseDown)
    {
        rade::vector3 rot(yrel * delta, xrel * delta, 0.0f);
        m_camera.GetTransform().OffsetRotation(rot);
        //m_camera.OffsetOrientation(0.0f, xrel * delta);
        //m_camera.OffsetOrientation(yrel * delta, 0.0f);
    }
}

void CAppMain::OnMouseWheel(int y)
{
    if (m_uiDisplay.IsAnyItemActive())
        return;

    float movementDelta = 8.0f * m_lastDeltaTime;
    if (y == -1)
    {
        movementDelta = -movementDelta;
    }
    m_camera.GetTransform().OffsetTranslation(m_camera.GetTransform().ForwardVector() * movementDelta);
}

void CAppMain::UpdateTransformViaInputs(rade::transform &trans)
{
    float moveSpeed = m_lastDeltaTime * 1.0f; //250 units per second
    using namespace rade::keys;
    if (m_inputs.IsPressed(KB_KEY_W))
        trans.OffsetTranslation(trans.ForwardVector() * moveSpeed);
    if (m_inputs.IsPressed(KB_KEY_S))
        trans.OffsetTranslation(trans.ForwardVector() * -moveSpeed);
    if (m_inputs.IsPressed(KB_KEY_A))
        trans.OffsetTranslation(trans.RightVector() * -moveSpeed);
    if (m_inputs.IsPressed(KB_KEY_D))
        trans.OffsetTranslation(trans.RightVector() * moveSpeed);
    if (m_inputs.IsPressed(KB_KEY_LEFT))
        trans.OffsetRotation( {0.0f, -1.0f * moveSpeed, 0.0f});
    if (m_inputs.IsPressed(KB_KEY_RIGHT))
        trans.OffsetRotation( {0.0f, 1.0f * moveSpeed, 0.0f});
    if (m_inputs.IsPressed(KB_KEY_SPACE))
        trans.OffsetTranslation(trans.UpVector() * moveSpeed);
    if (m_inputs.IsPressed(KB_KEY_Z))
        trans.OffsetTranslation(trans.UpVector() * -moveSpeed);
}

bool CAppMain::GenerateLightmaps(CLightmapGen::lmoptions_t lampOptions, std::vector<Light> lights)
{
    std::string outFile(ResourcePath("meshes/default.rbmesh"));

    // scale light colour values to RGB 0-255 instead of 0-1
    for (auto& light : lights)
    {
        for (float& i : light.color)
            i = std::min<float>(i * 255, 255);
    }

    lampOptions.sunColour[0] = std::min<float>(lampOptions.sunColour[0] * 255, 255);
    lampOptions.sunColour[1] = std::min<float>(lampOptions.sunColour[1] * 255, 255);
    lampOptions.sunColour[2] = std::min<float>(lampOptions.sunColour[2] * 255, 255);
    DeleteLightmaps();

    CLightmapGen lmGen;

    Log("generating lightmap data..\n");
    timer timer;

    lmGen.RegisterCallback(
            [this](int pctComplete)
            {
                m_uiDisplay.SetPercentComplete(pctComplete, false);
            });

    lmGen.Generate(lampOptions, m_polyMesh.GetPolyListRef(), lights, &m_lightMapList);

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

    m_mainMesh = m_display.AddPolyMesh(m_polyMesh);

    DeleteLights();

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
    m_mainMesh = m_display.AddPolyMesh(m_polyMesh);
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
    m_display.DeleteTextMesh(m_lights.at(index).renderObj);
    m_lights.erase(m_lights.begin() + index);
    return true;
}

bool CAppMain::AddLight(Light& newLight)
{
    rade::textmesh textMesh;
    if(!textMesh.Init("system/font", 3))
    {
        return false;
    }

    IRenderObj *textObj = m_display.AddTextMesh(textMesh);
    textObj->SetCamera(&m_camera);
    textObj->GetTransform().SetTranslation(newLight.pos);
    textObj->SetText(newLight.name);

    newLight.renderObj = textObj;
    m_lights.emplace_back(newLight);
    return true;
}

bool CAppMain::ChangeLightPos(Light& light, rade::vector3& pos)
{
    light.pos = pos;
    if (light.renderObj)
    {
        light.renderObj->GetTransform().SetTranslation(pos);
    }
    return true;
}
