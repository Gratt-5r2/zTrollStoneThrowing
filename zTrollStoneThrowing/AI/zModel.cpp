// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	HOOK Ivk_zCModel_DoAniEvents PATCH( &zCModel::DoAniEvents, &zCModel::DoAniEvents_Union );



  void zCModel::DoAniEvents_Union( zCModelAniActive* active ) {
    // check validations
    if( !homeVob || !homeVob->homeWorld )
      return;

    // do original logic
    THISCALL( Ivk_zCModel_DoAniEvents )( active );
    
    // define target npcs
    oCNpc* self = zDYNAMIC_CAST<oCNpc>( homeVob );
    if( !self )
      return;

    zCVob* target = self->focus_vob ? self->focus_vob : self->enemy;
    oCNpc* victim = target->CastTo<oCNpc>();
    if( !victim )
      return;


    for( int i = 0; i < occuredAniEvents.GetNum(); i++ ) {
      zCModelAniEvent* event = occuredAniEvents[i];
      if( event->aniEventType != zMDL_EVENT_TAG )
        continue;

      // register preempting loop for this npc
      if( A event->tagString == ani_event_tro_begin_preemption ) {
        zTPreemptingCache::BeginPreemption( victim );
        continue;
      }

      // release preempting loop for this npc
      if( A event->tagString == ani_event_tro_end_preemption ) {
        zTPreemptingCache::EndPreemption( victim );
        continue;
      }

      // create stone visual in troll hand
      if( A event->tagString == ani_event_tro_stonetake ) {
        zCModelNodeInst* node = self->GetModel()->SearchNode( Z npc_slot_r_hand );
        if( node ) {
          zCVisual* visual = zCVisual::LoadVisual( Z tro_stone_visual );
          node->SetNodeVisualS( visual, False );
        }

        continue;
      }

      // remove stone visual from troll hand and throw AI object
      if( A event->tagString == ani_event_tro_stonethrow ) {
        zTPreemptingCache* cache = zTPreemptingCache::FindPreemption( victim );
        zVEC3 selfPosition   = self->GetPositionWorld();
        zVEC3 victimPosition = victim->GetPositionWorld();
        float victimDistance = selfPosition.Distance( victimPosition ) * 0.01f; // centimeters to meters
        float flyTime        = ( victimDistance / ai_stone_speed ) * 1000.0f;   // seconds to milliseconds
        
        zVEC3 begin = selfPosition;
        zVEC3 end = victimPosition;
        if( cache )
          end = cache->PreemptPositionAfter( flyTime );
        
        zCModelNodeInst* pNode = self->GetModel()->SearchNode( Z npc_slot_r_hand );
        if( pNode ) {
          pNode->SetNodeVisualS( Null, True );
          zMAT4 vMat4 = self->GetTrafoModelNodeToWorld( pNode );
          begin = vMat4.GetTranslation();
        }

        oCVobThrowable::CreateVob( self, Z tro_stone_visual, begin, end, ai_stone_speed );
        continue;
      }

      // put gobbo to troll hand
      if( A event->tagString == ani_event_tro_gobbotake ) {
        oCNpc* gobbo = oCNpc::ThrownGobbos[self];
        self->PutNpcToSlot( npc_slot_r_hand, gobbo );
        continue;
      }

      //
      if( A event->tagString == ani_event_tro_gobbothrow ) {
        oCNpc* gobbo = self->RemoveNpcFromSlot( npc_slot_r_hand );

        if( gobbo && gobbo->enemy ) {

          zVEC3 direction   = (gobbo->enemy->GetPositionWorld() - self->GetPositionWorld()).Normalize();
          zVEC3 endPosition =  gobbo->enemy->GetPositionWorld() - direction * 100.0f;

          zTParabolicPath_Gobbo::Create( gobbo, endPosition );
          oCNpc::ThrownGobbos.Remove( gobbo );
        }
      }
    }
  }
}