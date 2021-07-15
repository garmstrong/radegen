#include <cstring>
#include <thread>
#include <chrono>
#include <algorithm>    // std::min
#include "point3d.h"
#include "lightmapgen.h"
#include "plane3d.h"
#include "image.h"
#include "rmath.h"
#include "osutils.h"

CLightmapGen::shpheremap_t* CLightmapGen::GetSphereRaysForNormal(const rade::vector3& normal)
{
    for (auto& m_sphere : m_spheres)
    {
        if (m_sphere->normal == normal)
        {
            return m_sphere;
        }
    }

    auto* newMap = new CLightmapGen::shpheremap_t;
    newMap->normal = normal;

    for (int i = 0; i < m_options.numSphereRays; i++)
    {
        rade::vector3 rayPoint;
        GenerateHemisphereRay(normal, &rayPoint);
        rayPoint.Scale(m_options.sphereSize);
        newMap->rays.push_back(rayPoint);
    }

    m_spheres.push_back(newMap);
    return m_spheres.at(m_spheres.size() - 1);
}

void CLightmapGen::GenerateHemisphereRay(
        const rade::vector3& normal,
        rade::vector3* ret)
{
    while (true)
    {
        rade::vector3 p;
        p.x = rade::math::RandomFloat(-1, 1);
        p.y = rade::math::RandomFloat(-1, 1);
        p.z = rade::math::RandomFloat(-1, 1);

        // reject ones outside unit sphere
        if (p.x * p.x + p.y * p.y + p.z * p.z > 0.9999) continue;

        if (p.Dot(normal) < 0.001) continue;  // ignore "down" dirs (below the surface)

        p.Normalize();

        ret->x = p.x;
        ret->y = p.y;
        ret->z = p.z;
        return;
    }
}

bool CLightmapGen::DoesLineIntersectWithPolyList(
        const rade::vector3& lightPos,
        const rade::vector3& lumelPos,
        const std::vector<rade::poly3d>& polyList)
{
    int count = 0;

    for (const rade::poly3d& poly : polyList)
    {
        count++;
        //rade::plane3d plane(poly);
        rade::plane3d plane = poly.GetPlane();
        // does this line cross the plane at any point
        rade::math::ESide lightSide = plane.ClassifyPoint(lightPos);
        rade::math::ESide lumelSide = plane.ClassifyPoint(lumelPos);
        if (lightSide != lumelSide)
        {
            rade::vector3 hitPos;
            if (plane.GetRayIntersect(lightPos, lumelPos, &hitPos))
            {
                if (poly.PointInPoly(hitPos))
                    return true;
            }
        }
    }
    return false;
}

bool CLightmapGen::DoesLineIntersectWithPolyList(
        const rade::vector3& lightPos,
        const rade::vector3& lumelPos,
        const std::vector<rade::poly3d>& polyList,
        float* distance)
{
    int count = 0;
    for (const rade::poly3d& poly: polyList)
    {
        count++;
        rade::plane3d plane = poly.GetPlane();

        // does this line cross the plane at any point
        rade::math::ESide lightSide = plane.ClassifyPoint(lightPos);
        rade::math::ESide lumelSide = plane.ClassifyPoint(lumelPos);
        if (lightSide != lumelSide)
        {
            rade::vector3 hitPos;
            if (plane.GetRayIntersect(lightPos, lumelPos, &hitPos))
            {
                if (poly.PointInPoly(hitPos))
                {
                    *distance = hitPos.Distance(lightPos);
                    return true;
                }
            }
        }
    }
    return false;
}

bool CLightmapGen::DoesRayIntersectWithPolyList(
        const rade::vector3& pos,
        const rade::vector3& ray,
        const std::vector<rade::poly3d>& polyList,
        float* distance)
{
    int count = 0;
    for (const rade::poly3d& poly: polyList)
    {
        count++;
        rade::plane3d plane = poly.GetPlane();

        // does this line cross the plane at any point
        rade::vector3 hitPos;
        if (plane.GetRayIntersection(pos, ray, &hitPos))
        {
            if (poly.PointInPoly(hitPos))
            {
                *distance = hitPos.Distance(pos);
                return true;
            }
        }
    }
    return false;
}

