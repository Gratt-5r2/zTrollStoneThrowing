// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

void Fighting_Union();
void InitByScript_Union( int instance, int reload );
Array<oCNpc*> CollectNearestGobbos();
oCNpc* GetNearestValidGobbo();
bool AI_TrollCallsGobbo();
void AI_GobboGotoTroll( oCNpc* troll );
void PutNpcToSlot( string slotName, oCNpc* npc );
oCNpc* RemoveNpcFromSlot( string slotName );
void ResetNpcPhysics();
void SetAnimationSpeedMultiplier( string aniName, float speedMultiplier );
bool IsTrowed();
bool CanBeTrollOwner( oCNpc* gobbo );

#if ENGINE == Engine_G2
int IsDead() { return attribute[NPC_ATR_HITPOINTS] <= 0; }
#endif

static Map<oCNpc*, oCNpc*> ThrownGobbos;