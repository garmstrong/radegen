#include <cstring>
#include <thread>
#include <chrono>
#include "point3d.h"
#include "lightmapgen.h"
#include "plane3d.h"
#include "image.h"
#include "rmath.h"

//using namespace rade;

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

CLightmapGen::sphereMap_t* CLightmapGen::GetSphereRaysForNormal(const rade::vector3& normal)
{
    for (auto& m_sphere : m_spheres)
    {
        if (m_sphere->normal == normal)
        {
            return m_sphere;
        }
    }

    auto* newMap = new sphereMap_t;
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

void CLightmapGen::GenerateHemisphereRay(const rade::vector3& normal, rade::vector3* ret)
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

bool CLightmapGen::DoesLineIntersectWithPolyList(const rade::vector3& lightPos, const rade::vector3& lumelPos,
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

bool CLightmapGen::DoesLineIntersectWithPolyList(const rade::vector3& lightPos, const rade::vector3& lumelPos,
        const std::vector<rade::poly3d>& polyList, float* distance)
{
    int count = 0;

    for (const rade::poly3d& poly: polyList)
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
                {
                    *distance = hitPos.Distance(lightPos);
                    return true;
                }
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

int CLightmapGen::CalcShadowLightmap(rade::poly3d* poly, std::vector<rade::poly3d>& polyList, const std::vector<rade::Light>& lights,
        CLightmapImg* lightmap) const
{
    //rade::plane3d plane(poly);
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

            rade::vector3 color((float)m_options.shadowUnlit, (float)m_options.shadowUnlit, (float)m_options.shadowUnlit);

            for (auto& light :lights)
            {
                rade::vector3* lumelPos = lumelData.GetPosition(iX, iY);

                rade::vector3 lightVector = *lumelPos - light.pos;
                lightVector.Normalize();
                float distanceFromLightToLumel = light.pos.Distance(*lumelPos);
                float radius = light.radius;

                if (distanceFromLightToLumel < radius)
                {
                    if (plane.ClassifyPoint(light.pos) == rade::math::ESide_FRONT)
                    {
                        // do a ray test on this vector with the polyset, if it doesnt intersect
                        // set the light, otherwise leave black!
                        //if (RayIntersect(&lights[i], lumelData->pos[iX][iY], brushList) == 0)
                        if (!DoesLineIntersectWithPolyList(light.pos, *lumelPos, polyList))
                        {
                            float intensity = (radius / distanceFromLightToLumel) - 1.0f;
                            float r = (light.color[0] * light.brightness) * intensity;
                            float g = (light.color[1] * light.brightness) * intensity;
                            float b = (light.color[2] * light.brightness) * intensity;

                            color.Set(color.x + r, color.y + g, color.z + b);
                            dataModified = true;
                        }
                    }
                }
            }
            if (color.x > 254) color.x = 254;
            if (color.y > 254) color.y = 254;
            if (color.z > 254) color.z = 254;

            lumelData.SetColor(iX, iY, color);
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
        for(int i=0;i<m_options.postBlur; i++)
            bm.Blur();

//
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
                rade::vector3 p((float)m_options.shadowUnlit, (float)m_options.shadowUnlit, (float)m_options.shadowUnlit);
                lightmap->SetPixel(iX, iY, p);
            }
        }
    }
    return dataModified;
}