void CLightmapGen::CalcEdgeVectors(const rade::plane3d& plane, const float* uvMin, const float* uvMax, rade::vector3& edge1,
        rade::vector3& edge2,
        rade::vector3& UVVector)
{
    float Distance = plane.GetDistance();
    const rade::vector3& normal = plane.GetNormal();

    float Min_U = uvMin[0];
    float Min_V = uvMin[1];
    float Max_U = uvMax[0];
    float Max_V = uvMax[1];

    rade::vector3 vect1, vect2;
    float X, Y, Z;

    // calc the missing uv vector based on plane equation
    switch (plane.GetPlaneAxis())
    {
    case rade::plane3d::EPlaneAxis::EPlaneAxis_YZ:
        X = -(normal.y * Min_U + normal.z * Min_V + Distance)
            / normal.x;
        UVVector.x = X;
        UVVector.y = Min_U;
        UVVector.z = Min_V;
        X = -(normal.y * Max_U + normal.z * Min_V + Distance)
            / normal.x;
        vect1.x = X;
        vect1.y = Max_U;
        vect1.z = Min_V;
        X = -(normal.y * Min_U + normal.z * Max_V + Distance)
            / normal.x;
        vect2.x = X;
        vect2.y = Min_U;
        vect2.z = Max_V;
        break;

    case rade::plane3d::EPlaneAxis::EPlaneAxis_XZ:
        Y = -(normal.x * Min_U + normal.z * Min_V + Distance)
            / normal.y;
        UVVector.x = Min_U;
        UVVector.y = Y;
        UVVector.z = Min_V;
        Y = -(normal.x * Max_U + normal.z * Min_V + Distance)
            / normal.y;
        vect1.x = Max_U;
        vect1.y = Y;
        vect1.z = Min_V;
        Y = -(normal.x * Min_U + normal.z * Max_V + Distance)
            / normal.y;
        vect2.x = Min_U;
        vect2.y = Y;
        vect2.z = Max_V;
        break;

    case rade::plane3d::EPlaneAxis::EPlaneAxis_XY:
        Z = -(normal.x * Min_U + normal.y * Min_V + Distance)
            / normal.z;
        UVVector.x = Min_U;
        UVVector.y = Min_V;
        UVVector.z = Z;
        Z = -(normal.x * Max_U + normal.y * Min_V + Distance)
            / normal.z;
        vect1.x = Max_U;
        vect1.y = Min_V;
        vect1.z = Z;
        Z = -(normal.x * Min_U + normal.y * Max_V + Distance)
            / normal.z;
        vect2.x = Min_U;
        vect2.y = Max_V;
        vect2.z = Z;
        break;
    }
    edge1 = vect1 - UVVector;
    edge2 = vect2 - UVVector;
}

void CLightmapGen::NormalizeLightmapUVs(std::vector<rade::vector3>& polyPoints, float* minimums, float* maximums,
        uint16_t* polyUWidth, uint16_t* polyVHeight)
{
    // get min/max uv values and re-scale the co-ords to fit
    const rade::vector3& puv = polyPoints.at(0);
    float Min_U = puv.lmU;
    float Min_V = puv.lmV;
    float Max_U = puv.lmU;
    float Max_V = puv.lmV;

    for (auto& p : polyPoints)
    {
        if (p.lmU < Min_U)
            Min_U = p.lmU;
        if (p.lmV < Min_V)
            Min_V = p.lmV;
        if (p.lmU > Max_U)
            Max_U = p.lmU;
        if (p.lmV > Max_V)
            Max_V = p.lmV;
    }

    float Delta_U = Max_U - Min_U;
    float Delta_V = Max_V - Min_V;

    for (auto& polyPoint : polyPoints)
    {
        polyPoint.lmU -= Min_U;
        polyPoint.lmV -= Min_V;
        polyPoint.lmU /= Delta_U;
        polyPoint.lmV /= Delta_V;
    }

    *polyUWidth = (uint16_t)Delta_U;
    *polyVHeight = (uint16_t)Delta_V;

    // return min and max ranges
    minimums[0] = Min_U;
    minimums[1] = Min_V;
    maximums[0] = Max_U;
    maximums[1] = Max_V;
}

void CLightmapGen::CalcLightmapUV(std::vector<rade::vector3>& polyPoints, rade::plane3d::EPlaneAxis bestAxis)
{
    switch (bestAxis)
    {
    case rade::plane3d::EPlaneAxis::EPlaneAxis_YZ:
        for (auto& point : polyPoints)
        {
            point.lmU = point.y;
            point.lmV = point.z;
        }
        break;

    case rade::plane3d::EPlaneAxis::EPlaneAxis_XZ:
        for (auto& point : polyPoints)
        {
            point.lmU = point.x;
            point.lmV = point.z;
        }
        break;

    case rade::plane3d::EPlaneAxis::EPlaneAxis_XY:
        for (auto& point : polyPoints)
        {
            point.lmU = point.x;
            point.lmV = point.y;
        }
        break;
    }
}

