
all: ../loader.bin boot.S ../../dosutils/partcopy.exe boot.ld
	gcc -c boot.S -o boot.o -I ../includes		\
			-D NR_SECTORS_PER_FAT=9 	\
			-D NR_BYTES_PER_SECTOR=512	\
			-D NR_FATS=2			\
			-D ENTRY_SIZE=32 		\
			-D ROOT_DIR_ENTRIES=224		
	ld -T boot.ld boot.o -o ../boot.bin

clean:
	del *.o
	del *.bin