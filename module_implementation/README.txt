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
* Prepare the environment for the implementation script:
	1. Unzip the GoAhead_1807_ECOSCALE.zip file
	2. Change paths in the goahead_ECOSCALE.bat batch script to direct your installed executable binaries (You may need to install the ISE 14.7)
	3. Source the goahead_ECOSCALE.bat batch script
* Steps to run the module implementation:
	1. Copy HLS-generated files (*.vhd, *.tcl, *.xdc) into the ./PR_WRP folder
	2. Adjust the boundary of the module (how many PR slots?):
		+ Available resources for 1 PR slot is as the above table
		+ Based on your synthesis report, calculate how many PR slots it requires
		+ Modify the Gen_KNL_v01.tcl script, and put your calculated number in:
			# Height of PR Module (Clock Region Units)
			set ::env(PRheight) x <--- (number of required slots: 1..4)
		+ Run the script:
			vivado -mode TCL -source Gen_KNL_vxx.tcl
* For re-implementation:
	1. Copy the PR_WRP.vhd file into the ./Sources folder
	2. Remove ./TOP_SYNTH and ./PR_WRP/OOC folders for smooth running