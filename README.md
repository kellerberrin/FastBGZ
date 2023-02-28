MT BGZ
========
A Fast BGZ (Block gzipped) Decompression Utility.
-------------------------------------------------


This small utility (400 lines) can decompress the block gzipped files (file.bgz) commonly used in genetics 
(VCF files). The utility has a very small memory footprint (< 20 megabytes) and uses multi-threading 
(15 threads) to achieve a 5 gigabytes/sec decompression rate. A 1.5 terabyte file can be decompressed 
in about 320 seconds on a Dell 7920 workstation with xeon 6140 cpus (72 hardware threads) and nvme disk storage. 
Your mileage may vary with different hardware.