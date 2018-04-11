/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp && diff tree1.tmp $(srcdir)/tree1.res
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
#include "xmltree.h"

char *target;
char *RS;
char *kernel_name;

int bitstream_len;
int ndrangeid;
int n_args;
int off_addr_0;
int off_addr_1;
int off_addr_2;
int off_addr_3;
int off_addr_4;
int off_addr_5;
int off_addr_6;
int off_addr_7;
int off_addr_8;
int off_addr_9;
int off_addr_10;
int off_addr_11;
int off_addr_12;
int off_addr_13;
int off_addr_14;
int off_addr_15;
int base_addr_0;
int base_addr_1;
int base_addr_2;
int base_addr_3;
int base_addr_4;
int base_addr_5;
int base_addr;
extern unsigned int *inData;
int height;
int width;
int start_column;

int exec_cycles;
int power_estimate;
//unsigned int *inData; // for testing xmltree only

#ifdef LIBXML_TREE_ENABLED

/*
 *To compile this file using gcc you can type
 *gcc `xml2-config --cflags --libs` -o xmlexample libxml2-example.c
 */

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
void
print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE)
            printf("node type: Element, name: %s\n", cur_node->name);

        print_element_names(cur_node->children);
    }
}

void
parseContent(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
            if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"hw")) 
               || (!xmlStrcmp(cur_node->name, (const xmlChar *)"target"))
               || (!xmlStrcmp(cur_node->name, (const xmlChar *)"resource-string"))) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                printf("keyword: %s\n", key);
                xmlFree(key);
            }
        }

        parseContent(doc, cur_node->children);
    }
}

void
parseHW(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"hw")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                if (kernel_name == NULL)
                    kernel_name = malloc(strlen(key)*sizeof(char));
                strcpy(kernel_name, key);
                xmlFree(key);
            }
        }

        parseHW(doc, cur_node->children);
    }
}

void
parseRS(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"resource-string")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                if (RS == NULL)
                    RS = malloc(strlen(key)*sizeof(char));
                strcpy(RS, key);
                xmlFree(key);
            }
        }

        parseRS(doc, cur_node->children);
    }
}

void
parseTarget(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"target")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                if (target == NULL)
                    target = malloc(strlen(key)*sizeof(char));
                strcpy(target, key);
                xmlFree(key);
            }
        }

        parseTarget(doc, cur_node->children);
    }
}

void
parseLength(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"length")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                bitstream_len = atoi(key);
                xmlFree(key);
            }
        }

        parseLength(doc, cur_node->children);
    }
}

void
parseNDRangeID(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"ndrangeid")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                ndrangeid = atoi(key);
                xmlFree(key);
            }
        }

        parseNDRangeID(doc, cur_node->children);
    }
}

void
parseBitstream(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[9];
    int getkey;
    int i, j, len;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"bitstream")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                len = strlen(key);
                if (inData == NULL)
                    inData = malloc(bitstream_len*sizeof(unsigned int));
                j = 0;

                for (i = 0; i < len; i= i+8) {
                    subkey[0] = key[i];
                    subkey[1] = key[i+1];
                    subkey[2] = key[i+2];
                    subkey[3] = key[i+3];
                    subkey[4] = key[i+4];
                    subkey[5] = key[i+5];
                    subkey[6] = key[i+6];
                    subkey[7] = key[i+7];

                    getkey = (int)strtol(subkey, NULL, 16);
                    inData[j++] = getkey;
                }
                xmlFree(key);
            }
        }

        parseBitstream(doc, cur_node->children);
    }
}

//void
//parseBaseAddr(xmlDocPtr doc, xmlNode * a_node)
//{
//    xmlNode *cur_node = NULL;
//    xmlChar *key;
//
//    char subkey[11];
//    int getkey;
//    int i;
//    subkey[8] = NULL;
//    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
//        if (cur_node->type == XML_ELEMENT_NODE) {
//            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"base_address")) {
//                key = xmlNodeListGetString(doc, cur_node->children, 1);
//
//                subkey[0] = key[2];
//                subkey[1] = key[3];
//                subkey[2] = key[4];
//                subkey[3] = key[5];
//                subkey[4] = key[6];
//                subkey[5] = key[7];
//                subkey[6] = key[8];
//                subkey[7] = key[9];
//
//                getkey = (int)strtol(subkey, NULL, 16);
//                base_addr = getkey;
//                xmlFree(key);
//            }
//
//        parseBaseAddr(doc, cur_node->children);
//        }
//    }
//}

void
parseNArgs(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"n_args")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                n_args = atoi(key);
                xmlFree(key);
            }
        }

        parseNArgs(doc, cur_node->children);
    }
}

void
parseOffAddr0(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_0")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_0 = getkey;
                xmlFree(key);
            }

        parseOffAddr0(doc, cur_node->children);
        }
    }
}

void
parseOffAddr1(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_1")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_1 = getkey;
                xmlFree(key);
            }

        parseOffAddr1(doc, cur_node->children);
        }
    }
}

