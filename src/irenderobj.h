#include "point3d.h"
#include "transform.h"

namespace rade
{
    class Camera;
}

class IRenderObj
{
public:
    IRenderObj()
    = default;

    virtual ~IRenderObj()
    = default;

    virtual rade::Transform& GetTransform()
    {
        return m_transform;
    }

    virtual void SetEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    virtual bool GetEnabled() const
    {
        return m_enabled;
    }

    virtual void SetCamera(rade::Camera* camera)
    {
        m_camera = camera;
    }

    virtual void GetBoundingBox() = 0;

protected:
    bool m_enabled = false;
    rade::Camera* m_camera = nullptr;
    rade::Transform m_transform;
};

