// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
  class oCVobThrowable;

  class zTThrowablePath {
    friend class oCVobThrowable;
  protected:
    oCVobThrowable* connectedVob;
    zCArray<zCVob*> arrIgnoredVobs;
    zTTraceRayReport collisionReport;
    zVEC3 oldPosition;
    zVEC3 motionVector;
  public:

    void SetConnectedVob( oCVobThrowable* vob );
    void ClearIngoredVobs();
    void InsertDefaultIgnoredVobs();
    void OnDamage( oCNpc* npc );
    bool32 CheckWorldIntersection();
    oCNpc* CheckNpcsIntersection();
    oCVobThrowable* GetConnectedVob();
    oCNpc* GetConnectedVobOwner();
    virtual void Update() = 0;
    virtual zVEC3 GetPosition() = 0;
    virtual zVEC3 GetVector() = 0;
    virtual float GetSpeed() = 0;
    bool32 FoundHit();
    zVEC3 GetHitPosition();
    zVEC3 GetHitNormal();
  };
}