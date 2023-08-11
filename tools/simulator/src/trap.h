#pragma once

typedef enum {
  UserSoftware = 0,
  SupervisorSoftware = 1,
  MachineSoftware = 3,
  UserTimer = 4,
  SupervisorTimer = 5,
  MachineTimer = 7,
  UserExternal = 8,
  SupervisorExternal = 9,
  MachineExternal = 11,
} InterruptType;

typedef enum {
  InstructionAddressMisaligned = 0,
  InstructionAcessFault = 1,
  IllegalInstruction = 2,
  Breakpoint = 3,
  LoadAddressMisaligned = 4,
  LoadAddressFault = 5,
  StoreAMOAddressMisaligned = 6,
  StoreAMOAccessFault = 7,
  EnvironmentCallFromUMode = 8,
  EnvironmentCallFromSMode = 9,
  EnvironmentCallFromMMode = 11,
  InstructionPageFault = 12,
  LoagPageFault = 13,
  StoreAMOPageFault = 15,
} ExceptionType;

