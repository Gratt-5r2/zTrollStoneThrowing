// Supported with union (c) 2020 Union team

// User API for zCObject
// Add your methods here

void ForceDelete() {
  refCtr = 1;
  Release();
}