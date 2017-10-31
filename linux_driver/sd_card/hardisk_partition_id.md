# 分区类型

## 常见分区类型
 主要为WINDOWS 和LINUX分区类型的识别码

|分区ID| 分区描述 |  
|:---------|:---------|
|00H| DOS或WINDOWS不允许使用，视为非法|  
|01H| FAT12|  
|04H| FAT16小于32MB| 
|05H| Extended|
|06H| FAT16大于32MB|  
|07H| HPFS/NTFS|  
|OBH| WINDOWS95 FAT32|  
|OCH| WINDOWS95 FAT32|  
|0EH| WINDOWS FAT16|  
|0FH| WINDOWS95 Extended(大于8G)|  
|82H| Linux swap|  
|83H| Linux|  
|85H| Linux extended|  
|86H| NTFS volume set|  
|87H| NTFS volume set|  
    
1 `DBR`
 DBR（DOS BOOT RECORD，DOS引导记录），位于柱面0，磁头1，扇区1，即逻辑扇区0。DBR分为两部分：DOS引导程序和BPB（BIOS参数块）。
 其中DOS 引导程序完成DOS系统文件（IO.SYS，MSDOS.SYS）的定位与装载，而BPB用来描述本DOS分区的磁盘信息，BPB位于DBR偏移0BH 处，共13字节。它包含逻辑格式化时使用的参数，可供DOS计算磁盘上的文件分配表，目录区和数据区的起始地址，BPB之后三个字提供物理格式化（低格）时采用的一些参数。引导程序或设备驱动程序根据这些信息将磁盘逻辑地址（DOS扇区号）转换成物理地址（绝对扇区号）。

(1) `BPB格式`

|序号|偏移地址 |意义|
|:---------|:---------:|:---------|
|1  | 03H－0AH| OEM号|
|2  | 0BH－0CH| 每扇区字节数|
|3  | 0DH         | 每簇扇区数|
|4  | 0EH－0FH |保留扇区数|
|5  | 10H         | FAT备份数|
|6  | 11H－12H| 根目录项数|
|7  | 13H－14H| 磁盘总扇区数|
|8  | 15H         | 描述介质|
|9  | 16H－17H| 每FAT扇区数|
|10| 18H－19H|每磁道扇区数|
|11| 1AH－1BH| 磁头数|
|12| 1CH－1FH |特殊隐含扇区数|
|13| 20H－23H |总扇区数|
|14| 24H－25H |物理驱动器数|
|15| 26H         | 扩展引导签证|
|16| 27H－2AH| 卷系列号|
|17| 2BH－35H| 卷标号|
|18| 36H－3DH| 文件系统号|

(2) `DOS`引导记录公式：
文件分配表≡保留扇区数
根目录≡保留扇区数＋FAT的个数×每个FAT的扇区数
数据区≡根目录逻辑扇区号＋（32×根目录中目录项数＋（每扇区字节数－1））DIV每扇区字节数
绝对扇区号≡逻辑扇区号＋隐含扇区数
扇区号≡（绝对扇区号MOD每磁道扇区数）＋1
磁头号≡（绝对扇区号DIV每磁道扇区数）MOD磁头数
磁道号≡（绝对扇区号DIV每磁道扇区数）DIV磁头数

要点：
a. DBR位于柱面0，磁头1，扇区1，其逻辑扇区号为0
b. DBR包含DOS引导程序和BPB;
c. BPB十分重要，由此可算出逻辑地址与物理地址;

 以上仅DOS（FAT16）为例，由于DOS（FAT16）已经退出历史舞台，但现在0磁道1柱面1扇区这个位置仍然是起着类似的作用，所以准确地说，DBR应该改称为 OBR（OS Boot Record）即操作系统引导扇区，如WINXP的OBR（FAT32或NTFS）就是在DOS的DBR基础逐步演变而来的。OBR（DBR）是高级格式化程序产生的，如FORMAT，PM，DM，DISKPART，WINXP磁盘管理器……

2 `OBR`

 OBR（OS Boot Record）即操作系统引导扇区，通常位于硬盘的0磁道1柱面1扇区（这是对于DOS来说的，对于那些以多重引导方式启动的系统则位于相应的主分区/扩展分区的第一个扇区），是操作系统可直接访问的第一个扇区，它也包括一个引导程序和一个被称为BPB（BIOS Parameter Block）的本分区参数记录表。其实每个逻辑分区都有一个OBR，其参数视分区的大小、操作系统的类别而有所不同。引导程序的主要任务是判断本分区根目录前两个文件是否为操作系统的引导文件（例如MSDOS或者起源于MSDOS的Win9x/Me的IO.SYS和MSDOS.SYS）。如是，就把第一个文件读入内存，并把控制权交予该文件。BPB参数块记录着本分区的起始扇区、结束扇区、文件存储格式、硬盘介质描述符、根目录大小、FAT个数、分配单元（Allocation Unit，以前也称之为簇）的大小等重要参数。OBR由高级格式化程序产生（例如DOS 的Format.com）。


