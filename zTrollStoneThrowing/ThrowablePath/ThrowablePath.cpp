// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	Array<oCVobThrowable*> oCVobThrowable::arrThrowableVobs;
  static float motionSpeed = 0.0f;


  
  // set the owner vob of this path
  void zTThrowablePath::SetConnectedVob( oCVobThrowable* vob ) {
    connectedVob = vob;
    InsertDefaultIgnoredVobs();
  }


  
  // clear list of vobs, which ignored by traceray
  void zTThrowablePath::ClearIngoredVobs() {
    arrIgnoredVobs.EmptyList();
    InsertDefaultIgnoredVobs();
  }



  // insert this owner vob and its owner npc to the ignore list
  void zTThrowablePath::InsertDefaultIgnoredVobs() {
    if( connectedVob )
      arrIgnoredVobs.Insert( connectedVob->GetOwner() );

    for( uint i = 0; i < oCVobThrowable::arrThrowableVobs.GetNum(); i++ )
      arrIgnoredVobs.Insert( oCVobThrowable::arrThrowableVobs[i] );
    
  }


  
  // to damage victim npc by entity speed
  void zTThrowablePath::OnDamage( oCNpc* npc ) {
    // debug
    if( debug_mode_enabled && debug_dont_hit_player && npc == player )
      return;

    // dont hit trolls
    if( npc->guild == NPC_GIL_TROLL )
      return;

    float damage = ai_damage_multiplier;
    float totalDamage = damage * GetSpeed() * motionSpeed;

    if( totalDamage >= 5.0f )
      npc->GetEM( False )->OnDamage( npc, connectedVob->GetOwner(), damage * GetSpeed() * motionSpeed, oEDamageType_Fly, GetPosition() );
  }



  // return connected vob
  oCVobThrowable* zTThrowablePath::GetConnectedVob() {
    return connectedVob;
  }



  // return connected vob owner
  oCNpc* zTThrowablePath::GetConnectedVobOwner() {
    if( connectedVob )
      return connectedVob->GetOwner();

    return Null;
  }



  // hit effect
  static void PlayDustPFX( zVEC3 position ) {
    zCVob* pfxVob = new zCVob();
    zCParticleFX* pfx = zCParticleFX::Load( "PFX_DUST" );

    ogame->GetGameWorld()->AddVob( pfxVob );
    pfxVob->dontWriteIntoArchive = True;
    pfxVob->SetSleeping( False );
    pfxVob->SetPositionWorld( position );
    pfxVob->SetVisual( pfx );
    pfx->CreateParticles();

    pfxVob->Release();
    pfx->Release();
  }



  // check collisions in the current iteration
  bool32 zTThrowablePath::CheckWorldIntersection() {
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
      if( speed >= 0.4f ) {
        PlayDustPFX( GetPosition() );
        if( speed >= 0.7f ) zsound->PlaySound3D( "SFX_TRO_STONE_FALL",      connectedVob, 0, Null ); // heavy hit for a ground
        else                zsound->PlaySound3D( "SFX_TRO_STONE_SMALLFALL", connectedVob, 0, Null ); // small hit for a ground
      }

      return True;
    }

    return False;
  }



  // check collisions with npcs
  oCNpc* zTThrowablePath::CheckNpcsIntersection() {
    oCWorld* world = ogame->GetGameWorld();
    int result = world->TraceRayNearestHit(
      oldPosition,
      motionVector,
      &arrIgnoredVobs,
      zTRACERAY_STAT_IGNORE | zTRACERAY_VOB_IGNORE_NO_CD_DYN | zTRACERAY_VOB_BBOX );

    if( result ) {
      zCVob* vob = world->traceRayReport.foundVob;
      if( vob->CastTo<oCNpc>() ) {
        oCNpc* npc = (oCNpc*)vob;
        arrIgnoredVobs.Insert( vob );
        return npc;
      }
    }

    return Null;
  }



  // return hit status
  bool32 zTThrowablePath::FoundHit() {
    return (bool32&)collisionReport.foundHit;
  }



  zVEC3 zTThrowablePath::GetHitPosition() {
    return collisionReport.foundIntersection;
  }



  zVEC3 zTThrowablePath::GetHitNormal() {
    return collisionReport.foundPoly ?
      collisionReport.foundPoly->polyPlane.normal :
      collisionReport.foundPolyNormal;
  }
}