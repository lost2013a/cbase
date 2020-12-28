ENTRY(_start)
SECTIONS
{
	PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x00010000)); . = SEGMENT_START("text-segment", 0x00010000) + SIZEOF_HEADERS;
	. = ALIGN(4);
	.text : { *(.text)}
	.data : { *(.data)}
	.init_array     :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	}
	.bss  : { *(.bss)}
	
}
