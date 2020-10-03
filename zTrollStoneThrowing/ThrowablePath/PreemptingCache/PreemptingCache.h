// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	class zTPreemptingCache : public zCObject {
    static Array<zTPreemptingCache*> arrCaches;

    Timer timer;
    oCNpc* target;
    zVEC3* frameCache;
    uint frame;
  public:

    zTPreemptingCache();
    void FrameRecord( zVEC3 position );
    void Reset();
    void GetVector( zVEC3& vector );
    zVEC3 SearchGroundPosition( zVEC3 source );
    zVEC3 PreemptPositionAfter( float time );
    void SetTarget( oCNpc* npc );
    void Update();
    virtual ~zTPreemptingCache();

    static void BeginPreemption( oCNpc* target );
    static void EndPreemption( oCNpc* target );
    static zTPreemptingCache* FindPreemption( oCNpc* target );
    static void ClearCacheList();
    static void OnTick_Global();
  };
}