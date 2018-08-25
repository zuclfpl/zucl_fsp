# Read the macros from DCP file and generate a GOA file to present the macro

set filename "macros_tunnel.goa"
set fileId [open $filename "w"]

# get the maximum number of bits for the interface
set max_j [get_property BUS_WIDTH [get_nets inst_StaticPlaceholder/NET_X1Y3_east[0]]]

# Read each signal from the bus
for {set j 0} {$j<$max_j} {incr j} 	{
puts $fileId "";  

puts $fileId "#East ($j);";  

# get each node from the net and generate a command to GoAhead to exclude this node/wire from the blocker
set data [get_nodes -of_objects  [get_nets inst_StaticPlaceholder/NET_X1Y3_east[$j]]]
foreach line $data {
if [regexp  "(INT.*)+(/)+(.*)" $line match5 tile slash wire] {
  puts $fileId "ExcludePortsFromBlocking PortName=$wire Location=$tile CheckForExistence=False IncludeAllPorts=True;";  
}
}
}

# repeat the above process for the West bus signals
set max_j [get_property BUS_WIDTH [get_nets inst_StaticPlaceholder/NET_X1Y3_west[0]]]

# INT_X10Y250/EE4_E_BEG4
for {set j 0} {$j<$max_j} {incr j} 	{
puts $fileId "";  

puts $fileId "#West ($j);";  

set data [get_nodes -of_objects  [get_nets inst_StaticPlaceholder/NET_X1Y3_west[$j]]]
foreach line $data {
if [regexp  "(INT.*)+(/)+(.*)" $line match5 tile slash wire] {
  puts $fileId "ExcludePortsFromBlocking PortName=$wire Location=$tile CheckForExistence=False IncludeAllPorts=True;";  
}
}
}

 
close $fileId