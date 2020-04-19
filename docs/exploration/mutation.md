Three mutation techniques/patterns can be used:
* Builder: pass object in. call mutation methods. those work on a copy. extract changed object. 
  hurts performance due to making a copy.
* Guard: pass object in. call mutation methods. current object has to revert it's state after guard is destructed.
* Mutator: pass object in. can call mutations. current object changes.
