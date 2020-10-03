// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  Map<oCNpc*, oCNpc*> oCNpc::ThrownGobbos;


  static string T_STONETHROW = "T_STONETHROW";
  static string T_GOBBOTHROW = "T_GOBBOTHROW";
  static string T_WARN       = "T_WARN";
  static string T_COMEHERE   = "T_COMEHERE";


  void oCNpc::SetAnimationSpeedMultiplier( string aniName, float speedMultiplier ) {
    int aniID = GetModel()->GetAniIDFromAniName( aniName );
    if( aniID != Invalid ) {
      zCModelAni* ani = GetModel()->GetAniFromAniID( aniID );
      if( ani )
        ani->fpsRate = ani->fpsRateSource * speedMultiplier;
    }
  }



  HOOK Ivk_oCNpc_Fighting AS( &oCNpc::Fighting, &oCNpc::Fighting_Union );

  void oCNpc::Fighting_Union() {
    if( guild != guild_troll_id )
      return THISCALL( Ivk_oCNpc_Fighting )();

    zCVob* target = focus_vob ? focus_vob : enemy;
    oCNpc* npc = target->CastTo<oCNpc>();
    if( npc && npc->attribute[NPC_ATR_HITPOINTS] <= 0 )
      return THISCALL( Ivk_oCNpc_Fighting )();

    if( GetDistanceToVob( *target ) > ai_throwing_distance ) {
      bool gobboAction = false;
      if( AI_TrollCallsGobbo() ) {
        oCNpc* gobbo = ThrownGobbos[this];
        Turn( gobbo->GetPositionWorld() );

        if( !GetModel()->IsAnimationActive( T_COMEHERE ) ) {
          GetModel()->StartAnimation( T_COMEHERE );
          gobbo->AI_GobboGotoTroll( this );
        }

        if( GetDistanceToVob( *gobbo ) > 250.0f )
          return;

        gobboAction = true;
      }

      gobboAction ?
        SetAnimationSpeedMultiplier( T_GOBBOTHROW, ai_trowing_speed_multiplier ) :
        SetAnimationSpeedMultiplier( T_STONETHROW, ai_trowing_speed_multiplier );
      
      oCMsgMovement* turnMessage = new oCMsgMovement( oCMsgMovement::EV_TURNTOVOB, target );
      oCMsgConversation* throwMessage = new oCMsgConversation( oCMsgConversation::EV_PLAYANI_NOOVERLAY, gobboAction ? T_GOBBOTHROW : T_STONETHROW );
      GetEM( False )->OnMessage( turnMessage, this );
      GetEM( False )->OnMessage( throwMessage, this );
    
    }
    else
      return THISCALL( Ivk_oCNpc_Fighting )();
  }



  HOOK Ivk_oCNpc_InitByScript PATCH( &oCNpc::InitByScript, &oCNpc::InitByScript_Union );

  static int GetScriptValue( string symName, int default ) {
    int value = default;
    zCPar_Symbol* sym = parser->GetSymbol( "AIV_MM_ThreatenBeforeAttack" );
    if( sym )
      sym->GetValue( value, 0 );
    
    return value;
  }

  void oCNpc::InitByScript_Union( int instance, int reload ) {
    THISCALL( Ivk_oCNpc_InitByScript )( instance, reload );

    if( guild == guild_troll_id ) {
      parser->SetInstance( "SELF", this );
      parser->CallFunc( "G_TROLLPOSTINIT" );
      ApplyOverlay( tro_overlay_name );
    }
  }



  Array<oCNpc*> oCNpc::CollectNearestGobbos() {
    Array<oCNpc*> gobbos;
    zCArray<zCVob*> vobList;
    CreateVobList( vobList, 1000 );

    for( int i = 0; i < vobList.GetNum(); i++ ) {
      oCNpc* npc = vobList[i]->CastTo<oCNpc>();
      if( npc && !npc->IsDead() && npc->guild == guild_gobbo_id && !npc->IsTrowed() && CanBeTrollOwner( npc ) )
        gobbos += npc;
    }

    return gobbos;
  }



  oCNpc* oCNpc::GetNearestValidGobbo() {
    oCNpc* nearestGobbo = Null;
    float minDistance = float_MAX;

    Array<oCNpc*> gobbos = CollectNearestGobbos();
    for( uint i = 0; i < gobbos.GetNum(); i++ ) {
      float distance = GetDistanceToVob( *gobbos[i] );

      if( distance < ai_gobbo_trow_distance && CanSee( gobbos[i], True ) ) {
        if( !nearestGobbo || distance < minDistance ) {
          nearestGobbo = gobbos[i];
          minDistance = distance;
        }
      }
    }

    return nearestGobbo;
  }



  bool oCNpc::AI_TrollCallsGobbo() {
    if( !ThrownGobbos[this].IsNull() ) {
      oCNpc* npc = ThrownGobbos[this];
      if( !npc->IsDead() && !IsTrowed() && CanBeTrollOwner( npc ) && GetDistanceToVob( *npc ) <= (ai_gobbo_trow_distance + 100.0f) )
        return true;
    }

    oCNpc* gobbo = GetNearestValidGobbo();
    if( !gobbo )
      return false;

    gobbo->AI_GobboGotoTroll( this );
    ThrownGobbos.Insert( this, gobbo );
    return true;
  }



  void oCNpc::AI_GobboGotoTroll( oCNpc* troll ) {
    enemy = troll->enemy;
    anictrl->SetWalkMode( ANI_WALKMODE_RUN );
    GetEM( 0 )->Clear();
    GetEM( 0 )->OnMessage( new oCMsgMovement( oCMsgMovement::EV_GOTOVOB, troll ), troll );
  }



  void oCNpc::PutNpcToSlot( string slotName, oCNpc* npc ) {
    TNpcSlot* slot = GetInvSlot( slotName );
    if( !slot ) {
      CreateInvSlot( slotName );
      slot = GetInvSlot( slotName );
    }

    PutInSlot( slot, npc, False );
  }



  oCNpc* oCNpc::RemoveNpcFromSlot( string slotName ) {
    TNpcSlot* slot = GetInvSlot( slotName );
    oCNpc* npc = slot->vob->CastTo<oCNpc>();
    if( slot )
      slot->ClearVob();

    return npc;
  }



  void oCNpc::ResetNpcPhysics() {
    SetPhysicsEnabled( True );
    GetRigidBody()->gravityOn = true;
  }



  bool oCNpc::IsTrowed() {
    return !GetRigidBody()->gravityOn ? true : false;
  }



  bool oCNpc::CanBeTrollOwner( oCNpc* gobbo ) {
    if( gobbo->enemy == this || !gobbo->enemy || gobbo->enemy->guild != NPC_GIL_TROLL )
      return true;

    return false;
  }
}