# 16th Coding Test Problem

### Problem
1. For two integers with binary representations, implement an efficient comparison circuit over the integers [CKK15].
    * A function comparisonTest() should support both less-than and greater-than circuits.
    * If 'bool lessThan = 1', comparisonTest(lessThan) becomes a less-than circuit.
    * If 'bool lessThan = 0', comparisonTest(lessThan) becomes a greater-than circuit.
    * You can add additional parameters.
2. You can pass this coding test if your code is faster than us.

### Schedule
* Submission Due Date: ~ July 31, 2017 11:59 AM
* Interim Meetings: July 25, July 27 (tentative) 

### How to Submit?
* make the whole folders into one compressd file whose name is your name or ID, e.g., heewon.zip.
* send an e-mail to {runghyun, cocomi11, nemokjs}@snu.ac.kr with the attachments.

### Rule
1. You cannot discuss with other examinees about the coding test.
2. You can refer any material or ask us.
3. You can add any header files(s) and source files(s) whatever you want.
4. Header files should be in the 'include' folder and source file(s) should be in the 'src' folder.

### How to Compile?
1. You have to modify CMakeLists.txt and lib/CMakeLists.txt.
    * need to change the paths HElib and NTL directories according to your system. (If you have installed GMP and GF2X, then change it, too.)
    * replace '<YourName>' with your name.
2. $ cmake CMakeLists.txt
3. $ make
4. You can make your own Makefile or CMakeLists.txt.