int CLightmapGen::CalcSunLightmap(rade::poly3d* poly, std::vector<rade::poly3d>& polyList, const rade::vector3& sunDir,
        const rade::vector3& sunColor,
        CLightmapImg& lightmap) const
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

    lightmap.Allocate(lightmapWidth, lightmapHeight);

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
            float ufactor = ((float)iX / (float)lightmapWidth); //  - 0.1f;
            float vfactor = ((float)iY / (float)lightmapHeight); //- 0.1f;

            rade::vector3 newedge1, newedge2;
            newedge1 = edge1 * ufactor;
            newedge2 = edge2 * vfactor;
            lumelData.SetPosition(iX, iY, UVVector + newedge2 + newedge1);

            rade::vector3 color(m_options.shadowUnlit, m_options.shadowUnlit, m_options.shadowUnlit);


            rade::vector3* lumelPos = lumelData.GetPosition(iX, iY);

            rade::vector3 lightVector = (*lumelPos + sunDir) - *lumelPos;
            lightVector.Normalize();
            //float distanceFromLightToLumel = light.pos.Distance(*lumelPos);
            //float radius = light.radius;

            //if (distanceFromLightToLumel < radius)
            {
                // TODO: is this the best way to do this?
                rade::vector3 fakeSunPos = *lumelPos + (lightVector * 1000);
                fakeSunPos = fakeSunPos * 2000;

                //if (plane.ClassifyPoint(fakeSunPos) == RMATH::ESide_FRONT)
                {
                    // do a ray test on this vector with the polyset, if it doesnt intersect
                    // set the light, otherwise leave black!
                    //if (RayIntersect(&lights[i], lumelData->pos[iX][iY], brushList) == 0)
                    if (!DoesLineIntersectWithPolyList(fakeSunPos, *lumelPos, polyList))
                    {
                        color.Set(color.x + sunColor.x, color.y + sunColor.y, color.z + sunColor.z);
                        dataModified = true;
                    }
                }
            }

            if (color.x > 254) color.x = 254;
            if (color.y > 254) color.y = 254;
            if (color.z > 254) color.z = 254;

            lumelData.SetColor(iX, iY, color);
        }
    }

    if (dataModified)
    {
        for (int iX = 0; iX < lightmapWidth; iX++)
        {
            for (int iY = 0; iY < lightmapHeight; iY++)
            {
                lightmap.SetPixel(iX, iY, *lumelData.GetColor(iX, iY));
            }
        }

        rade::Image bm(lightmapWidth, lightmapHeight, rade::Image::Format_RGBA, lightmap.m_data);
//        for (uint16_t i = 0; i < 3; i++)
//            bm.Blur();
//
//        static int counter = 0;
//        char fname[128];
//        sprintf(fname, "smooth%d.png", counter);
//        counter++;
//        bm.SavePNG(fname);

        unsigned char* pixBuff = bm.GetPixelBuffer();
        memcpy(lightmap.m_data, pixBuff, lightmapWidth * lightmapHeight * 4);
    }
    else
    {
        for (int iX = 0; iX < lightmapWidth; iX++)
        {
            for (int iY = 0; iY < lightmapHeight; iY++)
            {
                rade::vector3 p(m_options.shadowUnlit, m_options.shadowUnlit, m_options.shadowUnlit);
                lightmap.SetPixel(iX, iY, p);
            }
        }
    }
    return dataModified;
}

int CLightmapGen::CalcPolyAmbientOcclusion(rade::poly3d* poly, std::vector<rade::poly3d>& polyList, CLightmapImg& lightmap)
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

    lightmap.Allocate(lightmapWidth, lightmapHeight);

    // calculate the edge vectors to interpolate over later
    rade::vector3 edge1, edge2, UVVector;
    CalcEdgeVectors(plane, uvMin, uvMax, edge1, edge2, UVVector);

    // now that we have the two edge vectors, we can find the lumel positions in world space by
    // interpolating along these edges using the width and height of the lightmap
    LumelData lumelData(lightmapWidth, lightmapHeight);

    bool dataModified = false;

    sphereMap_t* sphere = GetSphereRaysForNormal(plane.GetNormal());

    for (int iX = 0; iX < lightmapWidth; iX++)
    {
        for (int iY = 0; iY < lightmapHeight; iY++)
        {
            // 0.0025f -> perfect for detail of 1.0 0.7 0.4
            // 2.0 (noticed white line in images, but not in game, so moved down to 0.0015 and all good)
            // 0.0015f -> 2.0 OK, 0.4 OK

            float ufactor = ((float)iX / (float)lightmapWidth) + 0.0015f;
            float vfactor = ((float)iY / (float)lightmapHeight) + 0.0015f;

            rade::vector3 newedge1 = edge1 * ufactor;
            rade::vector3 newedge2 = edge2 * vfactor;
            lumelData.SetPosition(iX, iY, UVVector + newedge2 + newedge1);

            rade::vector3 color(m_options.shadowUnlit, m_options.shadowUnlit, m_options.shadowUnlit);
            dataModified = true;

            rade::vector3* lumelPos = lumelData.GetPosition(iX, iY);

            int numhits = 0;
            float avgDist = 0;
            for (uint16_t i = 0; i < m_options.numSphereRays; i++)
            {
                rade::vector3 testPos = (*lumelPos + /*sphereRays[i]*/ sphere->rays.at(i));
                float distance = 0;
                if (DoesLineIntersectWithPolyList(testPos, *lumelPos, polyList, &distance))
                {
                    numhits++;
                    avgDist += distance;
                }
            }
            avgDist /= (m_options.numSphereRays);

            float shadeAmt = avgDist * 40;

            //shadeAmt *= 1.5;
            if (shadeAmt > 254)
                shadeAmt = 254;

            color.x = color.x - shadeAmt;
            color.y = color.y - shadeAmt;
            color.z = color.z - shadeAmt;


            if (color.x > 254) color.x = 254;
            if (color.y > 254) color.y = 254;
            if (color.z > 254) color.z = 254;

            if (color.x < 0) color.x = 0;
            if (color.y < 0) color.y = 0;
            if (color.z < 0) color.z = 0;

            lumelData.SetColor(iX, iY, color);
        }
    }

    if (dataModified)
    {
        for (int iX = 0; iX < lightmapWidth; iX++)
        {
            for (int iY = 0; iY < lightmapHeight; iY++)
            {
                rade::vector3* color = lumelData.GetColor(iX, iY);
                lightmap.SetPixel(iX, iY, *color);
            }
        }
    }

    rade::Image bm(lightmapWidth, lightmapHeight, rade::Image::Format_RGBA, lightmap.m_data);
    //bm.Blur();
    static int counter = 0;
    char fname[128];
    sprintf(fname, "smooth%d.png", counter);
    counter++;
    //bm.SavePNG(fname);
    unsigned char* pixBuff = bm.GetPixelBuffer();
    memcpy(lightmap.m_data, pixBuff, lightmapWidth * lightmapHeight * 4);

    return dataModified;
}

