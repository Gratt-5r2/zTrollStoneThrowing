// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
  class zTParabolicPath : public zTThrowablePath {
  protected:
    zVEC3 begin;
    zVEC3 end;
    float length;
    zVEC3 vector;
    zVEC3 position;
    float elevation;
    zVEC3 projection;
    float time;
    float speed;
  public:

    zTParabolicPath();
    void Begin( const zVEC3& begin, const zVEC3& end, const float& speed, float elevation = 0.1f );
    virtual void Update();
    virtual zVEC3 GetPosition();
    virtual zVEC3 GetVector();
    virtual float GetSpeed();
  };



  class zTParabolicPath_Gobbo : public zTParabolicPath {
    oCNpc* gobbo;
  public:

    zTParabolicPath_Gobbo();
    void Begin( const zVEC3& begin, const zVEC3& end, const float& speed, float elevation = 0.1f );
    bool32 CheckWorldIntersection();
    virtual void Update();
    void KnockOff( oCNpc* npc );

    static Array<zTParabolicPath_Gobbo*> GobboPaths;
    static void OnTick_Global();
    static zTParabolicPath_Gobbo* Create( oCNpc* npc, zVEC3 end );
    static void ClearAll();
  };

}