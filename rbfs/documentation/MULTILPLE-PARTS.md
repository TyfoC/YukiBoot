#	Multiple partitions (example, shell command)
```sh
$(UTILITY_RBFS) -cstgpt ../storage 20 "1 Partition" 0 ./rbfs-partition-header1.bin ./rbfs-file-table1.bin
$(UTILITY_RBFS) -cstgpt ../storage 21 "2 Partition" 0 ./rbfs-partition-header2.bin ./rbfs-file-table2.bin
$(UTILITY_RBFS) -cstgpt ../storage 22 "3 Partition" 0 ./rbfs-partition-header3.bin ./rbfs-file-table3.bin
$(UTILITY_RBFS) -cstgpt ../storage 23 "4 Partition" 0 ./rbfs-partition-header4.bin ./rbfs-file-table4.bin
$(UTILITY_RBFS) -cstgpt ../storage 24 "5 Partition" 0 ./rbfs-partition-header5.bin ./rbfs-file-table5.bin
$(UTILITY_RBFS) -cstgpt ../storage 25 "6 Partition" 0 ./rbfs-partition-header6.bin ./rbfs-file-table6.bin
$(UTILITY_RBFS) -cstgpt ../storage 26 "7 Partition" 0 ./rbfs-partition-header7.bin ./rbfs-file-table7.bin
$(UTILITY_RBFS) -cstgpt ../storage 27 "8 Partition" 0 ./rbfs-partition-header8.bin ./rbfs-file-table8.bin
$(UTILITY_RBFS) -cstgpt ../storage 28 "9 PARTITION LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO283L" 0 ./rbfs-partition-header9.bin ./rbfs-file-table9.bin
$(UTILITY_RBFS) -cstgpt ../storage 29 "10 Partition" 0 ./rbfs-partition-header10.bin ./rbfs-file-table10.bin
$(UTILITY_RBFS) -cstgpt ../storage 30 "11 Partition" 0 ./rbfs-partition-header11.bin ./rbfs-file-table11.bin
$(UTILITY_RBFS) -cstgpt ../storage 31 "12 Partition" 0 ./rbfs-partition-header12.bin ./rbfs-file-table12.bin
$(UTILITY_RBFS) -cstgpt ../storage 32 "13 Partition" 0 ./rbfs-partition-header13.bin ./rbfs-file-table13.bin
$(UTILITY_RBFS) -cstgpt ../storage 33 "14 Partition" 0 ./rbfs-partition-header14.bin ./rbfs-file-table14.bin
$(UTILITY_RBFS) -cstgpt ../storage 34 "15 Partition" 0 ./rbfs-partition-header15.bin ./rbfs-file-table15.bin
$(UTILITY_RBFS) -cstgpt ../storage 35 "16 Partition" 0 ./rbfs-partition-header16.bin ./rbfs-file-table16.bin
$(UTILITY_RBFS) -cstgpt ../storage 36 "17 Partition" 0 ./rbfs-partition-header17.bin ./rbfs-file-table17.bin
$(UTILITY_RBFS) -cstgpt ../storage 37 "18 Partition" 0 ./rbfs-partition-header18.bin ./rbfs-file-table18.bin
$(UTILITY_RBFS) -cstgpt ../storage 38 "19 Partition" 0 ./rbfs-partition-header19.bin ./rbfs-file-table19.bin
$(UTILITY_RBFS) -cstgpt ../storage 39 "20 Partition" 0 ./rbfs-partition-header20.bin ./rbfs-file-table20.bin
$(UTILITY_RBFS) -cfsh 1 20 ./rbfs-header.bin

cat $(IMAGE_YUKIBOOT) ./rbfs-header.bin \
	./rbfs-partition-header1.bin \
	./rbfs-partition-header2.bin \
	./rbfs-partition-header3.bin \
	./rbfs-partition-header4.bin \
	./rbfs-partition-header5.bin \
	./rbfs-partition-header6.bin \
	./rbfs-partition-header7.bin \
	./rbfs-partition-header8.bin \
	./rbfs-partition-header9.bin \
	./rbfs-partition-header10.bin \
	./rbfs-partition-header11.bin \
	./rbfs-partition-header12.bin \
	./rbfs-partition-header13.bin \
	./rbfs-partition-header14.bin \
	./rbfs-partition-header15.bin \
	./rbfs-partition-header16.bin \
	./rbfs-partition-header17.bin \
	./rbfs-partition-header18.bin \
	./rbfs-partition-header19.bin \
	./rbfs-partition-header20.bin \
	./rbfs-file-table1.bin \
	./rbfs-file-table2.bin \
	./rbfs-file-table3.bin \
	./rbfs-file-table4.bin \
	./rbfs-file-table5.bin \
	./rbfs-file-table6.bin \
	./rbfs-file-table7.bin \
	./rbfs-file-table8.bin \
	./rbfs-file-table9.bin \
	./rbfs-file-table10.bin \
	./rbfs-file-table11.bin \
	./rbfs-file-table12.bin \
	./rbfs-file-table13.bin \
	./rbfs-file-table14.bin \
	./rbfs-file-table15.bin \
	./rbfs-file-table16.bin \
	./rbfs-file-table17.bin \
	./rbfs-file-table18.bin \
	./rbfs-file-table19.bin \
	./rbfs-file-table20.bin \
> $(OUTPUT)
```