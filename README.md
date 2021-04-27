# SolARModulePopSift

[![License](https://img.shields.io/github/license/SolARFramework/SolARModuleOpenGV?style=flat-square&label=License)](https://www.apache.org/licenses/LICENSE-2.0)

**SolARModulePopSift** is a module based on [Popsift](https://github.com/alicevision/popsift). Popsift is released under the [MPL v2 license](https://www.freebsd.org/internal/software-license.html).

# PopSift
PopSift is an open-source implementation of the SIFT algorithm in CUDA. PopSift tries to stick as closely as possible to David Lowe's famous paper [1], while extracting features from an image in real-time at least on an NVidia GTX 980 Ti GPU.

## HW requirements

SolARModulePopSift compiles and works with NVidia cards of compute capability >= 3.0 (including the GT 650M), but the code is developed with the compute capability 5.2 card GTX 980 Ti in mind.
CUDA SDK 11 does no longer support compute capability 3.0. 3.5 is still supported with deprecation warning.

## License

PopSift is licensed under [MPL v2 license](COPYING.md).
SIFT was patented in the United States from 1999-03-08 to 2020-03-28. See the [patent link](https://patents.google.com/patent/US6711293B1/en) for more information.
PopSift license only concerns the PopSift source code and does not release users of this code from any requirements that may arise from patents.
SolARModulePopSift is licensed under [Apache v2 license].

```bibtex
@inproceedings{Griwodz2018Popsift,
	 author = {Griwodz, Carsten and Calvet, Lilian and Halvorsen, P{\aa}l},
	 title = {Popsift: A Faithful SIFT Implementation for Real-time Applications},
	 booktitle = {Proceedings of the 9th {ACM} Multimedia Systems Conference},
	 series = {MMSys '18},
	 year = {2018},
	 isbn = {978-1-4503-5192-8},
	 location = {Amsterdam, Netherlands},
	 pages = {415--420},
	 numpages = {6},
	 doi = {10.1145/3204949.3208136},
	 acmid = {3208136},
	 publisher = {ACM},
	 address = {New York, NY, USA},
}
```