// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	Array<zTPreemptingCache*> zTPreemptingCache::arrCaches;

  zTPreemptingCache::zTPreemptingCache() {
    Reset();
    arrCaches += this;
    frameCache = new zVEC3[preemptingCacheSize];
  }
  


  void zTPreemptingCache::FrameRecord( zVEC3 position ) {
    uint rframe = ++frame % preemptingCacheSize;
    frameCache[rframe] = position;
  }



  void zTPreemptingCache::Reset() {
    frame = Invalid;
    timer[0u].Delete();
  }



  void zTPreemptingCache::GetVector( zVEC3& vector ) {
    if( frame <= 1 )
      return;

    uint begin = frame >= preemptingCacheSize ? frame - preemptingCacheSize + 1 : 0;
    uint end   = frame;

    for( uint i = begin; i < end; i++ ) {
      zVEC3& pos1 = frameCache[( i + 0 ) % preemptingCacheSize];
      zVEC3& pos2 = frameCache[( i + 1 ) % preemptingCacheSize];
      vector += pos2 - pos1;

      // debug
      if( debug_mode_enabled && debug_show_preempting )
        zlineCache->Line3D( pos1, pos2, GFX_GREY, 0 );
    }

    vector /= (float)(end - begin);
  }


  
  // search best position for preemption
  zVEC3 zTPreemptingCache::SearchGroundPosition( zVEC3 source ) {
    // startup ray vector
    float y         = target->GetPositionWorld()[VY];
    zVEC3 rayStart  = zVEC3( source[VX], y + 300.0f, source[VZ] );
    zVEC3 rayVector = zVEC3( 0.0f, -800.0f, 0.0f );

    // search ground under source
    oCWorld* world = ogame->GetGameWorld();
    int result = world->TraceRayNearestHit(
      rayStart,
      rayVector,
      (zCVob*)Null,
      zTRACERAY_POLY_NORMAL | zTRACERAY_VOB_IGNORE_NO_CD_DYN | zTRACERAY_VOB_IGNORE_CHARACTER );

    // calculate best position
    if( result ) {
      zVEC3 intersection    = world->traceRayReport.foundIntersection;
      zTBBox3D bbox3D       = target->GetModel()->bbox3D;
      float bboxHeight      = bbox3D.maxs[VY] - bbox3D.mins[VY];
      zVEC3 groundElevation = zVEC3( 0.0f, bboxHeight * 0.5f, 0.0f );
      zVEC3 groundPosition  = intersection + groundElevation;
      return groundPosition;
    }

    return source;
  }



  // search position after time
  zVEC3 zTPreemptingCache::PreemptPositionAfter( float time ) {
    zVEC3 vector;
    GetVector( vector );
    float timeLeft = (float)(min( frame, preemptingCacheSize ) * preemptingUpdateFrequency);

    zVEC3 singleVector    = vector / preemptingUpdateFrequencyF;
    zVEC3 preemptVector   = singleVector * time;
    zVEC3 preemptPosition = target->GetPositionWorld() + preemptVector;
    zVEC3 groundPosition  = SearchGroundPosition( preemptPosition );

    // debug
    if( debug_mode_enabled && debug_show_preempting ) {
      zlineCache->Line3D( preemptPosition, preemptPosition + zVEC3( 0.0f, 20.0f, 0.0f ), GFX_RED,   0 );
      zlineCache->Line3D( groundPosition,  groundPosition  + zVEC3( 0.0f, 50.0f, 0.0f ), GFX_GREEN, 0 );
    }

    return groundPosition;
  }



  void zTPreemptingCache::SetTarget( oCNpc* npc ) {
    target = npc;
  }



  void zTPreemptingCache::Update() {
    if( !target )
      return;

    // timed recording
    timer.ClearUnused();
    timer[0u].Suspend( ogame->IsOnPause() );

    if( timer[0u].Await( preemptingUpdateFrequency, true ) )
      FrameRecord( target->GetPositionWorld() );
    
    // debug
    if( debug_mode_enabled && debug_show_preempting )
      PreemptPositionAfter( debug_show_preempting_time );
  }
  


  zTPreemptingCache::~zTPreemptingCache() {
    delete frameCache;
    arrCaches -= this;
  }



  // create new or addref preempting record for npc
  void zTPreemptingCache::BeginPreemption( oCNpc* target ) {
    for( uint i = 0; i < arrCaches.GetNum(); i++ ) {
      zTPreemptingCache* cache = arrCaches[i];
      if( cache->target == target ) {
        cache->AddRef();
        return;
      }
    }

    zTPreemptingCache* cache = new zTPreemptingCache();
    cache->SetTarget( target );
  }



  // release preempting record for npc
  void zTPreemptingCache::EndPreemption( oCNpc* target ) {
    for( uint i = 0; i < arrCaches.GetNum(); i++ ) {
      zTPreemptingCache* cache = arrCaches[i];
      if( cache->target == target ) {
        cache->Release();
        return;
      }
    }
  }



  // search preempting record for npc
  zTPreemptingCache* zTPreemptingCache::FindPreemption( oCNpc* target ) {
    for( uint i = 0; i < arrCaches.GetNum(); i++ ) {
      zTPreemptingCache* cache = arrCaches[i];
      if( cache->target == target )
        return cache;
    }

    return Null;
  }
  


  // delete all records
  void zTPreemptingCache::ClearCacheList() {
    while( arrCaches.GetNum() )
      arrCaches.GetLast()->ForceDelete();
  }


  
  // test function
  static void SwitchPreemptingForNpc( oCNpc* npc ) {
    zTPreemptingCache* preempting = zTPreemptingCache::FindPreemption( npc );
    if( preempting )
      zTPreemptingCache::EndPreemption( npc );
    else
      zTPreemptingCache::BeginPreemption( npc );
  }



  // test function
  static void PreemptingTest() {
    if( zKeyToggled( KEY_NUMPAD1 ) && player->GetFocusNpc() )
      SwitchPreemptingForNpc( player->GetFocusNpc() );

    if( zKeyToggled( KEY_NUMPAD2 ) )
      SwitchPreemptingForNpc( player );
  }



  void zTPreemptingCache::OnTick_Global() {
    // debug
    if( debug_mode_enabled && debug_switch_preempting )
      PreemptingTest();

    for( uint i = 0; i < arrCaches.GetNum(); i++ )
      arrCaches[i]->Update();
  }
}