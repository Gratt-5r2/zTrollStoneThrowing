// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  Array<zTParabolicPath_Gobbo*> zTParabolicPath_Gobbo::GobboPaths;



	zTParabolicPath::zTParabolicPath() {
    collisionReport.foundHit = False;
  }



  // preparation of initial values
  void zTParabolicPath::Begin( const zVEC3& begin_, const zVEC3& end_, const float& speed_m_s, float elevation_ ) {
    /* 
     * CONVERT M/S TO CM/MS
     * ----------------
     * 1M = 100CM
     * 1S = 1000MS
     * X  = CM/MS = 100 / 1000 = 0.1
     * ----------------
     * 1M/S == 0.1CM/MS
     */
    begin      = begin_;
    end        = end_;
    vector     = end - begin;
    length     = vector.Length();
    speed      = speed_m_s * 0.1f; // cm/ms
    elevation  = elevation_;
    time       = 0.0f;
    projection = begin;
    vector.Normalize();
  }


  
#pragma warning(push)
#pragma warning(disable:4244)
  // update position and collisions in the current iteration
  void zTParabolicPath::Update() {
    // define position relative
    // to the elapsed time
    time += motionSpeed;
    float distance = speed * time;
    position = begin + vector * distance;

    // define elevation on a single segment
    // and projection to world coorditates
    float fullFlyTime   = SafeDiv( length, speed ); // ms
    float eleMultiplier = fullFlyTime / 1000.0f;
    float eleAtPoint    = 2.0f / length * distance;
    float eleHeight     = length * elevation * eleMultiplier;
    float ele           = ( -powf( eleAtPoint - 1.0f, 2.0f ) + 1.0f ) * eleHeight;


    // define next vob position
    // and movement vector
    oldPosition  = projection;
    projection   = zVEC3( position[VX], position[VY] + ele, position[VZ] );
    motionVector = projection - oldPosition;

    // check collisions with npcs
    oCNpc* npc = CheckNpcsIntersection();
    if( npc )
      OnDamage( npc );
    
    // check collisions with world
    if( CheckWorldIntersection() )
      projection = collisionReport.foundIntersection;
  }
#pragma warning(pop)



  // return real vob position
  zVEC3 zTParabolicPath::GetPosition() {
    return projection;
  }



  // return motion vector
  zVEC3 zTParabolicPath::GetVector() {
    return motionVector;
  }



#pragma warning(push)
#pragma warning(disable:4244)
  // return real speed
  float zTParabolicPath::GetSpeed() {
    return SafeDiv( motionVector.Length(), motionSpeed );
  }
#pragma warning(pop)











  zTParabolicPath_Gobbo::zTParabolicPath_Gobbo() : zTParabolicPath() {
  }



  void zTParabolicPath_Gobbo::Begin( const zVEC3& begin, const zVEC3& end, const float& speed, float elevation ) {
    GobboPaths += this;
    zTParabolicPath::Begin( begin, end, speed, elevation );
  }



  bool32 zTParabolicPath_Gobbo::CheckWorldIntersection() {
    // more accurate surface search
    zVEC3 infelicity = zVEC3( motionVector ).Normalize() * 20.0f;
    zVEC3 rayStart   = oldPosition  - infelicity;
    zVEC3 rayVector  = motionVector + infelicity;

    oCWorld* world = ogame->GetGameWorld();
    int result = world->TraceRayNearestHit(
      rayStart,
      rayVector,
      &arrIgnoredVobs,
      zTRACERAY_POLY_NORMAL | zTRACERAY_VOB_IGNORE_NO_CD_DYN | zTRACERAY_VOB_IGNORE_CHARACTER );

    if( result ) {
      collisionReport = world->traceRayReport;

      // play hit effects
      float speed = GetSpeed();
      if( speed >= 0.4f )
        PlayDustPFX( GetPosition() );

      return True;
    }

    return False;
  }



#pragma warning(push)
#pragma warning(disable:4244)
  // update position and collisions in the current iteration
  void zTParabolicPath_Gobbo::Update() {
    // define position relative
    // to the elapsed time
    time += motionSpeed;
    float distance = speed * time;
    position = begin + vector * distance;

    // define elevation on a single segment
    // and projection to world coorditates
    float fullFlyTime = SafeDiv( length, speed ); // ms
    float eleMultiplier = fullFlyTime / 1000.0f;
    float eleAtPoint = 2.0f / length * distance;
    float eleHeight = length * elevation * eleMultiplier;
    float ele = (-powf( eleAtPoint - 1.0f, 2.0f ) + 1.0f) * eleHeight;


    // define next vob position
    // and movement vector
    oldPosition = projection;
    projection = zVEC3( position[VX], position[VY] + ele, position[VZ] );
    motionVector = projection - oldPosition;


    // check collisions with npcs
    oCNpc* npc = CheckNpcsIntersection();
    if( npc )
      KnockOff( npc );


    // check collisions with world
    if( CheckWorldIntersection() ) {
      gobbo->ResetNpcPhysics();
      projection = collisionReport.foundIntersection;
      GobboPaths -= this;
      delete this;
    }
    else
      gobbo->SetPositionWorld( projection );
  }
#pragma warning(pop)



  void zTParabolicPath_Gobbo::KnockOff( oCNpc* npc ) {
    // debug
    if( debug_mode_enabled && debug_dont_hit_player && npc == player )
      return;

    // dont hit trolls
    if( npc->guild == NPC_GIL_TROLL )
      return;

    npc->GetEM( False )->OnDamage( npc, gobbo, 1.0f, oEDamageType_Fly, GetPosition() );
  }



  void zTParabolicPath_Gobbo::OnTick_Global() {
    for( uint i = 0; i < GobboPaths.GetNum(); i++ )
      GobboPaths[i]->Update();
  }



  zTParabolicPath_Gobbo* zTParabolicPath_Gobbo::Create( oCNpc* npc, zVEC3 end ) {
    zTParabolicPath_Gobbo* path = new zTParabolicPath_Gobbo();
    path->gobbo = npc;
    path->Begin( npc->GetPositionWorld(), end, ai_stone_speed );
    return path;
  }



  void zTParabolicPath_Gobbo::ClearAll() {
    GobboPaths.DeleteData();
  }
}