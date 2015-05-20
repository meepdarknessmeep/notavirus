//#define DO_BREAKS

#ifdef DO_BREAKS

#define int3() __asm int 3

#else

#define int3()

#endif

template<typename t>
inline void setsafe(t &where, void *with)
{
	where = (t)with;
}
template<typename t>
inline void setsafe(t &where, unsigned int with)
{
	where = (t)with;
}
template<typename t>
inline void setsafe(t &where, unsigned long with)
{
	where = (t)with;
}
template<typename t>
inline void setsafe(t &where, t with)
{
	where = with;
}

#include "peb.h"
#include "mrc.h"


#define KERNEL32_DLL_CRC (0x8A817497)
#define WRITECONSOLEA_CRC (0x5D91EC61)
#define READCONSOLEA_CRC (0x0E1DD4D6)
#define GETSTDHANDLE_CRC (0xC47F2293)

#define SECTION_NAME ".fuckme."

#pragma comment(linker, "/SECTION:" SECTION_NAME ",DEKPRSW")
#pragma comment(linker, "/MERGE:.data=" SECTION_NAME)
#pragma comment(linker, "/MERGE:.rdata=" SECTION_NAME)
#pragma comment(linker, "/MERGE:.code=" SECTION_NAME)
#pragma comment(linker, "/MERGE:.text=" SECTION_NAME)
#pragma comment(linker, "/ENTRY:main")
#pragma comment(linker, "/ALIGN:4")

#define noinline __declspec(noinline)

typedef unsigned short ushort;



inline _peb *(getpeb)(void)
{
	__asm
	{
		mov eax, FS:[0x30];
	}
}

inline unsigned int strlen(const char *str)
{
	const char *ends = str;
	while (*ends++);
	return ends - str - 1;
}

void *(__stdcall *GetStdHandle)(DWORD);
int(__stdcall *WriteConsoleA)(void *handle, const char *str, DWORD numchars, DWORD *written, void *reserved);
int(__stdcall *ReadConsoleA)(void *handle, char *str, DWORD amount, DWORD *howmany, void *nullme);


void __stdcall main(void)
{

	union
	{
		_peb *peb;
		_module_entry **first;
		DWORD junk;
		char junk2[4];
	};

	peb = getpeb();

	_ldr *ldr = peb->ldr();

	first = &ldr->entry();
	union
	{
		_module_entry *mod;
		_module_entry *kernel32;
	};
	mod = *first;

	crc32 crc;
	while (1)
	{
		register _unicode_string *str = mod->getname();
		mrc(crc, str->buffer, str->length, false);
		int3();
		if (crc == KERNEL32_DLL_CRC)
			break;
		mod = mod->flink;
	}

	void *kernel32base = kernel32->getbase();

	_image_directory *export_dir = ((_IMAGE_DOS_HEADER *)kernel32base)->ntheaders()->optheaders()->exports();

	_export_directory *exports = export_dir->get<_export_directory>(kernel32base);

	register DWORD n = exports->nnames;

	unsigned short *ordinals;
	register unsigned int *funcs, *names;
	setsafe(funcs, exports->addr_functions + (char *)kernel32base);
	setsafe(names, exports->addr_names + (char *)kernel32base);
	setsafe(ordinals, exports->addr_ordinals + (char *)kernel32base);
	while (n--)
	{
		const char *name = names[n] + (char *)kernel32base; 
		mrc(crc, name, 0, true, true);
		auto func = (funcs[ordinals[n]] + (char *)kernel32base);
		if (crc == GETSTDHANDLE_CRC)
			setsafe(GetStdHandle, func);
		else if (crc == WRITECONSOLEA_CRC)
			setsafe(WriteConsoleA, func);
		else if (crc == READCONSOLEA_CRC)
			setsafe(ReadConsoleA, func);
	}


	const char str[13] = { 'M', 'e', 'e', 'p', ' ', 'w', 'u', 'z', ' ', 'h', 'e', 'r', 'e' };

	WriteConsoleA(GetStdHandle(-11), str, 13, &junk, 0);

	ReadConsoleA(GetStdHandle(-10), junk2, 1, &junk, 0);
}