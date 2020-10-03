// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
  class zTPhysicalPath : public zTThrowablePath {
    zVEC3 position;
    zVEC3 vector;
    float speed;
  public:

    void Begin( const zVEC3& begin, const zVEC3& vector, const float& speed );
    virtual void Update();
    virtual zVEC3 GetPosition();
    virtual zVEC3 GetVector();
    virtual float GetSpeed();
  };
}