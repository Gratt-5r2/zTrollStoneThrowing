// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// preparation of initial values
  void zTPhysicalPath::Begin( const zVEC3& begin_, const zVEC3& vector_, const float& speed_ ) {
    position = begin_;
    vector   = vector_ * speed_;
  }


  
  // update position and collisions in the current iteration
  void zTPhysicalPath::Update() {
    static zVEC3 gravity( 0.0f, -0.001f, 0.0f );
    static float collisionBraking = 0.95f;

    // reset collision report
    if( collisionReport.foundHit )
      collisionReport.foundHit = False;

    // define position relative
    // to the elapsed time
    oldPosition = position;
    position += vector * motionSpeed;
    motionVector = position - oldPosition;

    // add gravity and air resistance
    vector += gravity * motionSpeed;
    vector *= 0.997f;
    
    // check collisions with npcs
    oCNpc* npc = CheckNpcsIntersection();
    if( npc )
      OnDamage( npc );
    
    // check collisions with world
    if( CheckWorldIntersection() ) {
      // calculate collision position
      // and new fly vector
      position        = GetHitPosition();
      zVEC3 hitNormal = GetHitNormal();
      zVEC3 flyNormal = GetVector();
      float speed     = GetSpeed();
      vector          = ( hitNormal + flyNormal ) * speed * collisionBraking;

      // reset information
      // about ignored vobs
      ClearIngoredVobs();
    }
  }


  
  // return real vob position
  zVEC3 zTPhysicalPath::GetPosition() {
    return position;
  }
  


  // return motion vector
  zVEC3 zTPhysicalPath::GetVector() {
    return zVEC3( vector ).Normalize();
  }


  
  // return real speed
  float zTPhysicalPath::GetSpeed() {
    return vector.Length();
  }
}