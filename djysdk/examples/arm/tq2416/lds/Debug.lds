ENTRY(AppStart)

INCLUDE  ..\lds\memory.lds

/*不同的make程序，文件路径表示方式，是不同的，输入段中文件名写法要相应修改*/
/*正确的写法，可参看console中输出的命令行*/
SECTIONS
{
    /*地址填充区域，Release版本下载起始地址实质上是0x0040 0000 + IbootSize + 256*/
    /*因此在此处填充section内，将InnerFlash地址偏移，使生成的bin文件起始地址为0x00440100*/
    /*切勿在填充section内填写数据（例如LONG( . + 256)），否则生成的bin文件中会包含此段*/
    Iboot_IbootFill :        
    {
        gc_IbootStart = .;
        . += IbootSize;  
    }>ROM
	
    Iboot_AppInfo :
    {
        gc_ptIbootCtrl = .;         /*结构变量，c中可用，用于检查APP正确性*/
        LONG(0xffffffff)            /*APP size*/
        LONG(0xffffffff)            /*CRC32 of APP*/
        LONG(0xffffffff)            /*rev*/
        LONG(0x00676264)            /* "dbg",调试标志，Iboot遇到此标志不检查APP正确性*/

        . = ALIGN(256);
        gc_ptAppInfo = .;   
        LONG( . + 256);
        gc_AppSize = .;             /*ROM区 存储APP的bin文件大小 */
        LONG(pProgRomEnd - gc_AppSize + 4); /* APP的bin文件大小 */
        . = ALIGN(256);
        gc_AppAddr = .;             /*函数指针变量，指向APP的起始地址*/
    }>ROM
    
    /*RevRam是Ram保留区域，用于存储preload前使用或保存的内在数据,只有IAP_pload.c*/
    /*的pg_IapVar变量从该段内分配，且该段在iboot.lds和debug/release.lds保持一致*/
    Iboot_RevRam (NOLOAD):
    {
        . = ALIGN(0x08);
        KEEP(*(.IapBootFlgRam .IapBootFlgRam.*))    
    }>RAM1
    
    text_startup_rROM1_lROM1 :
    {
        . = ALIGN(0x08);          
        KEEP(*liblibOS_tq2416_o0.a:initcpu.o (.text.AppStart))
        KEEP(*liblibOS_tq2416_o0.a:pre_loader.o (.text .text.* .rodata .rodata.*))
        . = ALIGN(0x08);
        KEEP(*(.isr_vector .isr_vector.*))
    } >ROM 
 
    text_preload_rROM1_lROM1 :
    {
        . = ALIGN(0x08);
        
        KEEP(src/critical/critical.o (.text .text.* .rodata .rodata.*))
        KEEP(*liblibOS_tq2416_o0.a:hard-exps.o (.text .text.* .rodata .rodata.*))
        KEEP(*liblibOS_tq2416_o0.a:hard-exp.o (.text .text.* .rodata .rodata.*))
        KEEP(*liblibOS_tq2416_o0.a:set-cache.o (.text .text.* .rodata .rodata.*))
        *liblibOS_tq2416_o0.a:loader.o (.text .text.* .rodata .rodata.*)
        *liblibOS_tq2416_o0.a:int_hard.o (.text .text.* .rodata .rodata.*)
        *liblibOS_tq2416_o0.a:int.o (.text .text.* .rodata .rodata.*)
        *liblibOS_tq2416_o0.a:ints.o (.text .text.* .rodata .rodata.*)
        *liblibOS_tq2416_o0.a:cpu_peri_gpio.o (.text .text.* .rodata .rodata.*)
        *liblibOS_tq2416_o0.a:cpu_peri.o (.text .text.* .rodata .rodata.*)
        *liblibOS_tq2416_o0.a:heap-static.o (.text .text.* .rodata .rodata.*)
        *liblibOS_tq2416_o0.a:list.o (.text .text.* .rodata .rodata.*)
    }>RAM1 AT>ROM
    
    rw_preload_rRAM1_lROM1 :
    {
        . = ALIGN(0x08);
        KEEP(* (.table.exceptions .table.exceptions.*))
        KEEP(* (.table.vectors .table.vectors.*))
        KEEP(src/critical/critical.o (.data .data.*))
        *liblibOS_tq2416_o0.a:loader.o (.data .data.*)
        *liblibOS_tq2416_o0.a:int_hard.o (.data .data.*)
        *liblibOS_tq2416_o0.a:int.o (.data .data.*)
        *liblibOS_tq2416_o0.a:ints.o (.data .data.*)
        *liblibOS_tq2416_o0.a:hard-exp.o (.data .data.*)
        *liblibOS_tq2416_o0.a:cpu_peri_gpio.o (.data .data.*)
        *liblibOS_tq2416_o0.a:cpu_peri.o (.data .data.*)
        *liblibOS_tq2416_o0.a:heap-static.o (.data .data.*)
        *liblibOS_tq2416_o0.a:list.o (.data .data.*)
    }>RAM1 AT>ROM
    
    zi_preload_rRAM1_lRAM1 (NOLOAD) :
    {
        . = ALIGN(0x08);
        KEEP(src/critical/critical.o (.bss .bss.* COMMON))
        *liblibOS_tq2416_o0.a:loader.o (.bss .bss.* COMMON)
        *liblibOS_tq2416_o0.a:int_hard.o (.bss .bss.* COMMON)
        *liblibOS_tq2416_o0.a:int.o (.bss .bss.* COMMON)
        *liblibOS_tq2416_o0.a:ints.o (.bss .bss.* COMMON)
        *liblibOS_tq2416_o0.a:hard-exp.o (.bss .bss.* COMMON)
        *liblibOS_tq2416_o0.a:cpu_peri_gpio.o (.bss .bss.* COMMON)
        *liblibOS_tq2416_o0.a:cpu_peri.o (.bss .bss.* COMMON)
        *liblibOS_tq2416_o0.a:heap-static.o (.bss .bss.* COMMON)
        *liblibOS_tq2416_o0.a:list.o (.bss .bss.* COMMON)
    }>RAM1 
     
    text_sysload_rROM1_lROM1 :
    {
        . = ALIGN(0x08);
        * (.text .text.* .rodata .rodata.*)
    }>RAM1 AT>ROM 
    
    rw_sysload_rRAM1_lROM1 :
    {
        . = ALIGN(0x08);
        * (.data .data.*)
    }>RAM1 AT>ROM
    
    
    zi_sysload_rRAM1_lRAM1 (NOLOAD) :
    {
        . = ALIGN(0x08);
        * (.bss .bss.* COMMON)
    }>RAM1 
    
    stack_rRAM1_lRAM1 (NOLOAD) :
    {
        . = ALIGN(0x08);
        
        usr_bottom = .;
        /*KEEP(* (.stack.usr .stack.usr.*))*/
        . += 4096;
        usr_top = .;
        
        svc_bottom = .;
        /*KEEP(* (.stack.svc .stack.svc.*))*/
        . += 4096;
        svc_top = .;
        
        irq_bottom = .;
        /*KEEP(* (.stack.irq .stack.irq.*))*/
        . += 4096;
        irq_top = .;
        
        fiq_bottom = .;
        /*KEEP(* (.stack.fiq .stack.fiq.*))*/
        . += 4096;
        fiq_top = .;
        
        undef_bottom = .;
        /*KEEP(* (.stack.undef .stack.undef.*))*/
        . += 4096;
        undef_top = .;
        
        abort_bottom = .;
        /*KEEP(* (.stack.abort .stack.abort.*))*/
        . += 4096; 
        abort_top = .;
        
    }>RAM1 
    
    
    preload_cp_table :
    {
        . = ALIGN(0x08);
        preload_copy_table = .;
        
        LONG(16)
        LONG(3)
        
        LONG(LOADADDR(text_preload_rROM1_lROM1))
        LONG(ADDR(text_preload_rROM1_lROM1))
        LONG(SIZEOF(text_preload_rROM1_lROM1))
        LONG(1)
        
        LONG(LOADADDR(rw_preload_rRAM1_lROM1))
        LONG(ADDR(rw_preload_rRAM1_lROM1))
        LONG(SIZEOF(rw_preload_rRAM1_lROM1))
        LONG(1)
        
        LONG(LOADADDR(zi_preload_rRAM1_lRAM1))
        LONG(ADDR(zi_preload_rRAM1_lRAM1))
        LONG(SIZEOF(zi_preload_rRAM1_lRAM1))
        LONG(0)
    }>ROM 
    
    sysload_cp_table :
    {
        . = ALIGN(0x08);
        sysload_copy_table = .;
        
        LONG(16)
        LONG(3)
        
        LONG(LOADADDR(text_sysload_rROM1_lROM1))
        LONG(ADDR(text_sysload_rROM1_lROM1))
        LONG(SIZEOF(text_sysload_rROM1_lROM1))
        LONG(1)
        
        LONG(LOADADDR(rw_sysload_rRAM1_lROM1))
        LONG(ADDR(rw_sysload_rRAM1_lROM1))
        LONG(SIZEOF(rw_sysload_rRAM1_lROM1))
        LONG(1)
                
        LONG(LOADADDR(zi_sysload_rRAM1_lRAM1))
        LONG(ADDR(zi_sysload_rRAM1_lRAM1))
        LONG(SIZEOF(zi_sysload_rRAM1_lRAM1))
        LONG(0)
    }>ROM 
            
    heap_rRAM1_lRAM1 (NOLOAD) :
    {
        . = ALIGN(0x08);
        sys_heap_bottom = .;
        . += 8M;
        sys_heap_top = .;
    }>RAM1
    
    heap_rextram_lextram (NOLOAD) :
    {
        . = ALIGN(0x08);
        extram_heap_bottom = .;
        . += RAM2Size;
        extram_heap_top = .;
        
    }>RAM2 
    
    heap_cp_table :
    {
        . = ALIGN(0x08);
        pHeapList = .;
        
        /* 默认堆，必须排首位，名字必须是“sys” */
        LONG(1)                     /*表示该堆由1段内存组成*/
        LONG(0)                     /*该堆上分配的内存的对齐尺寸，0表示使用系统对齐*/
        LONG(0)                     /*0=通用堆,1=专用堆,如果系统只有唯一一个堆,则只能是通用堆*/
        LONG(sys_heap_bottom)       /*第一段基址，须符合对齐要求*/
        LONG(sys_heap_top)          /*第一段地址上限（不含），须符合对齐要求*/
        LONG(32)                    /*第一段页尺寸=32bytes*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0x79)                  /*'y'*/
        BYTE(0x73)                  /*'s'*/
        BYTE(0)                     /*串结束，"sys"是堆的名字*/

        . = ALIGN(0x04);
        LONG(1)                     /*表示该堆由1段内存组成*/
        LONG(32)                    /*该堆上分配的内存的对齐尺寸，0表示使用系统对齐*/
        LONG(0)                     /*0=通用堆,1=专用堆,如果系统只有唯一一个堆,则只能是通用堆*/
        LONG(extram_heap_bottom)    /*第一段基址，须符合对齐要求*/
        LONG(extram_heap_top)       /*第一段地址上限（不含），须符合对齐要求*/
        LONG(128)                   /*第一段页尺寸=128bytes*/
        BYTE(0x65)                  /*'e'*/
        BYTE(0x78)                  /*'x'*/
        BYTE(0x74)                  /*'t'*/
        BYTE(0x72)                  /*'r'*/
        BYTE(0x61)                  /*'a'*/
        BYTE(0x6d)                  /*'m'*/
        BYTE(0)                     /*串结束，"extram"是堆的名字*/
        . = ALIGN(0x04);
        LONG(0)                     /*结束标志*/
    }>ROM 
    
    zi_startup_rRAM_nocache_lRAM_nocache (NOLOAD) :
    {
        . = ALIGN(0x08);
        mmu_page_table = .;     
        . += 0x8000;
        
        engine_irq = .;
        . += 0x04;
        engine_fiq = .;
        . += 0x04;
        engine_undef = .;
        . += 0x04;
        engine_swi = .;
        . += 0x04;
        engine_pabort = .;
        . += 0x04;
        engine_dabort = .;
        . += 0x04;
        engine_reserved = .;
        . += 0x04;

    } >RAM_nocache
    
    Iboot_Info :
    {
        gc_ptIbootSize = .;
        LONG(IbootSize)
    }>ROM
    
    rom_prog_end :
    {
       . = ALIGN(0x04);
        pProgRomEnd = .;
    }>ROM 

}
/**** The End ****/
