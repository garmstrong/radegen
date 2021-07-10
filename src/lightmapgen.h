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
    class CTextMesh;
};

namespace NRadeLamp
{
    typedef struct
    {
        int numSphereRays;
        float sphereSize;
        int shadowLit;
        int shadowUnlit;
        float lmDetail;
        bool createAO;
        bool createShadows;
    } lmOptions_t;
};

typedef std::function<void(int)> cb_t;

class CLightmapGen
{
public:
    CLightmapGen()
    {
    }

    ~CLightmapGen()
    {
    }

    // register a callback
    void RegisterCallback(const cb_t &cb)
    {
        // add callback to end of callback list
        m_callbacks.push_back(cb);
    }

protected:
    std::vector<cb_t> m_callbacks;
    int m_progress = 0;

    // call all the registered callbacks
    void NotifyCallbacks() const
    {
        for (const auto &cb : m_callbacks)
        {
            cb(m_progress);
        }
    }

    NRadeLamp::lmOptions_t m_options = {
            40,     // numSphereRays
            15.0f,  // spheresize
            200,    // lit
            120,    // unlit
            0.6f,   // lmDetail
            true,   // AO
            true    // shadows
    };

    std::mutex m_lmMutex;

    typedef struct
    {
        unsigned int startIndex;
        unsigned int endIndex;
        uint16_t completedItems;
    } threadData_t;

    std::vector<CLightmapImg> m_lightMapList;

    typedef struct
    {
        rade::vector3 normal;
        std::vector<rade::vector3> rays;
    } sphereMap_t;

    std::vector<sphereMap_t*> m_spheres;

    //void PrintProgress(double percentage);

    //float RandomFloat(float a, float b);

    sphereMap_t* GetSphereRaysForNormal(const rade::vector3& normal);

    static void GenerateHemisphereRay(const rade::vector3& normal, rade::vector3* ret);

    static bool DoesLineIntersectWithPolyList(const rade::vector3& lightPos, const rade::vector3& lumelPos,
            const std::vector<rade::CPoly3D>& polyList);

    static bool DoesLineIntersectWithPolyList(const rade::vector3& lightPos, const rade::vector3& lumelPos,
            const std::vector<rade::CPoly3D>& polyList, float* distance);

    static void
    CalcEdgeVectors(const rade::plane3d& plane, const float* uvMin, const float* uvMax, rade::vector3& edge1, rade::vector3& edge2,
            rade::vector3& UVVector);

    static void
    NormalizeLightmapUVs(std::vector<rade::vector3>& polyPoints, float* minimums, float* maximums, uint16_t* polyUWidth,
            uint16_t* polyVHeight);

    static void CalcLightmapUV(std::vector<rade::vector3>& polyPoints, rade::plane3d::EPlaneAxis bestAxis);

    int CalcShadowLightmap(rade::CPoly3D* poly, std::vector<rade::CPoly3D>& polyList, const std::vector<rade::Light>& lights,
            CLightmapImg& lightmap) const;

    int CalcSunLightmap(rade::CPoly3D* poly, std::vector<rade::CPoly3D>& polyList, const rade::vector3& sunDir,
            const rade::vector3& sunColor,
            CLightmapImg& lightmap) const;

    int CalcPolyAmbientOcclusion(rade::CPoly3D* poly, std::vector<rade::CPoly3D>& polyList, CLightmapImg& lightmap);

    void GenerateLMData(unsigned char val, CLightmapImg& lm);

    int GenerateLightMapDataRange(std::vector<rade::CPoly3D>& polyList,
            const std::vector<rade::Light>& lights,
            threadData_t* threadData);

    void ThreadWorkerLightmapRange(std::vector<rade::CPoly3D>* polyList,
            const std::vector<rade::Light>* lights,
            threadData_t* threadData,
            int threadID);

    void ThreadStatusUpdate(threadData_t* threadData, uint16_t numThreads, uint16_t totalItems);

public:
    int GenerateLightmaps(
            NRadeLamp::lmOptions_t lampOptions,
            std::vector<rade::CPoly3D>& polyList,
            const std::vector<rade::Light>& lights,
            std::vector<CLightmapImg>* lightMapList);

};

