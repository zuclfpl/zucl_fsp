#!/user/bin/env python3

import sys
# Usage:
#       python gen_xml.py $bitstream_ascii.txt $module.xml

# meta-data which are put in the xml header
new_hw_name = "increment-3d"
new_target_device = "XCZU9EG"
new_resource_string = "ImsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLMsDMsLMsLRsLMsDMsLMsDMsLMsLRsLMsDMsLMsDMsLMsDMsLMsLRsLMsDMsLMsLRsLMs"
new_bitstream_length = ""
new_bitstream_ascii = ""

# data which are replace in the template.xml
old_hw_name = "hw_name"
old_target_device = "target_device"
old_resource_string = "island_resource_string"
old_bitstream_length = "bitstream_length"
old_bitstream_ascii = "bitstream_ascii"

# Bitstream ASCII keywords:
no_of_words_key = "Number of Words"
bitstream_key   = "Bitstream"

###############################################################################
def main(argv):
    bit_ascii = argv[0]
    template  = "./template.xml"
    output    = argv[1]

#    print "Opening the bitstream file " + bit_ascii
#    print "Opening the template file " + template
#    print "Opening the output file " + output

    b = open(bit_ascii, 'r+')
    t = open(template, 'r')
    o = open(output, 'wib')

    for line in b:
        if bitstream_key in line:
            list_of_words = line.split(" ")
            new_bitstream_ascii = list_of_words[1].strip("\n")
        elif no_of_words_key in line:
            list_of_words = line.split(" ")
#           print list_of_words[3].strip("\n")
            new_bitstream_length = list_of_words[3].strip("\n")

    for line in t:
        if old_hw_name in line:
#            print old_hw_name
            o.write(line.replace(old_hw_name, new_hw_name))
        elif old_target_device in line:
            o.write(line.replace(old_target_device, new_target_device))
        elif old_resource_string in line:
            o.write(line.replace(old_resource_string, new_resource_string))
        elif old_bitstream_length in line:
            o.write(line.replace(old_bitstream_length, new_bitstream_length))
        elif old_bitstream_ascii in line:
            o.write(line.replace(old_bitstream_ascii, new_bitstream_ascii))
        else:
            o.write(line)

###############################################################################
if __name__ == "__main__":
    main(sys.argv[1:])
