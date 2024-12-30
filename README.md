# lhook
PoC Library inline-hooking using LD_PRELOAD (to load the engine).

Hooks can be inserted in ```hook.c```  using ```NIX_register(<library name>, <symbol>, fake_<...>, <should be used as return value>)```

## To run
```
make
LD_PRELOAD=./main.o ip a
```
### Notes
```hook_fix_orig``` is incomplete - in case of an instr with ```imm``` (immediate) size (eg. DWORD) less than the new (patched) ```imm``` = Fails<br>
Which could be (partially) fixed by finding another instr which the same name and operand types (and ```imm``` of the adequate size).
