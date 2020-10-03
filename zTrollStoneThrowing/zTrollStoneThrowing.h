// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	// preemption
  uint preemptingCacheSize;
  uint preemptingUpdateFrequency;
  float preemptingUpdateFrequencyF;

  // animation tags
  string ani_event_tro_begin_preemption = "TRO_BEGIN_PREEMPTION";
  string ani_event_tro_end_preemption   = "TRO_END_PREEMPTION";
  string ani_event_tro_stonetake        = "TRO_STONETAKE";
  string ani_event_tro_stonethrow       = "TRO_STONETHROW";
  string ani_event_tro_gobbotake        = "TRO_GOBBOTAKE";
  string ani_event_tro_gobbothrow       = "TRO_GOBBOTHROW";
  string tro_overlay_name               = "TRO_STONETHROW.MDS";

  // ai
  float ai_stone_speed;
  float ai_trowing_speed_multiplier;
  float ai_throwing_distance;
  float ai_damage_multiplier;
  float ai_gobbo_trow_distance = 850.0f;

  // visual
  string npc_slot_r_hand  = "BIP01 R FINGER1"; // "BIP01 R HAND";
#if ENGINE >= Engine_G2
  string tro_stone_visual = "TRO_STONE_G2.3DS";
#else
  string tro_stone_visual = "TRO_STONE.3DS";
#endif

  // debug
  bool debug_mode_enabled;
  bool debug_show_preempting;
  float debug_show_preempting_time;
  bool debug_switch_preempting;
  bool debug_throw_stone;
  bool debug_dont_hit_player;
  int guild_troll_id;
  int guild_gobbo_id;



  void InitOptions() {
    COption& opt = Union.GetGameOption();
    opt.Read( (int32&)preemptingCacheSize,       "ZTROLLSTONETHROWING_PREEMPTING", "preemptingCacheSize",       10 );
    opt.Read( (int32&)preemptingUpdateFrequency, "ZTROLLSTONETHROWING_PREEMPTING", "preemptingUpdateFrequency", 80 );
    preemptingUpdateFrequencyF = (float)preemptingUpdateFrequency;
  
    opt.Read( ai_stone_speed,              "ZTROLLSTONETHROWING_AI", "stone_speed",              20.0f );
    opt.Read( ai_trowing_speed_multiplier, "ZTROLLSTONETHROWING_AI", "trowing_speed_multiplier", 1.15f );
    opt.Read( ai_throwing_distance,        "ZTROLLSTONETHROWING_AI", "throwing_distance",        1000.0f );
    opt.Read( ai_damage_multiplier,        "ZTROLLSTONETHROWING_AI", "damage_multiplier",        10.0f );

    opt.Read( debug_mode_enabled,         "ZTROLLSTONETHROWING_DEBUG", "debug_mode_enabled",      false );
    opt.Read( debug_show_preempting,      "ZTROLLSTONETHROWING_DEBUG", "show_preempting_trails",  true );
    opt.Read( debug_show_preempting_time, "ZTROLLSTONETHROWING_DEBUG", "show_preempting_by_time", 1000.0f );
    opt.Read( debug_switch_preempting,    "ZTROLLSTONETHROWING_DEBUG", "switch_preempting",       true );
    opt.Read( debug_throw_stone,          "ZTROLLSTONETHROWING_DEBUG", "throw_stone_rmb",         true );
    opt.Read( debug_dont_hit_player,      "ZTROLLSTONETHROWING_DEBUG", "dont_hit_player",         false );

    opt.SaveAll();
  }



  int InitGuildID( string guildName, int default ) {
    int guild = default;
    zCPar_Symbol* sym = parser->GetSymbol( guildName );
    if( sym )
      sym->GetValue( guild, 0 );

    return guild;
  }
}