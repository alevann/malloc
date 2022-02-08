# malloc

Implementation of `malloc` I made following this [CS170 lab assignment](https://sites.cs.ucsb.edu/~rich/class/cs170/labs/lab1.malloc/).

I modified slightly the header file provided in the lab meaning the tests files (also provided in the lab) won't work.

## Building and testing

You can build with `make`, should work on both Windows and Linux, probably MacOS as well but don't quote me on that; defaults to build with `gcc` but you can change that in the makefile.

Running the resulting executable will run the tests.

```sh
make malloctests
```