void
parseOffAddr2(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_2")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_2 = getkey;
                xmlFree(key);
            }

        parseOffAddr2(doc, cur_node->children);
        }
    }
}

void
parseOffAddr3(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_3")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_3 = getkey;
                xmlFree(key);
            }

        parseOffAddr3(doc, cur_node->children);
        }
    }
}

void
parseOffAddr4(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_4")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_4 = getkey;
                xmlFree(key);
            }

        parseOffAddr4(doc, cur_node->children);
        }
    }
}

void
parseOffAddr5(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_5")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_5 = getkey;
                xmlFree(key);
            }

        parseOffAddr5(doc, cur_node->children);
        }
    }
}

void
parseOffAddr6(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_6")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_6 = getkey;
                xmlFree(key);
            }

        parseOffAddr6(doc, cur_node->children);
        }
    }
}

void
parseOffAddr7(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_7")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_7 = getkey;
                xmlFree(key);
            }

        parseOffAddr7(doc, cur_node->children);
        }
    }
}

void
parseOffAddr8(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_8")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_8 = getkey;
                xmlFree(key);
            }

        parseOffAddr8(doc, cur_node->children);
        }
    }
}

void
parseOffAddr9(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_9")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_9 = getkey;
                xmlFree(key);
            }

        parseOffAddr9(doc, cur_node->children);
        }
    }
}

void
parseOffAddr10(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_10")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_10 = getkey;
                xmlFree(key);
            }

        parseOffAddr10(doc, cur_node->children);
        }
    }
}

void
parseOffAddr11(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_11")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_11 = getkey;
                xmlFree(key);
            }

        parseOffAddr11(doc, cur_node->children);
        }
    }
}

void
parseOffAddr12(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_12")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_12 = getkey;
                xmlFree(key);
            }

        parseOffAddr12(doc, cur_node->children);
        }
    }
}

void
parseOffAddr13(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_13")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_13 = getkey;
                xmlFree(key);
            }

        parseOffAddr13(doc, cur_node->children);
        }
    }
}

void
parseOffAddr14(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_14")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_14 = getkey;
                xmlFree(key);
            }

        parseOffAddr14(doc, cur_node->children);
        }
    }
}

void
parseOffAddr15(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"offset_address_15")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                off_addr_15 = getkey;
                xmlFree(key);
            }

        parseOffAddr15(doc, cur_node->children);
        }
    }
}

void
parseBaseAddr0(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"base_address_island_0")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                base_addr_0 = getkey;
                xmlFree(key);
            }

        parseBaseAddr0(doc, cur_node->children);
        }
    }
}

void
parseBaseAddr1(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"base_address_island_1")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                base_addr_1 = getkey;
                xmlFree(key);
            }

        parseBaseAddr1(doc, cur_node->children);
        }
    }
}

void
parseBaseAddr2(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"base_address_island_2")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                base_addr_2 = getkey;
                xmlFree(key);
            }

        parseBaseAddr2(doc, cur_node->children);
        }
    }
}

void
parseBaseAddr3(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"base_address_island_3")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                base_addr_3 = getkey;
                xmlFree(key);
            }

        parseBaseAddr3(doc, cur_node->children);
        }
    }
}

void
parseBaseAddr4(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"base_address_island_4")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                base_addr_4 = getkey;
                xmlFree(key);
            }

        parseBaseAddr4(doc, cur_node->children);
        }
    }
}

void
parseBaseAddr5(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    char subkey[11];
    int getkey;
    int i;
    subkey[8] = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"base_address_island_5")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);

                subkey[0] = key[2];
                subkey[1] = key[3];
                subkey[2] = key[4];
                subkey[3] = key[5];
                subkey[4] = key[6];
                subkey[5] = key[7];
                subkey[6] = key[8];
                subkey[7] = key[9];

                getkey = (int)strtol(subkey, NULL, 16);
                base_addr_5 = getkey;
                xmlFree(key);
            }

        parseBaseAddr5(doc, cur_node->children);
        }
    }
}
int
getBaseAddr(int island)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    /*parse the file and get the DOM */
    doc = xmlReadFile("./address_map.xml", NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file ./address_map.xml\n");
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    switch(island)
    {
        case 0:
            parseBaseAddr0(doc, root_element);
            return base_addr_0;
        case 1:
            parseBaseAddr1(doc, root_element);
            return base_addr_1;
        case 2:
            parseBaseAddr2(doc, root_element);
            return base_addr_2;
        case 3:
            parseBaseAddr3(doc, root_element);
            return base_addr_3;
        case 4:
            parseBaseAddr4(doc, root_element);
            return base_addr_4;
        case 5:
            parseBaseAddr5(doc, root_element);
            return base_addr_5;
        default:
            return NULL;
    }
}

void
parseHeight(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"height")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                height = atoi(key);
                xmlFree(key);
            }
        }

        parseHeight(doc, cur_node->children);
    }
}

