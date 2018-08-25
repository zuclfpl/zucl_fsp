

# Simplified String (Resource Index)  - From Bogdan
#    00 00 00 00 00 11 11 11 11 11 22 22 22 22 22 22 22 22 22 22 ....
#    01 23 45 67 89 01 23 45 67 89 01 23 45 67 89 01 23 45 67 89 ....
#    LM LB LM DM LM LB LM DM LM DM LM LB LM DM LM LB LM DM LM DM LM LB 
# ImsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLM
  # 0  0  0  0  0  0  0  0  0  0  1  1  1  1  1  1  1  1  1  1  2  .....  
  # 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  ..... 
# Original String (Switchbox Index) 



# PBLOCK Generation from Resource Index (simplified string)
# The resource string is screened row by row and the resize_pblock command adds the resources to the pblock
for {set j $::env(start)} {$j<=$::env(stop)} {incr j}	{
puts $j

if {[expr {$j/2.0}] > [expr {$j/2}]} {
	# odd index (resource left to the switchbox) 
	set pblock_par [get_sites -of_objects  [get_tiles *X[expr {$j/2+1}]Y$::env(CRrow)]]
	puts $pblock_par
	# only logic resources on each side of the switchbox
	# example: SLICE_X11Y240 SLICE_X12Y240
	if { [llength $pblock_par] == 2} {
		set pblock_par_left [lindex $pblock_par 0]
		set f [split [lindex $pblock_par 0] Y]
		set h [lindex $f 0]Y[expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]
		set ee "$pblock_par_left:$h "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee
									 }	


	# Logic + DSPs resources
	# example: SLICE_X13Y240 DSP48E2_X2Y96 DSP48E2_X2Y97
	if { [llength $pblock_par] == 3} {
		set pblock_par_left [lindex $pblock_par 0]
		set f [split [lindex $pblock_par 0] Y]
		set h [lindex $f 0]Y[expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]
		set ee "$pblock_par_left:$h "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee


									 }	

	# Logic + BRAMs resources
	# example: RAMB18_X2Y97 RAMB18_X2Y96 RAMB36_X2Y48 SLICE_X18Y240
	if { [llength $pblock_par] == 4} {
		set pblock_par_left [lindex $pblock_par 0]
		set f [split [lindex $pblock_par 0] Y]
		set h [lindex $f 0]Y[expr {[lindex $f 1]+($::env(PRheight)*22)+(($::env(PRheight)-1)*2)}]
		set ee "$pblock_par_left:$h "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee

		set pblock_par_left [lindex $pblock_par 1]
		set f [split [lindex $pblock_par 1] Y]
		set h [lindex $f 0]Y[expr {[lindex $f 1]+($::env(PRheight)*22)+(($::env(PRheight)-1)*2)}]
		set ee "$pblock_par_left:$h "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee

		set pblock_par_left [lindex $pblock_par 2]
		set f [split [lindex $pblock_par 2] Y]
		set h [lindex $f 0]Y[expr {[lindex $f 1]+($::env(PRheight)*11)+(($::env(PRheight)-1)*1)}]
		set ee "$pblock_par_left:$h "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee

		

									 }	



									 } else {

	# even index (resource right to the switchbox) 
	set pblock_par [get_sites -of_objects  [get_tiles *X[expr {$j/2}]Y$::env(CRrow)]]
	puts $pblock_par
	# only logic resources on each side of the switchbox
	# example: SLICE_X11Y240 SLICE_X12Y240
	if { [llength $pblock_par] == 2} {
		set pblock_par_right [lindex $pblock_par 1]
		set g [split [lindex $pblock_par 1] Y]
		set i [lindex $g 0]Y[expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]
		set ee "$pblock_par_right:$i "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee
		
									 }	

									 
	# Logic + DSPs resources
	# example: SLICE_X13Y240 DSP48E2_X2Y96 DSP48E2_X2Y97
	if { [llength $pblock_par] == 3} {
		set pblock_par_right [lindex $pblock_par 1]
		set g [split [lindex $pblock_par 1] Y]
		set i [lindex $g 0]Y[expr {[lindex $g 1]+($::env(PRheight)*22)+(($::env(PRheight)-1)*2)}]
		set ee "$pblock_par_right:$i "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee


		set pblock_par_right [lindex $pblock_par 2]
		set g [split [lindex $pblock_par 2] Y]
		set i [lindex $g 0]Y[expr {[lindex $g 1]+($::env(PRheight)*22)+(($::env(PRheight)-1)*2)}]
		set ee "$pblock_par_right:$i "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee

		
	
									 } 	

	# Logic + BRAMs resources
	# example: RAMB18_X2Y97 RAMB18_X2Y96 RAMB36_X2Y48 SLICE_X18Y240
	if { [llength $pblock_par] == 4} {
		set pblock_par_right [lindex $pblock_par 3]
		set f [split [lindex $pblock_par 3] Y]
		set h [lindex $f 0]Y[expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]
		set ee "$pblock_par_right:$h "
		puts $ee
		resize_pblock pblock_PR_Kernel -add $ee

		

									 }	
									 
									 
									 
											 }

										}
										
								

