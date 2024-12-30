# lhook
PoC Library inline-hooking using LD_PRELOAD (to load the engine).

In ```hook.c```  can be inserted hooks using ```NIX_register(<library name>, <symbol>, fake_<...>, <should be used as return value>)```
