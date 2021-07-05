#include <algorithm>    // std::max
#include "appmain.h"
#include "osutils.h"
#include "polymesh.h"
#include "keycodes.h"
#include "timer.h"

CAppMain::CAppMain() :
        m_uiDisplay(*this)
{
}

CAppMain::~CAppMain()
= default;

bool CAppMain::Init(int videoWidth, int videoHeight)
{
    // init video
    if (!m_display.Init(videoWidth, videoHeight))
    {
        OS::Abort("Display could not be initialised\n");
        return false;
    }

    // setup camera
    m_camera.SetPosition(glm::vec3(10, 70, 20));
    m_camera.SetViewportAspectRatio(static_cast<float>(videoWidth) / static_cast<float>(videoHeight));
    m_camera.SetFieldOfView(90);
    m_camera.SetNearAndFarPlanes(0.1f, 1500.0f);

    // set the active camera, used for default View and Projection matrix when rendering
    m_display.SetActiveCamera(&m_camera);

    // set viewport and camera dimensions
    OnScreenResize(videoWidth, videoHeight);

    m_meshID = LoadMesh(OS::ResourcePath("meshes/simple_out_new.rbmesh"));

    return m_meshID > 0;
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

    m_display.RenderMeshID(m_meshID, m_camera);

    m_uiDisplay.Draw();
}

void CAppMain::OnUIButtonPressed()
{

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
    m_camera.SetViewport(width, height);
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

void CAppMain::OnMouseMove(double xrel, double yrel)
{
    if (m_uiDisplay.IsAnyItemActive())
        return;

    float delta = -10.0f * m_lastDeltaTime;

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

    if (y == 1)
    {
        m_camera.OffsetPosition(10.0f * m_camera.ForwardVector());
    }
    else
    {
        m_camera.OffsetPosition(-10.0f * m_camera.ForwardVector());
    }

}

// "meshes/simple.rbmesh"
uint32_t CAppMain::LoadMesh(const std::string& meshFile)
{
    CMeshFile m_rbMeshFile;

    if (!m_rbMeshFile.LoadFromFile(meshFile))
    {
        OS::Abort("Mesh LoadFromFile() failed\n");
        return 0;
    }

    std::vector<CPoly3D> polyList;
    m_rbMeshFile.GetAsPolyList(polyList);

    CPolyMesh mesh;
    mesh.AddPolyList(polyList);
    mesh.LoadLightmaps(m_display.GetMaterialMgr(), m_rbMeshFile.GetLightMaps());
    //mesh.LoadMaterials(m_display.GetMaterialMgr());

    return m_display.AddMesh(mesh);
}

void CAppMain::UpdateTransformViaInputs(float deltaTime)
{
    float moveSpeed = deltaTime * 250.0f; //250 units per second

    using namespace NKeyboardKeys;

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

//    if (m_captureMouse)
//    {
//        // small offset to keep the mouse pointer within the window so it doesnt interfere with other apps
//        int offsetX = 100;
//        int offsetY = 100;
//
//        //rotate camera based on mouse movement
//        const float mouseSensitivity = 25.0f * deltaTime;
//        int mouseX, mouseY;
//        rade::os::GetPlatform()->GetMousePosition(&mouseX, &mouseY);
//        mouseX -= offsetX;
//        mouseY -= offsetY;
//        m_editModeCamera.OffsetOrientation(mouseSensitivity * (float)mouseY,
//                mouseSensitivity * (float)mouseX);
//        rade::os::GetPlatform()->SetMousePosition(offsetX, offsetY);
//    }
}


bool CAppMain::ProcessLightmaps(NRadeLamp::lmOptions_t lampOptions, std::vector<CLight> lights)
{
    std::string outFile(OS::ResourcePath("meshes/simple_out_new.rbmesh"));

    // scale light colour values
    for (auto& light : lights)
    {
        for (int i = 0; i < 3; i++)
            light.color[i] = std::min<float>(light.color[i] * 255, 255);
    }

    CMeshFile meshFile;
    if (!meshFile.LoadFromFile(OS::ResourcePath("meshes/simple.rbmesh")))
    {
        OS::Log("meshfile does not exists\n");
        return false;
    }

//	DebugDumpMeshLightmapsToPNGs(meshFile);
//	return 0;

    //meshFile.DeleteLightmapData();


    std::vector<CPoly3D> polyList;
    meshFile.GetAsPolyList(polyList);

    OS::Log("loaded %zu polys \n", polyList.size());
    if (!polyList.size())
    {
        OS::Log("Mesh has no polys\n");
        return false;
    }

    CLightmapGen lmGen;

    // generate lightmap data for the polyset
    printf("generating lightmap data..\n");
    std::vector<CLightmapImg> lightMapList;
    CTimer timer;
    lmGen.GenerateLightmaps(lampOptions, polyList, lights, &lightMapList);

    float elapsedTime = timer.ElapsedTime();
    printf("lightmap generation took %.2f seconds\n", elapsedTime);

    // store the new lightmap data to meshfile
    printf("storing compressed lightmap data in meshfile..\n");
    CMeshFile outputMeshFile(polyList);

    // copy lightmap data to meshfile
    //outputMeshFile.DeleteLightmapData();
    for (CLightmapImg& lm : lightMapList)
    {
        // add these in the same order as generated so indexes match up
        outputMeshFile.AddLightmapData(lm.m_width, lm.m_height, lm.m_data, lm.m_width * lm.m_height * 4);
    }

    // write over the original, but now with lightmap data
    outputMeshFile.WriteToFile(outFile);

    //DebugDumpMeshLightmapsToPNGs(outputMeshFile);

    // free allocated LM's, now they are on the meshfile
    printf("freeing resources\n");
    for (CLightmapImg& lm : lightMapList)
    {
        lm.Free();
    }
    lightMapList.clear();

    return true;
}