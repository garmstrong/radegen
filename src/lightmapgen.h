#pragma once

#include <mutex>
#include <vector>
#include <functional>
#include "polygon3d.h"
#include "plane3d.h"
#include "lightmapimage.h"
#include "lumeldata.h"
#include "light3d.h"

namespace rade
{
    class textmesh;
};

namespace NRadeLamp
{


};

typedef std::function<void(int)> cb_t;

class CLightmapGen
{
private:


    typedef struct
    {
        unsigned int startIndex;
        unsigned int endIndex;
        uint16_t completedItems;
    } threaddata_t;

    typedef struct
    {
        rade::vector3 normal;
        std::vector<rade::vector3> rays;
    } shpheremap_t;

public:

    typedef struct
    {
        int numSphereRays;
        float sphereSize;
        int shadowLit;
        int shadowUnlit;
        float lmDetail;
        bool createAO;
        bool createShadows;
        int postBlur;
        bool createSun;
        float sunColour[3];
        float sunDir[3];
    } lmoptions_t;

    // generate lightmaps
    int Generate(
            lmoptions_t lampOptions,
            std::vector<rade::poly3d>& polyList,
            const std::vector<rade::Light>& lights,
            std::vector<CLightmapImg*>* lightMapList);

    // register status callback
    void RegisterCallback(const cb_t& cb)
    {
        // add callback to end of callback list
        m_callbacks.push_back(cb);
    }

protected:


    std::vector<CLightmapImg*> m_lightMapList;
    std::vector<cb_t> m_callbacks;
    int m_progress = 0;

    // call all the registered callbacks
    void NotifyCallbacks() const
    {
        for (const auto& cb : m_callbacks)
        {
            cb(m_progress);
        }
    }

    lmoptions_t m_options = {
            30,     // numSphereRays
            7.0f,   // spheresize
            200,    // lit
            10,     // unlit
            0.6f,   // lmDetail
            true,   // AO
            true,   // shadows
            1,      // blur
            true,   // genereate sun
            { 102, 178, 255 },  // sun colour
            { 0.1f, 0.6f, 0.3f }   // sun dir
    };

    std::mutex m_lmMutex;

    std::vector<shpheremap_t*> m_spheres;

    shpheremap_t* GetSphereRaysForNormal(const rade::vector3& normal);

    static void GenerateHemisphereRay(const rade::vector3& normal, rade::vector3* ret);

    static bool DoesLineIntersectWithPolyList(
            const rade::vector3& lightPos,
            const rade::vector3& lumelPos,
            const std::vector<rade::poly3d>& polyList);

    bool DoesLineIntersectWithPolyList(
            const rade::vector3& lightPos,
            const rade::vector3& lumelPos,
            const std::vector<rade::poly3d>& polyList,
            float* distance);

    void CalcEdgeVectors(
            const rade::plane3d& plane,
            const float* uvMin,
            const float* uvMax,
            rade::vector3& edge1,
            rade::vector3& edge2,
            rade::vector3& UVVector);

    void NormalizeLightmapUVs(
            std::vector<rade::vector3>& polyPoints,
            float* minimums,
            float* maximums,
            uint16_t* polyUWidth,
            uint16_t* polyVHeight);

    void CalcLightmapUV(std::vector<rade::vector3>& polyPoints, rade::plane3d::EPlaneAxis bestAxis);

    void GenerateLMData(unsigned char val, CLightmapImg& lm);

    int GenerateLightMapDataRange(
            std::vector<rade::poly3d>& polyList,
            const std::vector<rade::Light>& lights,
            threaddata_t* threadData);

    void ThreadWorkerLightmapRange(
            std::vector<rade::poly3d>* polyList,
            const std::vector<rade::Light>* lights,
            threaddata_t* threadData);

    void ThreadStatusUpdate(threaddata_t* threadData, uint16_t numThreads, uint16_t totalItems);

    bool GetShadowFactor(
            rade::poly3d* poly,
            rade::vector3* lumelPos,
            const std::vector<rade::Light>& lights,
            std::vector<rade::poly3d>& polyList,
            rade::vector3* color);

    bool GetAmbientFactor(
            rade::poly3d* poly,
            rade::vector3* lumelPos,
            const std::vector<rade::Light>& lights,
            std::vector<rade::poly3d>& polyList,
            rade::vector3* outColor);

    bool GetSunFactor(
            rade::poly3d* poly,
            rade::vector3* lumelPos,
            const rade::vector3& sunColor,
            const rade::vector3& sunDir,
            std::vector<rade::poly3d>& polyList,
            rade::vector3* outColor);

    int GenerateLightmap(rade::poly3d* poly,
            std::vector<rade::poly3d>& polyList,
            const std::vector<rade::Light>& lights,
            CLightmapImg* lightmap);

    bool DoesRayIntersectWithPolyList(
            const rade::vector3& pos,
            const rade::vector3& ray,
            const std::vector<rade::poly3d>& polyList,
            float* distance);
};

