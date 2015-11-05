#ifndef _XMLUTILITY_H_
#define _XMLUTILITY_H_

#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

xmlDocPtr find_element(xmlDocPtr xml_document_pointer);

#endif // _XMLUTILITY_H_