void
parseWidth(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"width")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                width = atoi(key);
                xmlFree(key);
            }
        }

        parseWidth(doc, cur_node->children);
    }
}

void
parseStartColumn(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"start_column")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                start_column = atoi(key);
                xmlFree(key);
            }
        }

        parseStartColumn(doc, cur_node->children);
    }
}

void
parseExecCycles(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"exec_cycles")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                exec_cycles = atoi(key);
                xmlFree(key);
            }
        }

        parseExecCycles(doc, cur_node->children);
    }
}

void
parsePowerEst(xmlDocPtr doc, xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *key;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"power_estimate")) {
                key = xmlNodeListGetString(doc, cur_node->children, 1);
                power_estimate = atoi(key);
                xmlFree(key);
            }
        }

        parsePowerEst(doc, cur_node->children);
    }
}
/**
 * Simple example to parse a file called "file.xml", 
 * walk down the DOM, and print the name of the 
 * xml elements nodes.
 */
#if 0
int
main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    int i;
    int addr;
    if (argc != 2)
        return(1);

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(argv[1], NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", argv[1]);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    print_element_names(root_element);

//    parseTarget(doc, root_element);
//    printf("target device %s\n", target);
//    parseRS(doc, root_element);
//    printf("resource string %s\n", RS);
//    parseLength(doc, root_element);
//    printf("bitstream length %d\n", bitstream_len);
//    parseBaseAddr(doc, root_element);
//    printf("Base address 0x%08x\n", base_addr);
//    parseNArgs(doc, root_element);
//    printf("No of args %d\n", n_args);
//    parseNDRangeID(doc, root_element);
//    printf("NDRangeID %d\n", ndrangeid);
//    parseOffAddr0(doc, root_element);
//    printf("Off addr 0 0x%08x\n", off_addr_0);
//    parseOffAddr1(doc, root_element);
//    printf("Off addr 1 0x%08x\n", off_addr_1);
//    parseOffAddr2(doc, root_element);
//    printf("Off addr 2 0x%08x\n", off_addr_2);
//    parseOffAddr3(doc, root_element);
//    printf("Off addr 3 0x%08x\n", off_addr_3);
//    parseOffAddr4(doc, root_element);
//    printf("Off addr 4 0x%08x\n", off_addr_4);
//    parseOffAddr5(doc, root_element);
//    printf("Off addr 5 0x%08x\n", off_addr_5);
//    parseOffAddr6(doc, root_element);
//    printf("Off addr 6 0x%08x\n", off_addr_6);
//    parseOffAddr7(doc, root_element);
//    printf("Off addr 7 0x%08x\n", off_addr_7);
//    parseOffAddr8(doc, root_element);
//    printf("Off addr 8 0x%08x\n", off_addr_8);
//    parseOffAddr9(doc, root_element);
//    printf("Off addr 9 0x%08x\n", off_addr_9);
//    parseOffAddr10(doc, root_element);
//    printf("Off addr 10 0x%08x\n", off_addr_10);
//    parseOffAddr11(doc, root_element);
//    printf("Off addr 11 0x%08x\n", off_addr_11);
//    parseOffAddr12(doc, root_element);
//    printf("Off addr 12 0x%08x\n", off_addr_12);
//    parseOffAddr13(doc, root_element);
//    printf("Off addr 13 0x%08x\n", off_addr_13);
//    parseOffAddr14(doc, root_element);
//    printf("Off addr 14 0x%08x\n", off_addr_14);
//    parseOffAddr15(doc, root_element);
//    printf("Off addr 15 0x%08x\n", off_addr_15);
//    parseBitstream(doc, root_element);
//    printf("inData array\n");
//    for (i = 0; i < bitstream_len; i++)
//        printf("0x%08x\n", inData[i]);
//    parseBaseAddr0(doc, root_element);
//    printf("Base addr of island 0 0x%08x\n", base_addr_0);
//    parseBaseAddr1(doc, root_element);
//    printf("Base addr of island 1 0x%08x\n", base_addr_1);
//    parseBaseAddr2(doc, root_element);
//    printf("Base addr of island 2 0x%08x\n", base_addr_2);
//    parseBaseAddr3(doc, root_element);
//    printf("Base addr of island 3 0x%08x\n", base_addr_3);
//    parseBaseAddr4(doc, root_element);
//    printf("Base addr of island 4 0x%08x\n", base_addr_4);
//    parseBaseAddr5(doc, root_element);
//    printf("Base addr of island 5 0x%08x\n", base_addr_5);
    for (i = 0; i < 6; i++)
    {
        addr = getBaseAddr(i);
        printf("getBaseAddr: Base addr of island %d 0x%08x\n", i, addr);
    }

    if (RS != NULL)
        free(RS);
    if (target != NULL)
        free(target);
    if (inData != NULL)
        free(inData);
    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return 0;
}
#endif
#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif
