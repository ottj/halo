void FUN_001bdf10(undefined4 param_1,int param_2)

{
  if (*(int *)(param_2 + 0x30) != 0) {
    display_assert("sound->cache_base_address==NULL","c:\\halo\\SOURCE\\cache\\xbox_sound_cache.c",
                   0x9e,1);
    system_exit(0xffffffff);
  }
  *(undefined4 *)(param_2 + 0x2c) = 0xffffffff;
  *(undefined4 *)(param_2 + 0x30) = 0;
  *(undefined4 *)(param_2 + 0x34) = param_1;
  return;
}

void FUN_001be1b0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_00119320(DAT_004e9368,param_1);
  if ((*(char *)(iVar1 + 4) != '\0') || (*(char *)(iVar1 + 5) != '\0')) {
    uVar2 = FUN_001ba1f0(*(undefined4 *)(*(int *)(iVar1 + 8) + 0x3c),*(int *)(iVar1 + 8),
                         "c:\\halo\\SOURCE\\cache\\xbox_sound_cache.c",0x141,1);
    uVar2 = FUN_0008d9d0(&DAT_005ab100,
                         "tried to delete sound %s(%s) from the cache while it was playing.",uVar2);
    display_assert(uVar2);
    system_exit(0xffffffff);
  }
  if (*(int *)(*(int *)(iVar1 + 8) + 0x2c) != param_1) {
    display_assert("cache_sound->sound->cache_block_index==block_index",
                   "c:\\halo\\SOURCE\\cache\\xbox_sound_cache.c",0x144,1);
    system_exit(0xffffffff);
  }
  *(undefined4 *)(*(int *)(iVar1 + 8) + 0x2c) = 0xffffffff;
  *(undefined4 *)(*(int *)(iVar1 + 8) + 0x30) = 0;
  FUN_001196d0(DAT_004e9368,param_1);
  return;
}