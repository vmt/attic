//-----------------------------------------------------------------------------
// cpu.h - Cpu Stat
//-----------------------------------------------------------------------------
#ifndef _CPU_H_
#define _CPU_H_

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>

#ifdef __FreeBSD_version
#if __FreeBSD_version < 500101
#include <kvm.h>
#include <sys/dkstat.h>
#else
#include <sys/resource.h>
#endif
//-----------------------------------------------------------------------------
// CPU Object - Implementation For FreeBSD Systems
//-----------------------------------------------------------------------------
class Cpu
{
private:
	kvm_t *csKd;
	struct nlist csNList[2];
	int pre_used, pre_total;
public:
	enum Error
	{
		eInitFailed,
		eReadFailed
	};
  
	Cpu(); ~Cpu();
	int load();
};
#else
//-----------------------------------------------------------------------------
// CPU Object - Implementation For Generic Systems.
//-----------------------------------------------------------------------------
class Cpu
{

public:
	Cpu() {	srandrom(random());}
	int load() { return (random()) % 100; }
};
#endif
#endif
