# lhook
PoC Library inline-hooking using LD_PRELOAD (to load the engine).

In ```hook.c```  can be inserted hooks using ```NIX_register(<library name>, <symbol>, fake_<...>, <should be used as return value>)```

## To run
```
make
LD_PRELOAD=./main.o ip a
```
### Notes
```hook_fix_orig``` is incomplete - in case of istructions with imm (immediate) size less than the new (patched) one.
