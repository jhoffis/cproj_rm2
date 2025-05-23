#pragma once
#include "nums.h"

#define BODY_GENDER 0x00000001
#define BODY_GENDER_MALE 0
#define BODY_GENDER_FEMALE 1
#define BODY_RACE   0x0000000E
#define BODY_HUNGER   0x000000F0 // 15 == extremely hungry

typedef u32 body_t;

typedef struct {
    const char *name;
    body_t body;
} npc_t;
