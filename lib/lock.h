#ifndef LOCK_H
#define LOCK_H

#include"type.h"

void init_all_spin_lock();
void init_spin_lock(u32 * g_spin_lock);
void get_spin_lock(u32 * g_spin_lock);
void release_spin_lock(u32 * g_spin_lock);

extern u32 * my_lock;

#endif
