cat ./Scripts/module_blocker.tcl|sed 's/gnd/VCC/Ig' > ./Scripts/t1.tcl
cat ./Scripts/t1.tcl|sed 's/\/G/\/P/g' > ./Scripts/module_blocker_VCC.tcl
