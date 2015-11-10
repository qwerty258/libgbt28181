#ifndef _XMLUTILITY_H_
#define _XMLUTILITY_H_

#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "clientConfigDefine.h"

xmlDocPtr find_element(xmlDocPtr xml_document_pointer, char* element_name);
MANSCDP_xml_type get_xml_type(char* xml_type_string);
MANSCDP_command_type get_MANSCDP_command_type(char* MANSCDP_command_string);
MANSCDP_on_off_line get_MANSCDP_online(char* MANSCDP_string);
MANSCDP_result_type get_MANSCDP_statues(char* MANSCDP_string);
void free_MANSCDP_xml_pointer(MANSCDP_xml** pointer);
void parse_MANSCDP_xml_device_list(xmlDocPtr xml_device_list, unsigned long long count, MANSCDP_device* p_MANSCDP_device);

#endif // _XMLUTILITY_H_