bool CLightmapGen::GetSunFactor(
        rade::poly3d* poly,
        rade::vector3* lumelPos,
        const rade::vector3& sunColor,
        const rade::vector3& sunDir,
        std::vector<rade::poly3d>& polyList,
        rade::vector3* outColor)
{
    bool dataModified = false;
    rade::plane3d plane = poly->GetPlane();
    rade::vector3 lightVectorFwd = (*lumelPos + sunDir) - *lumelPos;
    //rade::vector3 lightVectorBack = *lumelPos - (*lumelPos + sunDir);
    lightVectorFwd.Normalize();

    float distance = 0.0f;

    rade::vector3 fakeSunPos = *lumelPos + (lightVectorFwd * 1000);
    fakeSunPos = fakeSunPos * 2;

    // TODO: this should be changed, needs a ray cast not a line segment test
    if (!DoesLineIntersectWithPolyList(fakeSunPos, *lumelPos, polyList))
    //if(DoesRayIntersectWithPolyList(*lumelPos, lightVectorBack, polyList, &distance))
    {
        outColor->Set(outColor->x + sunColor.x / 2, outColor->y + sunColor.y /2, outColor->z + sunColor.z /2);
        dataModified = true;
    }
    if (outColor->x > 254) outColor->x = 254;
    if (outColor->y > 254) outColor->y = 254;
    if (outColor->z > 254) outColor->z = 254;
    return dataModified;
}

bool CLightmapGen::GetAmbientFactor(
        rade::poly3d* poly,
        rade::vector3* lumelPos,
        const std::vector<rade::Light>& lights,
        std::vector<rade::poly3d>& polyList,
        rade::vector3* outColor)
{
    rade::plane3d plane = poly->GetPlane();
    shpheremap_t* sphere = GetSphereRaysForNormal(plane.GetNormal());

    int numhits = 0;
    float avgDist = 0;
    for (uint16_t i = 0; i < m_options.numSphereRays; i++)
    {
        rade::vector3 testPos = (*lumelPos + sphere->rays.at(i));
        float distance = 0;
        if (DoesLineIntersectWithPolyList(testPos, *lumelPos, polyList, &distance))
        {
            numhits++;
            avgDist += distance;
        }
    }
    avgDist /= (m_options.numSphereRays);

    float shadeAmt = avgDist * 40;

    if (shadeAmt > 255)
        shadeAmt = 255;

    //rade::Log("shadeAmt = %f\n", shadeAmt);

    outColor->x = outColor->x - shadeAmt;
    outColor->y = outColor->y - shadeAmt;
    outColor->z = outColor->z - shadeAmt;

    if (outColor->x > 254) outColor->x = 254;
    if (outColor->y > 254) outColor->y = 254;
    if (outColor->z > 254) outColor->z = 254;

    if (outColor->x < 0) outColor->x = 0;
    if (outColor->y < 0) outColor->y = 0;
    if (outColor->z < 0) outColor->z = 0;

    // AO is always generated
    return true;
}

bool CLightmapGen::GetShadowFactor(
        rade::poly3d* poly,
        rade::vector3* lumelPos,
        const std::vector<rade::Light>& lights,
        std::vector<rade::poly3d>& polyList,
        rade::vector3* outColor)
{
    bool dataModified = false;

    rade::plane3d plane = poly->GetPlane();

    for (auto& light :lights)
    {
        rade::vector3 lightVector = *lumelPos - light.pos;
        lightVector.Normalize();
        float distanceFromLightToLumel = light.pos.Distance(*lumelPos);
        float radius = light.radius;

        if (distanceFromLightToLumel < radius)
        {
            if (plane.ClassifyPoint(light.pos) == rade::math::ESide_FRONT)
            {
                // do a ray test on this vector with the polyset, if it doesnt intersect
                // set the light, otherwise leave it at "m_options.shadowUnlit" colour
                if (!DoesLineIntersectWithPolyList(light.pos, *lumelPos, polyList))
                {
                    float intensity = (radius / distanceFromLightToLumel) - 1.0f;
                    float r = (light.color[0] * light.brightness) * intensity;
                    float g = (light.color[1] * light.brightness) * intensity;
                    float b = (light.color[2] * light.brightness) * intensity;

                    outColor->Set(std::min(outColor->x + r, 255.0f), std::min(outColor->y + g, 255.0f), std::min(outColor->z + b, 255.0f));
                    dataModified = true;
                }
            }
        }
    }
    return dataModified;
}