## 各种分区类型对应的partition_Id

|ID| Name | Note| 
|:---------|:---------|:---------|
|00h| empty| [空]| 
| 01h | DOS 12-bit FAT|  [MS DOS FAT12] | 
| 02h | XENIX root file system | [MS XENIX 根文件系统] | 
| 03h | XENIX /usr file system (obsolete) | [MS XENIX /usr 文件系统] | 
| 04h | DOS 16-bit FAT (up to 32M)|  [MS DOS FAT16 支持32M以下的分区] | 
| 05h | DOS 3.3+ extended partition | [MS DOS 3.3以上的扩展分区] | 
| 06h|  DOS 3.31+ Large File System (16-bit FAT, over 32M)|  [MS DOS 3.31以上大文件系统,支持32M以上分区的FAT16] | 
|07h| QNX |
|07h| OS/2 HPFS |[IBM OS/2 高性能文件系统]| 
|07h| Windows NT NTFS |[MS WindowsNT NT文件系统] |
|07h| Advanced Unix |
|08h| OS/2 (v1.0-1.3 only)| [IBM OS/2 仅仅适用于 1.0-1.3版] |
|08h |AIX bootable partition, SplitDrive| [IBM AIX 引导分区,分割驱动器] |
|08h |Commodore DOS| 
|08h |DELL partition spanning multiple drives| [DELL 跨驱动器分区] |
|09h |AIX data partition| [IBM AIX数据分区] |
|09h |Coherent filesystem| [Coherent 文件系统] |
|0Ah |OS/2 Boot Manager| [IBM OS/2 引导管理器分区] |
|0Ah |OPUS |
|0Ah| Coherent swap partition| [Coherent 交换分区] |
|0Bh| Windows 95 with 32-bit FAT| [MS Windows 95 FAT32] |
|0Ch| Windows 95 with 32-bit FAT (using LBA-mode INT 13 extensions)| [MS Windows 95 FAT32 使用LBA模式INT13扩展] |
|0Eh| LBA VFAT (same as 06h but using LBA-mode INT 13)| [LBA VFAT 类似06h但使用LBA模式INT13] |
|0Fh| LBA VFAT (same as 05h but using LBA-mode INT 13)| [LBA VFAT 类似06h但使用LBA模式INT13] |
|10h| OPUS 
|11h| OS/2 Boot Manager hidden 12-bit FAT partition| [IBM OS/2 引导管理器的FAT12隐藏分区] |
|12h| Compaq Diagnostics partition| [Compaq 诊断分区] |
|12h| Unkown hidden FAT partition 
|14h| (using Novell DOS 7.0 FDISK to delete Linux Native part)| [使用Novell DOS 7.0的FDISK删除Linux原生分区]
|14h| OS/2 Boot Manager hidden sub-32M 16-bit FAT partition| [IBM OS/2 引导管理器的小于等于32M的FAT16隐藏分区] |
|16h| OS/2 Boot Manager hidden over-32M 16-bit FAT partition| [IBM OS/2 引导管理器的大于32M的FAT16隐藏分区] |
|17h| OS/2 Boot Manager hidden HPFS partition| [IBM OS/2 引导管理器的HPFS隐藏分区] |
|18h| AST special Windows swap file| [AST 特殊Windows交换文件分区] |
|1Bh| Hidden Windows 95 with 32-bit FAT| [Windows 95 FAT32隐藏分区] |
|1Ch| Hidden Windows 95 with 32-bit LBA FAT| [Windows 95 LBA模式FAT32隐藏分区] |
|1Eh| Hidden Windows 95 with LBA BIGDOS| [Windows 95 LBA模式BIGDOS隐藏分区] |
|24h| NEC MS-DOS 3.x |
|27h| Unkown hidden NTFS partition |
|38h| Theos |
|3Ch| PowerQuest PartitionMagic recovery partition| [PowerQuest PartitionMagic 恢复分区] |
|40h| VENIX 80286 |
|41h| Personal RISC Boot| [Personal RISC 引导分区] |
|42h| SFS (Secure File System) by Peter Gutmann| [Peter Gutmann安全文件系统] |
|4Fh| Oberon |
|50h| OnTrack Disk Manager, read-only partition| [OnTrack Disk Manger 只读分区] |
|51h| OnTrack Disk Manager, read/write partition| [Personal RISC 读写分区] |
|51h| NOVELL |
|52h| CP/M |
|52h| Microport System V/386 |
|53h| OnTrack Disk Manager, write-only partition???| [OnTrack Disk Manager只写分区???] |
|54h| OnTrack Disk Manager (DDO) |
|56h| GoldenBow VFeature |
|57h| Unkown hidden FAT partition |
|58h| Unkown hidden FAT partition |
|61h| SpeedStor |
|63h| Unix SysV/386, 386/ix |
|63h| Mach, MtXinu BSD 4.3 on Mach |
|63h| GNU HURD |
|64h| Novell NetWare 286 |
|65h| Novell NetWare (3.11) |
|67h| Novell |
|68h| Novell |
|69h| Novell |
|70h| DiskSecure Multi-Boot |
|75h| PC/IX |
|80h| Minix v1.1 - 1.4a |
|81h| Minix v1.4b+ |
|81h| Linux |
|81h| Mitac Advanced Disk Manager |
|82h| Linux Swap partition| [Linux 交换分区] |
|82h| Prime |
|82h| Solaris |
|83h| Linux native file system (ext2fs/xiafs)| [Linux 原生分区] [包括ext3.ext4等]|
|84h| OS/2-renumbered type 04h partition (hiding DOS C: drive) |
|87h| HPFS Fault-Tolerant mirrored partition| [HPFS 容错镜像分区] |
|93h| Amoeba file system| [Amoeba文件系统] |
|94h| Amoeba bad block table| [Amoeba坏块表] |
|A5h| FreeBSD, BSD/386 |
|B7h| BSDI file system (secondarily swap)| [BSDI 文件系统第二交换分区] |
|B8h| BSDI swap partition (secondarily file system)| [BSDI 文件系统第二文件分区] |
|BCh| ATI hidden FAT partition| [ 用于保存ATI映像的隐藏分区 ] |
|C1h| DR DOS 6.0 LOGIN.EXE-secured 12-bit FAT partition| [DR DOS 6.0 LOGIN.EXE 安全 FAT12分区] |
|C4h| DR DOS 6.0 LOGIN.EXE-secured 16-bit FAT partition| [DR DOS 6.0 LOGIN.EXE 安全 FAT16分区] |
|C6h| DR DOS 6.0 LOGIN.EXE-secured Huge partition| [DR DOS 6.0 LOGIN.EXE 安全巨分区] |
|C7h| Syrinx Boot |
|D7h| DELL hidden FAT partition |
|D8h| CP/M-86 |
|DBh| CP/M, Concurrent CP/M, Concurrent DOS |
|DBh| CTOS (Convergent Technologies OS) |
|DBh| DELL hidden FAT partition |
|DD|h DELL hidden FAT partition |
|DEh| DELL hidden FAT partition |
|E1h| SpeedStor 12-bit FAT extended partition| [SpeedStor FAT12扩展分区] |
|E3h| DOS read-only| [DOS只读分区] |
|E3h| Storage Dimensions |
|E4h| SpeedStor 16-bit FAT extended partition| [SpeedStor FAT16扩展分区] |
|EBh| BeOS partition |
|F1h| Storage Dimensions |
|F2h| DOS 3.3+ secondary partition| [DOS 3.3以上第二分区] |
|F4h| SpeedStor |
|F4h| Storage Dimensions |
|FEh| LANstep |
|FEh| IBM PS/2 IML |
|FFh| Xenix bad block table| [Xenix坏块表]|
|21h| officially listed as reserved| [官方保留] |
|23h| officially listed as reserved| [官方保留] |
|26h| officially listed as reserved| [官方保留] |
|31h| officially listed as reserved| [官方保留] |
|33h| officially listed as reserved| [官方保留] |
|34h| officially listed as reserved| [官方保留] |
|36h| officially listed as reserved| [官方保留] |
|F6h| officially listed as reserved| [官方保留] |
|E5h| officially listed as reserved| [官方保留] |
|E6h| officially listed as reserved| [官方保留] |
|F3h| officially listed as reserved| [官方保留] |
|86h| officially listed as reserved| [官方保留] |
|A1h| officially listed as reserved| [官方保留] |
|A3h| officially listed as reserved| [官方保留] |
|A4h| officially listed as reserved| [官方保留] |
|A6h| officially listed as reserved| [官方保留] |
|B1h| officially listed as reserved| [官方保留] |
|B3h| officially listed as reserved| [官方保留] |
|B4h| officially listed as reserved| [官方保留] |
|B6h| officially listed as reserved| [官方保留] |
|71h| officially listed as reserved| [官方保留] |
|73h| officially listed as reserved| [官方保留] |
|74h| officially listed as reserved| [官方保留] |
|76h| officially listed as reserved| [官方保留] ||