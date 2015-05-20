#include "mrc.h"

#ifndef PEB_H
#define PEB_H

#define inline __inline

typedef unsigned short ushort;


struct _IMAGE_OPTIONAL_HEADER
{
	inline struct _image_directory *exports(void)
	{
		return (struct _image_directory *)(96 + (char *)this);
	}
};

// 6 * 4 = IMAGE_OPTIONAL_HEADERS offset 

struct _IMAGE_NT_HEADERS
{
	inline _IMAGE_OPTIONAL_HEADER *optheaders(void)
	{
		return (struct _IMAGE_OPTIONAL_HEADER *)(0x18 + (char *)this);
	}
};


struct _IMAGE_DOS_HEADER {  // DOS .EXE header
	ushort e_magic;         // Magic number
	ushort e_cblp;          // Bytes on last page of file
	ushort e_cp;            // Pages in file
	ushort e_crlc;          // Relocations
	ushort e_cparhdr;       // Size of header in paragraphs
	ushort e_minalloc;      // Minimum extra paragraphs needed
	ushort e_maxalloc;      // Maximum extra paragraphs needed
	ushort e_ss;            // Initial (relative) SS value
	ushort e_sp;            // Initial SP value
	ushort e_csum;          // Checksum
	ushort e_ip;            // Initial IP value
	ushort e_cs;            // Initial (relative) CS value
	ushort e_lfarlc;        // File address of relocation table
	ushort e_ovno;          // Overlay number
	ushort e_res[4];        // Reserved words
	ushort e_oemid;         // OEM identifier (for e_oeminfo)
	ushort e_oeminfo;       // OEM information; e_oemid specific
	ushort e_res2[10];      // Reserved words
	long   e_lfanew;        // File address of new exe header

	inline struct _IMAGE_NT_HEADERS *ntheaders(void)
	{
		return (struct _IMAGE_NT_HEADERS *)(this->e_lfanew + (char *)this);
	}

};

typedef unsigned long DWORD;

struct _export_directory
{
	unsigned int characteristics;
	//4
	unsigned int timedatestamp;
	//8
	unsigned short majorversion;
	unsigned short minorversion;
	//C
	unsigned int rva_name;
	//10
	unsigned int rva_base;
	//14
	unsigned int nfunctions;
	//18
	unsigned int nnames;
	//1C
	// RVA
	unsigned int addr_functions;
	//20
	// RVA
	unsigned int addr_names;
	//24
	// RVA
	unsigned int addr_ordinals;

};

struct _image_directory
{
	DWORD rva;
	DWORD size;
	template <typename t>
	t *get(void *base)
	{
		return (t *)(rva + (char *)base);
	}
};



struct _unicode_string
{
	unsigned short length;
	unsigned short max_length;
	char *buffer;

	inline crc32 mrc(bool sensitive = false)
	{
		crc32 ret;
		::mrc(ret, buffer, length, sensitive);
		return ret;
	}
};

struct _module_entry
{
	struct _module_entry *flink;
	struct _module_entry *blink;
	inline _unicode_string *getname(void)
	{
		return (_unicode_string *)(0x24 + (char *)this);
	}

	inline void *getbase(void)
	{
		return *(void **)(0x10 + (char *)this);
	}

};

struct _ldr
{
	inline _module_entry *&entry(void)
	{
		return *(_module_entry **)(0x14 + (char *)this);
	}
};

struct _peb
{
	inline _ldr *ldr(void)
	{
		return *(_ldr **)(0xC + (char *)this);
	}
};
#endif