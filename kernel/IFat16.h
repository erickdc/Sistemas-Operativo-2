/* 
 * File:   IFat16.h
 * Author: erick304
 *
 * Created on September 23, 2015, 1:31 PM
 */

#ifndef IFAT16_H
#define	IFAT16_H
#include "types.h"
#include "system.h"
#include "keyboard.h"



#ifdef	__cplusplus
extern "C" {
#endif
 typedef struct PartitionEntryMBR{
    unsigned char BootIndicator;
    unsigned char StartHead;
    unsigned char StartCylSector;
    unsigned char StartCylinder;
    unsigned char Type;
    unsigned char EndHead;
    unsigned char EndSector;
    unsigned char EndCylinder;
    uint32_t StartLBA;
    uint32_t SectorCountLBA;
}__attribute__((packed)) pembr_t;

typedef struct MBR{
    uint8_t boot[446]; // boot sector code
    pembr_t partitions[4];
    uint8_t signature[2];    
}__attribute__((packed)) mbr_t;

typedef struct BootSector{
   char jmpBoot[3];
   unsigned char oemName[8];
   uint16_t  bytsPerSec;
   uint8_t secPerClus;
   uint16_t  rsvdSEcCnt;
   uint8_t  numFAts;
   uint16_t  rootEntCnt;
   uint16_t totSec16;
   uint8_t  bMedia;
   uint16_t  bFatsz16;
   uint16_t secPerTrk;
   uint16_t numHeads;
   uint32_t bHiddSEc;
   uint32_t totSec32;
   uint8_t drvNum;
   uint8_t reserved1;
   uint8_t bootSig;
   uint32_t volId;
   unsigned char volLab[11];
   unsigned char filSysType[8];
   unsigned char boot[448];
   unsigned char signature [2];
}__attribute__((packed))bootS;

struct DirectoryEntry{
    unsigned char dirName[11];
     uint8_t dirAttr;
    uint8_t dirNtr;
    uint8_t dirCrtTimeTenth;
    uint16_t dirCrtTime;
    uint16_t dirCrtDate;
    uint16_t dirLstAccDate;
    uint16_t dirFstClusHi;
    uint16_t dirWrtTime;
    uint16_t dirWrtDate;
    uint16_t dirFstClusLo;
    uint32_t dirFileSize;
}__attribute__((packed))directorio;
uint32_t tamanioFat;
uint32_t bytePorSector;
uint32_t direccionFat1;
uint32_t direccionFat2;
uint32_t dirRoot;
uint32_t sectorPC;
uint32_t dataEntryDisco;

void InicializarEstructuraFat(){
     init_ide_devices();
     mbr_t* mbr;
     ide_read_blocks(0, 0, 1, mbr);
    
     
    
    uint32_t sectorRelativo = mbr->partitions[0].StartLBA;
  //  kprintf("%s%d","Sector Relativo",sectorRelativo);
    bootS* mbr2;
    ide_read_blocks(0, sectorRelativo, 1, mbr2);
    puts(mbr2->volLab);
    tamanioFat = mbr2->bFatsz16;
    bytePorSector = mbr2->bytsPerSec;
    direccionFat1 = sectorRelativo + mbr2->rsvdSEcCnt;
     //kprintf("%s%d","direccionFat1",direccionFat1);
    direccionFat2 + direccionFat1 + tamanioFat;
       //kprintf("%s%d","direccionFat2",direccionFat2);
    dirRoot = direccionFat2 + tamanioFat;
    sectorPC = mbr2->secPerClus;
    dataEntryDisco = dirRoot + (mbr2->rootEntCnt*32/bytePorSector);
}

void ImprimirDirRoot(uint32_t dir){
    struct DirectoryEntry directorios[16];
    ide_read_blocks(0,dir,1,directorios);
    int i;
    
    for(i=0;i<16;i++)
    {
        printString(directorios[i].dirName);
          printString("\n");
    }
}


#ifdef	__cplusplus
}
#endif

#endif	/* IFAT16_H */

