FastBGZ
========
A Fast BGZ (block gzipped) Decompression Utility.
-------------------------------------------------


This small utility (400 lines) can decompress the block gzipped files (file.bgz) commonly used in genetics 
(VCF files). The utility has a very small runtime memory footprint (< 20 megabytes) and uses multi-threading 
to achieve a 5 gigabytes/sec decompression rate. A 1.5 terabyte file can be decompressed 
in about 320 seconds on a Dell 7920 workstation with nvme disk storage. Your mileage may vary with different 
hardware.

The key decompression object is BGZStreamIO (kel_bzip_workflow.h) which is about 400 lines of code. The object
uses zlib to decompress data blocks in a multi-threaded pipeline (a threadpool with input/output 
queues). The object exposes an IO stream-like interface and was designed to be the lowest level of a 
parser for very large genetics (VCF) files.

The kel_thread directory contains a threadpool (kel_workflow_threads.h)
and a thread pipeline. The pipeline (kel_workflow_pipeline.h) is a threadpool with threadsafe input and output 
queues. Enqueue zlib compressed blocks on the input, dequeue decompressed data at the output queue. There are also 
2 thread safe queues, the most notable of which is the tidal queue (kel_queue_tidal.h). The tidal queue has 
bounded size (high tide) and balances CPU load between producer and consumer threads by implementing a 2 state 
regime, flood tide and ebb tide (producers blocked).

As an example in using the BGZStreamIO object, a toy application (kel_bgz_main.cpp) is provided which decompresses
files to stdout (all app logging is to stderr) and verifies that files are in valid block bgz format (RFC1952).
In addition, there are some (5) application infrastructure files in directory kel_app that are not relevant to
the decompression logic.

The software is written in C++20 (g++12.1) on linux kernel 5.15. It has not been tested on other platforms but 
there is no OS specific code (except possibly zlib). 
A cmake file is provided for easy compilation and you must have the popular spdlog C++ logging utility installed.

If you have any questions about the software or want more information on parsing large genetics (VCF) files in
C++ please email/message me.

James McCulloch.