int CLightmapGen::GenerateLightmap(rade::poly3d* poly, std::vector<rade::poly3d>& polyList,
        const std::vector<rade::Light>& lights,
        CLightmapImg* lightmap)
{
    rade::plane3d plane = poly->GetPlane();
    std::vector<rade::vector3>& polyPoints = poly->GetPointListRef();
    rade::plane3d::EPlaneAxis bestAxis = plane.GetPlaneAxis();

    // calc new planar mapped uvs for lightmap (world position values based on best/closest axis)
    CalcLightmapUV(polyPoints, bestAxis);

    // convert the world based vertex positions to texture space ones (0-1 range)
    float uvMin[2];
    float uvMax[2];
    uint16_t lightmapWidth = 0;
    uint16_t lightmapHeight = 0;
    NormalizeLightmapUVs(polyPoints, uvMin, uvMax, &lightmapWidth, &lightmapHeight);

    lightmapWidth = static_cast<uint16_t>(lightmapWidth * m_options.lmDetail);
    lightmapHeight = static_cast<uint16_t>(lightmapHeight * m_options.lmDetail);

    lightmap->Allocate(lightmapWidth, lightmapHeight);

    // calculate the edge vectors to interpolate over later
    rade::vector3 edge1, edge2, UVVector;
    CalcEdgeVectors(plane, uvMin, uvMax, edge1, edge2, UVVector);

    // now that we have the two edge vectors, we can find the lumel positions in world space by
    // interpolating along these edges using the width and height of the lightmap
    LumelData lumelData(lightmapWidth, lightmapHeight);

    bool dataModified = false;

    for (int iX = 0; iX < lightmapWidth; iX++)
    {
        for (int iY = 0; iY < lightmapHeight; iY++)
        {
            float ufactor = ((float)iX / (float)lightmapWidth) + 0.0025f;
            float vfactor = ((float)iY / (float)lightmapHeight) + 0.0025f;

            rade::vector3 newedge1, newedge2;
            newedge1 = edge1 * ufactor;
            newedge2 = edge2 * vfactor;
            lumelData.SetPosition(iX, iY, UVVector + newedge2 + newedge1);

            rade::vector3* lumelPos = lumelData.GetPosition(iX, iY);

            rade::vector3 finalColour((float)m_options.shadowUnlit, (float)m_options.shadowUnlit,
                    (float)m_options.shadowUnlit);

            bool hasShadows = false;
            bool hasSun = false;
            bool hasAmbient = false;

            if(m_options.createShadows)
                hasShadows = GetShadowFactor(poly, lumelPos, lights, polyList, &finalColour);

            if(m_options.createSun)
                hasSun = GetSunFactor(poly, lumelPos, rade::vector3(m_options.sunColour), rade::vector3(m_options.sunDir), polyList, &finalColour);

            if(m_options.createAO)
                hasAmbient = GetAmbientFactor(poly, lumelPos, lights, polyList, &finalColour);

            if(hasAmbient || hasShadows || hasSun)
                dataModified = true;

            lumelData.SetColor(iX, iY, finalColour);
        }
    }

    if (dataModified)
    {
        for (int iX = 0; iX < lightmapWidth; iX++)
        {
            for (int iY = 0; iY < lightmapHeight; iY++)
            {
                lightmap->SetPixel(iX, iY, *lumelData.GetColor(iX, iY));
            }
        }

        rade::Image bm(lightmapWidth, lightmapHeight, rade::Image::Format_RGBA, lightmap->m_data);
        for (int i = 0; i < m_options.postBlur; i++)
            bm.Blur();

//        static int counter = 0;
//        char fname[128];
//        sprintf(fname, "smooth%d.png", counter);
//        counter++;
//        bm.SavePNG(fname);

        unsigned char* pixBuff = bm.GetPixelBuffer();
        memcpy(lightmap->m_data, pixBuff, lightmapWidth * lightmapHeight * 4);
    }
    else
    {
        for (int iX = 0; iX < lightmapWidth; iX++)
        {
            for (int iY = 0; iY < lightmapHeight; iY++)
            {
                rade::vector3 p((float)m_options.shadowUnlit, (float)m_options.shadowUnlit,
                        (float)m_options.shadowUnlit);
                lightmap->SetPixel(iX, iY, p);
            }
        }
    }
    return dataModified;
}