# Switchbox indexes for GoAhead								
set ::env(ModLowerLeftY) $::env(CRrow)
if {[expr {$::env(start)/2.0}] > [expr {$::env(start)/2}]} {
	puts "Start odd = INT_X[expr {$::env(start)/2+1}]Y$::env(CRrow)"
	set ::env(ModLowerLeftX) [expr {$::env(start)/2+1}]
											 } else {
	puts "Start even = INT_X[expr {$::env(start)/2}]Y$::env(CRrow)"
	set ::env(ModLowerLeftX) [expr {$::env(start)/2}]
													}

set ::env(ModUpperRightY)  [expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]
if {[expr {$::env(stop)/2.0}] > [expr {$::env(stop)/2}]} 	{
	puts "Stop odd = INT_X[expr {$::env(stop)/2+1}]Y[expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]"
	set ::env(ModUpperRightX)  [expr {$::env(stop)/2+1}]

											} else 	{
	puts "Stop even = INT_X[expr {$::env(stop)/2}]Y[expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]"
	set ::env(ModUpperRightX)  [expr {$::env(stop)/2}]
													}

puts "ModLowerLeftX = $::env(ModLowerLeftX)"
puts "ModLowerLeftY = $::env(ModLowerLeftY)"
puts "ModUpperRightX = $::env(ModUpperRightX)"
puts "ModUpperRightY = $::env(ModUpperRightY)"



# Parameters for BITMAN
# Calculate COL paramenters for BITMAN
	# * In the Zynq UltraScale+ family: resource columns are organized as the pattern L-s-R
	# (left resouce column - Switchbox - right resource column)
		# + Let say the Switchbox has the X-Y coordinator
		# + The COL value of the L column is the value of (X*3+1)
		# + The COL value of the s column is the value of (X*3+2)
		# + The COL value of the R column is the value of (X*3+3)
set ::env(BitRow1) $::env(CRrow)
if {[expr {$::env(start)/2.0}] > [expr {$::env(start)/2}]} {
	puts "Start odd = INT_X[expr {$::env(start)/2+1}]Y$::env(CRrow)"
	set ::env(BitCol1) [expr {($::env(start)/2+1)*3+1}]
											 } else {
	puts "Start even = INT_X[expr {$::env(start)/2}]Y$::env(CRrow)"
	set ::env(BitCol1) [expr {($::env(start)/2)*3+3}]
													}

set ::env(BitRow2)  [expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]
if {[expr {$::env(stop)/2.0}] > [expr {$::env(stop)/2}]} 	{
	puts "Stop odd = INT_X[expr {$::env(stop)/2+1}]Y[expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]"
	set ::env(BitCol2) [expr {($::env(stop)/2+1)*3+1}]

											} else 	{
	puts "Stop even = INT_X[expr {$::env(stop)/2}]Y[expr {$::env(CRrow)+($::env(PRheight)*$::env(CRheight))-1}]"
	set ::env(BitCol2) [expr {($::env(stop)/2)*3+3}]
													}



puts "BitCol1 = $::env(BitCol1)"
puts "BitRow1 = $::env(BitRow1)"
puts "BitCol2 = $::env(BitCol2)"
puts "BitRow2 = $::env(BitRow2)"



											