void CLightmapGen::GenerateLMData(unsigned char val, CLightmapImg& lm)
{
    lm.Allocate(64, 64);

    for (int iX = 0; iX < lm.m_width; iX++)
    {
        for (int iY = 0; iY < lm.m_height; iY++)
        {
            rade::vector3 p(val, val, val);
            lm.SetPixel(iX, iY, p);
        }
    }
}

int CLightmapGen::GenerateLightMapDataRange(std::vector<rade::poly3d>& polyList,
        const std::vector<rade::Light>& lights,
        threadData_t* threadData)
{
    for (unsigned int i = threadData->startIndex; i < threadData->endIndex; i++)
    {
        rade::poly3d& poly = polyList.at(i);

        bool writeLM = false;


//		CLightmapImg lmAO;
//		vector3 sunDir(0.1, 0.6, 0.3);
//		vector3 sunColor(102, 178, 255);
//		CalcSunLightmap(&poly, polyList, sunDir, sunColor, lmAO);
//		writeLM = true;




          int hasAmbient = 0;
//        CLightmapImg lmAO;
//        hasAmbient = CalcPolyAmbientOcclusion(&poly, polyList, lmAO);
//        writeLM = true;

        int hasShadows = 0;
        CLightmapImg *lmShadow = new CLightmapImg();
        hasShadows = CalcShadowLightmap(&poly, polyList, lights, lmShadow);
        if (hasShadows)
        {
            // lmShadow contains shadow pixel data
            writeLM = true;
        }
        else
        {
            // no lights touched, use standard black lightmap, unless ambient already there
            // lmShadow will have unlit default texture data, we need to mix it into lmAO
            if (!hasAmbient)
            {
                // no ambient - so just blank out LM
                poly.SetLightmapDataIndex(0);
            }
        }

        //lmAO.Combine(lmShadow);
        //lmShadow.Free();

        if (writeLM)
        {
            // BEGIN LOCK
            m_lmMutex.lock();
            // copy the ptr to the shared list, get an index and quickly get out of here
            //m_lightMapList.push_back(lmAO);
            m_lightMapList.push_back(lmShadow);
            uint32_t lmIndex = static_cast<uint32_t>(m_lightMapList.size()) - 1;
            m_lmMutex.unlock();
            // END LOCK
            poly.SetLightmapDataIndex(lmIndex);
        }
        else
        {
            delete lmShadow;
        }

        threadData->completedItems++;
    }
    return 0;
}

void CLightmapGen::ThreadWorkerLightmapRange(std::vector<rade::poly3d>* polyList,
        const std::vector<rade::Light>* lights,
        threadData_t* threadData,
        int threadID)
{
    //rade::Log("Thread %i processing from %i - %i\n", threadID, startIndex, endIndex);
    GenerateLightMapDataRange(*polyList, *lights, threadData);
}

void CLightmapGen::ThreadStatusUpdate(threadData_t* threadData, uint16_t numThreads, uint16_t totalItems)
{
    bool complete = false;
    uint16_t totalDone = 0;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        totalDone = 0;
        for (uint16_t i = 0; i < numThreads; i++)
        {
            totalDone += threadData[i].completedItems;
        }

        float pctComplete = float(totalDone) / float(totalItems);
        //PrintProgress(pctComplete);

        if (totalDone >= totalItems)
            complete = true;

        m_progress = static_cast<int>(pctComplete*100.0f);
        NotifyCallbacks();

    } while (!complete);

    m_progress = 0;
}

int CLightmapGen::GenerateLightmaps(
        NRadeLamp::lmOptions_t lampOptions,
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

    threadData_t threadData[/*processor_count*/ 128];
    printf("spawning %i threads\n", processor_count);

    unsigned int polyCount = static_cast<unsigned int>(polyList.size());
    unsigned int range = polyCount / processor_count;

    // generate simple black lightmap to use for all polys that have no lights affecting them
    CLightmapImg *lmBlack = new CLightmapImg();
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
                this, &polyList, &lights, &threadData[i], i);
    }

    std::thread statusThread(&CLightmapGen::ThreadStatusUpdate, this,
            &threadData[0], processor_count, polyCount);

    for (std::thread& t: workers)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    statusThread.join();

    for (auto sphere : m_spheres)
    {
        delete sphere;
    }

    // copy the pointers to the returned list
    for (auto& j : m_lightMapList)
    {
        lightMapList->push_back(j);
    }
    return 0;
}
