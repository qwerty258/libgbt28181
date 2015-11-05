#include "XMLUtility.h"

xmlDocPtr find_element(xmlDocPtr xml_document_pointer, char* element_name)
{
    if(NULL != xml_document_pointer &&
       NULL != element_name)
    {
        while(true)
        {
            if(NULL == xml_document_pointer)
            {
                break;
            }
            if(0 == xmlStrncmp(xml_document_pointer->name, element_name, xmlStrlen(element_name)))
            {
                return xml_document_pointer;
            }
            xml_document_pointer = xml_document_pointer->next;
        }
    }
    return NULL;
}