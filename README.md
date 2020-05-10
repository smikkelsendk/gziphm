
The compression tool *gzip* uses two different compression strategies (Lempel-Ziv and Huffman coding). These work great in tandem in most cases, however when the input is somewhat random in nature (like a DNA sequence) Lempel-Ziv uses a lot of CPU time without providing much compression. Huffman coding on the other hand is very useful for DNA sequences since it allows multiple nucleotides to be packed into one byte.

The underlying zlib implementation allows for using Huffman only, however it is not accessible from gzip /pigz commandline. I have therefore made a new versions of the two tools using Huffman only strategy, allowing much faster compression of especially DNA sequences without much loss in compression ratio.

Benchmarks:
Performed on i9-9900K (8 cores, 16 threads), 64 gb DDR4 3200MHz
All data stored on ramdisk (tmpfs)  during benchmark

TL;DR: *gziphm* is much faster with similar or in some cases even better compression ratio than *gzip --fast* for nucleotide sequences. For protein sequences *gziphm* is twice as fast as *gzip --fast* but also compresses worse. Zstd however performs on pair with regular gzip while being faster than gziphm, making it the obvious choice if compatibility is not a concern.
 

**Compression of first 5 gb of nt.fa (BLAST Nucleotide database)**
|Tool| Ratio | Compression speed (MB/s) | Decompression speed (MB/s)|
|--|--|--|--|
| gzip | 0.282  | 7.438| 235.003
| gzip --fast | 0.331 | 75.594 | 182.265
| gziphm | 0.295 | 156.733 | 193.734 

Multithreaded version of gzip for compression

|Tool| Ratio | Compression speed (MB/s)|
|--|--|--|--|
| pigz | 0.282 | 84.315 
| pigz --fast | 0.331 | 777.879| 
| pigzhm | 0.295 | 1064.449 |

Comparison with alternative tools
|Tool| Ratio | Compression speed (MB/s) | Decompression speed (MB/s)|
|--|--|--|--|
| lzop | 0.48 | 485.078| 499.415
| lz4 | 0.527 | 512.102 | 1272.999
| zstd | 0.294 | 187.285 | 705.72 

Conclusion: *gziphm* is almost on pair with regular *gzip* while being 20x as fast. Zstd is even faster, especially on decompression

**Human Genome hg19.fa 3.2 GB (Nucleotide sequence)**
|Tool| Ratio | Compression speed (MB/s) | Decompression speed (MB/s)|
|--|--|--|--|
| gzip | 0.296   | 7.694| 236.106
| gzip --fast | 0.339 | 76.598  | 185.523
| gziphm |  0.354 | 159.882 | 196.451

Multithreaded version of gzip for compression

|Tool| Ratio | Compression speed (MB/s)|
|--|--|--|--|
| pigz | 0.296 | 87.911 
| pigz --fast | 0.339 | 753.126| 
| pigzhm |0.354| 1074.91 |

Comparison with alternative tools
|Tool| Ratio | Compression speed (MB/s) | Decompression speed (MB/s)|
|--|--|--|--|
| lzop | 0.499 | 459.105| 471.155
| lz4 | 0.526| 464.981 | 1354.491
| zstd |  0.311 | 197.391 | 710.186 




**Compression of first 5 gb of nr.fa (BLAST Protein database)**
|Tool| Ratio | Compression speed (MB/s) | Decompression speed (MB/s)|
|--|--|--|--|
| gzip | 0.541 | 34.21| 150.886
| gzip --fast | 0.559 | 68.303 | 167.397
| gziphm | 0.657 | 132.361 | 167.495

Multithreaded version of gzip for compression

|Tool| Ratio | Compression speed (MB/s) 
|--|--|--|--|
| pigz | 0.541 | 341.835| 
| pigz --fast | 0.559 | 645.405| 
| pigzhm | 0.657 | 975.982 |

Comparison with alternative tools
|Tool| Ratio | Compression speed (MB/s) | Decompression speed (MB/s)|
|--|--|--|--|
| lzop | 0.897 | 624.01| 864.865
| lz4 | 0.86 | 767.962 | 1707.236
| zstd | 0.504 | 147.955 | 871.045 

Conclusion: If possible use zstd for protein sequences, as it is more than 4 times faster than regular gzip while giving better compression.
gziphm is more than 2x faster than gzip --fast, but compression is also worse.
