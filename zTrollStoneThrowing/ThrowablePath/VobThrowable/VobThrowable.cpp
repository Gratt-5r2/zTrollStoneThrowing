// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	oCVobThrowable::oCVobThrowable() : oCVob() {
    path = Null;
    owner = Null;
    angularVector = 0.01f;
    dontWriteIntoArchive = True;
    arrThrowableVobs += this;
    fadeOut = false;
    fadeSpeed = 2000;
    visualAlpha = 1.0f;
  }
  


  // stone owner for the damage registration
  void oCVobThrowable::SetOwner( oCNpc* npc ) {
    owner = npc;
  }


  
  // return owner
  oCNpc* oCVobThrowable::GetOwner() {
    return owner;
  }
  


  // preparation of initial values
  void oCVobThrowable::Begin( const zVEC3& begin, const zVEC3& end, const float& speed ) {
    if( path )
      return;

    // first of paths is a basic parabolic algorithm
    zTParabolicPath* parabolicPath = new zTParabolicPath();
    parabolicPath->SetConnectedVob( this );
    parabolicPath->Begin( begin, end, speed );
    path = parabolicPath;
  }



  void oCVobThrowable::Update() {
    if( !path )
      return;

    //
    if( fadeOut && FadeTimer[this].Await( fadeSpeed ) ) {
      fadeSpeed = 10;
      visualAlphaEnabled = True;
      if( (visualAlpha -= 0.01f) <= 0 ) {
        return;
      }
    }

    // update path collision and position
    path->Update();

    // check collisions
    if( path->FoundHit() ) {
      // check path type
      zTParabolicPath* parabolicPath = dynamic_cast<zTParabolicPath*>( path );
      if( parabolicPath ) {
        // create the break vector
        zVEC3 position  = parabolicPath->GetPosition();
        zVEC3 vector    = parabolicPath->GetVector();
        float speed     = parabolicPath->GetSpeed();
        zVEC3 hitVector = parabolicPath->GetHitNormal() + vector.Normalize();

        // replace parabolic path to
        // basic physical simulation
        delete parabolicPath;
        zTPhysicalPath* plysicalPath = new zTPhysicalPath();
        plysicalPath->SetConnectedVob( this );
        plysicalPath->Begin( position, hitVector, speed * 0.8f );
        path = plysicalPath;
        fadeOut = true;
      }
      else {
        // TO DO
      }

      // calculate angle speed for rotate simulation
      angularVector = path->GetVector() * path->GetSpeed();
    }
    
    
    // calculate ritation parameters (approximate ;D)
    float radiusCorrection = 1.5f;
    zVEC3 rotationVector   = zVEC3( angularVector[VZ], 0, -angularVector[VX] );
    float rotationSpeed    = angularVector.Length() * motionSpeed * radiusCorrection;

    // apply rotation and position
    static zVEC3 objectElevation( 0.0f, 25.0f, 0.0f );
    RotateWorld( rotationVector, rotationSpeed );
    SetPositionWorld( path->GetPosition() + objectElevation );
  }



  oCVobThrowable::~oCVobThrowable() {
    if( path )
      delete path;

    arrThrowableVobs -= this;
  }


  
  // test function
  static void ThrowingTest() {
    static oCVobThrowable* vob = Null;
    static bool pressed = false;
    if( zinput->GetMouseButtonPressedRight() ) {
      if( !pressed ) {
        pressed = true;
        oCNpc* focusNpc = player->GetFocusNpc();
        zVEC3 begin = player->GetPositionWorld();
        zVEC3 end = focusNpc ?
          focusNpc->GetPositionWorld() :
          player->GetPositionWorld() + ogame->GetCameraVob()->GetAtVectorWorld() * 3000.0f;

        vob = oCVobThrowable::CreateVob( player, Z tro_stone_visual, begin, end, ai_stone_speed );
      }
    }
    else if( !zinput->GetMouseButtonPressedRight() && pressed )
      pressed = false;
  }



  void oCVobThrowable::OnTick_Global() {
    FadeTimer.ClearUnused();

    // debug
    if( debug_mode_enabled && debug_throw_stone )
      ThrowingTest();

    motionSpeed = ztimer->frameTimeFloat * ztimer->factorMotion;

    for( uint i = 0; i < arrThrowableVobs.GetNum(); i++ ) {
      oCVobThrowable* vob = arrThrowableVobs[i];
      if( vob->visualAlpha > 0.0f )
        vob->Update();
    }
  }



  oCVobThrowable* oCVobThrowable::CreateVob( oCNpc* owner, const zSTRING& visual, const zVEC3& begin, const zVEC3& end, const float speed ) {
    oCVobThrowable* vob = new oCVobThrowable();
    ogame->GetGameWorld()->AddVob( vob );
    vob->SetSleeping( False );
    vob->SetVisual( visual );
    vob->SetOwner( owner );
    vob->Begin( begin, end, speed );
    return vob;
  }



  void oCVobThrowable::ClearAll() {
    arrThrowableVobs.ReleaseData();
  }
}