void CLightmapGen::GenerateLMData(unsigned char val, CLightmapImg& lm)
{
    lm.Allocate(32, 32);
    for (int iX = 0; iX < lm.m_width; iX++)
    {
        for (int iY = 0; iY < lm.m_height; iY++)
        {
            rade::vector3 p(val, val, val);
            lm.SetPixel(iX, iY, p);
        }
    }
}

int CLightmapGen::GenerateLightMapDataRange(
        std::vector<rade::poly3d>& polyList,
        const std::vector<rade::Light>& lights,
        threaddata_t* threadData)
{
    for (unsigned int i = threadData->startIndex; i < threadData->endIndex; i++)
    {
        rade::poly3d& poly = polyList.at(i);
        auto* lm = new CLightmapImg();
        bool hasShadows = GenerateLightmap(&poly, polyList, lights, lm);

        if (hasShadows)
        {
            m_lmMutex.lock();
            // copy the ptr to the shared list, get an index and quickly get out of here
            m_lightMapList.emplace_back(lm);
            uint32_t lmIndex = static_cast<uint32_t>(m_lightMapList.size()) - 1;
            m_lmMutex.unlock();
            poly.SetLightmapDataIndex(lmIndex);
        }
        else
        {
            poly.SetLightmapDataIndex(0);
            delete lm;
        }
        threadData->completedItems++;
    }
    return 0;
}

void CLightmapGen::ThreadWorkerLightmapRange(
        std::vector<rade::poly3d>* polyList,
        const std::vector<rade::Light>* lights,
        threaddata_t* threadData)
{
    GenerateLightMapDataRange(*polyList, *lights, threadData);
}

void CLightmapGen::ThreadStatusUpdate(
        threaddata_t* threadData,
        uint16_t numThreads,
        uint16_t totalItems)
{
    bool complete = false;
    uint16_t totalDone;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        totalDone = 0;
        for (uint16_t i = 0; i < numThreads; i++)
            totalDone += threadData[i].completedItems;

        float pctComplete = static_cast<float>(totalDone) / static_cast<float>(totalItems) * 100;
        //PrintProgress(pctComplete);

        if (totalDone >= totalItems)
            complete = true;

        m_progress = static_cast<int>(pctComplete);
        NotifyCallbacks();

    } while (!complete);
    m_progress = 0;
}

int CLightmapGen::Generate(
        lmoptions_t lampOptions,
        std::vector<rade::poly3d>& polyList,
        const std::vector<rade::Light>& lights,
        std::vector<CLightmapImg*>* lightMapList)
{
    // copy options
    m_options = lampOptions;

    uint16_t processor_count = std::thread::hardware_concurrency();
    if (processor_count == 0)
    {
        processor_count = 1;
    }

    // FOR SANE DEBUGGING
    //processor_count = 1;

    threaddata_t threadData[/*processor_count*/ 128];
    rade::Log("Spawning %i threads\n", processor_count);

    auto polyCount = static_cast<unsigned int>(polyList.size());
    unsigned int range = polyCount / processor_count;

    // generate simple black lightmap to use for all polys that have no lights affecting them
    auto* lmBlack = new CLightmapImg();
    GenerateLMData(m_options.shadowUnlit, *lmBlack);
    m_lightMapList.push_back(lmBlack);

    std::vector<std::thread> workers;
    for (int i = 0; i < processor_count; i++)
    {
        threadData[i].startIndex = i * range;
        threadData[i].endIndex = threadData[i].startIndex + range;

        // if last thread, process to end of list (rounding due to divide of items/num threads)
        bool isLastRange = (i + 1 == processor_count);
        if (isLastRange)
        {
            threadData[i].endIndex = polyCount;
        }
        threadData[i].completedItems = 0;

        workers.emplace_back(&CLightmapGen::ThreadWorkerLightmapRange,
                this, &polyList, &lights, &threadData[i]);
    }

    std::thread statusThread(&CLightmapGen::ThreadStatusUpdate, this,
            &threadData[0], processor_count, polyCount);

    for (std::thread& t: workers)
    {
        if (t.joinable())
            t.join();
    }
    statusThread.join();

    for (auto sphere : m_spheres)
        delete sphere;

    // copy the pointers to the returned list
    for (auto& j : m_lightMapList)
        lightMapList->push_back(j);

    return 0;
}

//#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
//#define PBWIDTH 60
//
//void PrintProgress(double percentage)
//{
//    int val = (int)(percentage * 100);
//    int lpad = (int)(percentage * PBWIDTH);
//    int rpad = PBWIDTH - lpad;
//    OS::Log("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
//    fflush(stdout);
//}