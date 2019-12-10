# zucl_fsp2018
# This is a part of our (Khoa Dang Pham, Anuj Vaishnav, and Malte Vesper) PhD projects at the University of Manchester, UK.

* Available resources for 1 PR slot:
-----------------------------------
|						|Available|
|-----------------------|---------|
|CLB LUTs				|32640    |
|LUT as Logic			|32640    |
|LUT as Memory			|17280    |
|CLB Registers			|65280    |
|Register as Flip Flop	|65280    |
|Register as Latch		|65280    |
|CARRY8					|4080     |
|F7 Muxes				|16320    |
|F8 Muxes				|8160     |
|F9 Muxes				|4080     |
|Block RAM Tile			|108      |
|RAMB36/FIFO			|108      |
|RAMB18					|216      |
|DSPs					|336      |
-----------------------------------

## 1 slot module
### Merging the module to the static
bitman.exe -m 0 180 138 239 ${module name}.bit zucl_stc.bit -F Merged_${module name}_zucl_stc.bit
bitman.exe -m 0 240 138 299 ${module name}.bit zucl_stc.bit -F Merged_${module name}_zucl_stc.bit
bitman.exe -m 0 300 138 359 ${module name}.bit zucl_stc.bit -F Merged_${module name}_zucl_stc.bit
bitman.exe -m 0 360 138 419 ${module name}.bit zucl_stc.bit -F Merged_${module name}_zucl_stc.bit
bitman.exe -m 0 180 138 239 ${module name}.bit zucl_stc.bit -F Merged_${module name}_zucl_stc.bit
### Cut out the partial bitstream
bitman.exe -x 0 180 138 239 Merged_${module name}_zucl_stc.bit -M 0 180 Partial_${module name}.bit
bitman.exe -x 0 240 138 299 Merged_${module name}_zucl_stc.bit -M 0 180 Partial_${module name}.bit
bitman.exe -x 0 300 138 359 Merged_${module name}_zucl_stc.bit -M 0 180 Partial_${module name}.bit
bitman.exe -x 0 360 138 419 Merged_${module name}_zucl_stc.bit -M 0 180 Partial_${module name}.bit
bitman.exe -x 0 180 138 239 Merged_${module name}_zucl_stc.bit -M 0 180 Partial_${module name}.bit


Contact: if you have any concern, please write to me at khoa.pham@manchester.ac.uk!
