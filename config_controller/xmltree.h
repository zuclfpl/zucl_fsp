#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

void print_element_names(xmlNode * a_node);
void parseContent(xmlDocPtr doc, xmlNode * a_node);

void parseHW(xmlDocPtr doc, xmlNode * a_node);
void parseRS(xmlDocPtr doc, xmlNode * a_node);
void parseTarget(xmlDocPtr doc, xmlNode * a_node);
void parseLength(xmlDocPtr doc, xmlNode * a_node);
void parseBitstream(xmlDocPtr doc, xmlNode * a_node);
void parseNDRangeID(xmlDocPtr doc, xmlNode * a_node);
//void parseBaseAddr(xmlDocPtr doc, xmlNode * a_node);
void parseNArgs(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr0(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr1(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr2(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr3(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr4(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr5(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr6(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr7(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr8(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr9(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr10(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr11(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr12(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr13(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr14(xmlDocPtr doc, xmlNode * a_node);
void parseOffAddr15(xmlDocPtr doc, xmlNode * a_node);
void parseBaseAddr0(xmlDocPtr doc, xmlNode * a_node);
void parseBaseAddr1(xmlDocPtr doc, xmlNode * a_node);
void parseBaseAddr2(xmlDocPtr doc, xmlNode * a_node);
void parseBaseAddr3(xmlDocPtr doc, xmlNode * a_node);
void parseBaseAddr4(xmlDocPtr doc, xmlNode * a_node);
void parseBaseAddr5(xmlDocPtr doc, xmlNode * a_node);

int  getBaseAddr(int island);

void parseHeight(xmlDocPtr doc, xmlNode * a_node);
void parseWidth(xmlDocPtr doc, xmlNode * a_node);
void parseStartColumn(xmlDocPtr doc, xmlNode * a_node);
void parseExecCycles(xmlDocPtr doc, xmlNode * a_node);
void parsePowerEst(xmlDocPtr doc, xmlNode * a_node);
void parseClkFreq(xmlDocPtr doc, xmlNode * a_node);

void parseWGVector(xmlDocPtr doc, xmlNode * a_node);
void parseInputParamWidth(xmlDocPtr doc, xmlNode * a_node);
void parseOutputParamWidth(xmlDocPtr doc, xmlNode * a_node);
