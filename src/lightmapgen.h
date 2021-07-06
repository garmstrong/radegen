#pragma once

#include <mutex>
#include <vector>
#include <functional>
#include "polygon3d.h"
#include "plane3d.h"
#include "lightmapimage.h"
#include "lumeldata.h"

class CLight
{
public:
    std::string name;
    CPoint3D pos;
    CPoint3D orientation;
    float radius = 128.0f;
    float brightness = 100.0f;
    float color[3] = { 1.0f, 1.0f, 1.0f };
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
    int m_progress;

    // call all the registered callbacks
    void NotifyCallbacks() const
    {
        for (const auto &cb : m_callbacks)
        {
            cb(m_progress);
        }
    }

    NRadeLamp::lmOptions_t m_options = {
            40,        // numSphereRays
            15,        // spheresize
            200,    // lit
            120,    // unlit
            0.6,    // lmDetail
            true,    // AO
            true    // shadows

    };

    std::mutex m_lmMutex;

    typedef struct
    {
        int startIndex;
        int endIndex;
        uint16_t completedItems;
    } threadData_t;

    std::vector<CLightmapImg> m_lightMapList;

    typedef struct
    {
        CPoint3D normal;
        std::vector<CPoint3D> rays;
    } sphereMap_t;

    std::vector<sphereMap_t*> m_spheres;

    //void PrintProgress(double percentage);

    //float RandomFloat(float a, float b);

    sphereMap_t* GetSphereRaysForNormal(const CPoint3D& normal);

    static void GenerateHemisphereRay(const CPoint3D& normal, CPoint3D* ret);

    static bool DoesLineIntersectWithPolyList(const CPoint3D& lightPos, const CPoint3D& lumelPos,
            const std::vector<CPoly3D>& polyList);

    static bool DoesLineIntersectWithPolyList(const CPoint3D& lightPos, const CPoint3D& lumelPos,
            const std::vector<CPoly3D>& polyList, float* distance);

    static void
    CalcEdgeVectors(const CPlane3D& plane, const float* uvMin, const float* uvMax, CPoint3D& edge1, CPoint3D& edge2,
            CPoint3D& UVVector);

    static void
    NormalizeLightmapUVs(std::vector<CPoint3D>& polyPoints, float* minimums, float* maximums, uint16_t* polyUWidth,
            uint16_t* polyVHeight);

    static void CalcLightmapUV(std::vector<CPoint3D>& polyPoints, CPlane3D::EPlaneAxis bestAxis);

    int CalcShadowLightmap(CPoly3D* poly, std::vector<CPoly3D>& polyList, const std::vector<CLight>& lights,
            CLightmapImg& lightmap) const;

    int CalcSunLightmap(CPoly3D* poly, std::vector<CPoly3D>& polyList, const CPoint3D& sunDir,
            const CPoint3D& sunColor,
            CLightmapImg& lightmap) const;

    int CalcPolyAmbientOcclusion(CPoly3D* poly, std::vector<CPoly3D>& polyList, CLightmapImg& lightmap);

    void GenerateLMData(unsigned char val, CLightmapImg& lm);

    int GenerateLightMapDataRange(std::vector<CPoly3D>& polyList,
            const std::vector<CLight>& lights,
            threadData_t* threadData);

    void ThreadWorkerLightmapRange(std::vector<CPoly3D>* polyList,
            const std::vector<CLight>* lights,
            threadData_t* threadData,
            int threadID);

    void ThreadStatusUpdate(threadData_t* threadData, uint16_t numThreads, uint16_t totalItems);

public:
    int GenerateLightmaps(
            NRadeLamp::lmOptions_t lampOptions,
            std::vector<CPoly3D>& polyList,
            const std::vector<CLight>& lights,
            std::vector<CLightmapImg>* lightMapList);

};

