// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
  Timer FadeTimer;

	class oCVobThrowable : public oCVob {
    friend class zTThrowablePath;
    static Array<oCVobThrowable*> arrThrowableVobs;

    oCNpc* owner;
    zVEC3 angularVector;
    zTThrowablePath* path;
    zCVob* pfxVob;
    zCParticleFX* pfx;
    bool fadeOut;
    int fadeSpeed;
  public:

    oCVobThrowable();
    void SetOwner( oCNpc* npc );
    oCNpc* GetOwner();
    void Begin( const zVEC3& begin, const zVEC3& end, const float& speed );
    void Update();
    virtual ~oCVobThrowable();

    static void OnTick_Global();
    static oCVobThrowable* CreateVob(
      oCNpc* owner,
      const zSTRING& visual,
      const zVEC3& begin,
      const zVEC3& end,
      const float speed
    );

    static void ClearAll();
  };
}