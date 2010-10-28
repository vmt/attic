//-----------------------------------------------------------------------------
// cpu-freebsd.cc - Cpu Object Implementation for FreeBSD Systems.
//-----------------------------------------------------------------------------
#include <cstring>
#include <sys/param.h>
#include <cpu.hh>
#include <error.hh>


#ifdef __FreeBSD_version

static struct nlist _nlist[] = { {"_cp_time"}, {NULL} };

//-----------------------------------------------------------------------------
// Cpu() - Constructor
//-----------------------------------------------------------------------------
Cpu::Cpu(): csKd(NULL)
{
	memcpy(csNList, _nlist, sizeof(_nlist));    
	if((csKd = ::kvm_open(NULL, NULL, NULL, O_RDONLY, "kvm_open")) == NULL)
		throw Error(Cpu::eInitFailed, "Failed kvm_open().");  	
	kvm_nlist(csKd, _nlist);  
	if (_nlist[0].n_type == 0)
		throw Error(Cpu::eInitFailed, "Failed symbol extraction().");
}

//-----------------------------------------------------------------------------
// ~Cpu() - Destructor
//-----------------------------------------------------------------------------
Cpu::~Cpu()
{
  ::kvm_close(csKd);
}

//-----------------------------------------------------------------------------
// load() - Returns the cpu load.
//-----------------------------------------------------------------------------
int Cpu::load()
{	
	int used, total, result;
	unsigned long int cpu_time[CPUSTATES];

	if (kvm_read(csKd, _nlist[0].n_value, &cpu_time, 
		     sizeof(cpu_time)) != sizeof(cpu_time))
		throw Error(Cpu::eReadFailed, "Failed kvm_read().");

	used  = cpu_time[CP_USER] + cpu_time[CP_SYS];
	used += cpu_time[CP_NICE];
	total = used + cpu_time[CP_IDLE];

	if (pre_total == 0)
		result = 0;	 
	else if ((total - pre_total) > 0) 
		result = int((100 * (double) (used - pre_used)) / 
			 (double) (total - pre_total));
	else	result = 0;

	pre_used = used;
	pre_total= total;

	return result;
}
#endif // __FreeBSD_version
