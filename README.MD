# mmu simulator

<!-- ABOUT THE PROJECT -->

## About The Project

An MMU simulator with Physical Memory size of 128 and 256, with LRU as a page replacement algorithm, page table and TLB utilized.

<p align="right">(<a href="#top">back to top</a>)</p>

### Built With

- C
- Makefile

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->

## Getting Started

The simulation work out of the box, using the addresses in addresses.txt file as 32int and output csv file as a result.

### Prerequisites

- [GNUWin32](http://gnuwin32.sourceforge.net/install.html)
- [Git](https://git-scm.com/download)

### Installation

No installation needed

## Usage

1. Make sure that Makefile is installed
2. Make the mmu using makefile
   ```sh
   make mmu
   ```
3. Run the 128 version or 256 version
   128:
   ```sh
   ./mmu 128 BACKING_STORE.bin addresses.txt
   ```
   256:
   ```sh
   ./mmu 256 BACKING_STORE.bin addresses.txt
   ```
4. Check the result in the csv file as output.csv
