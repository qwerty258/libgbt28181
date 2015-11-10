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
                return (xmlDocPtr)xml_document_pointer;
            }
            xml_document_pointer = xml_document_pointer->next;
        }
    }
    return NULL;
}

MANSCDP_xml_type get_xml_type(char* xml_type_string)
{
    if(NULL == xml_type_string)
    {
        return MANSCDP_xml_Unknown;
    }

    if(0 == xmlStrncmp(xml_type_string, "Control", xmlStrlen("Control")))
    {
        return MANSCDP_xml_Control;
    }
    if(0 == xmlStrncmp(xml_type_string, "Query", xmlStrlen("Query")))
    {
        return MANSCDP_xml_Query;
    }
    if(0 == xmlStrncmp(xml_type_string, "Notify", xmlStrlen("Notify")))
    {
        return MANSCDP_xml_Notify;
    }
    if(0 == xmlStrncmp(xml_type_string, "Response", xmlStrlen("Response")))
    {
        return MANSCDP_xml_Response;
    }

    return MANSCDP_xml_Unknown;
}

MANSCDP_command_type get_MANSCDP_command_type(char* MANSCDP_command_string)
{
    if(NULL == MANSCDP_command_string)
    {
        return MANSCDP_command_type_unknown;
    }

    if(0 == xmlStrncmp(MANSCDP_command_string, "DeviceInfo", xmlStrlen("DeviceInfo")))
    {
        return MANSCDP_DeviceInfo;
    }
    if(0 == xmlStrncmp(MANSCDP_command_string, "DeviceStatus", xmlStrlen("DeviceStatus")))
    {
        return MANSCDP_DeviceStatus;
    }
    if(0 == xmlStrncmp(MANSCDP_command_string, "Catalog", xmlStrlen("Catalog")))
    {
        return MANSCDP_Catalog;
    }

    return MANSCDP_command_type_unknown;
}

MANSCDP_on_off_line get_MANSCDP_online(char* MANSCDP_string)
{
    if(0 == xmlStrncmp(MANSCDP_string, "ONLINE", xmlStrlen("ONLINE")))
    {
        return MANSCDP_ONLINE;
    }
    if(0 == xmlStrncmp(MANSCDP_string, "OFFLINE", xmlStrlen("OFFLINE")))
    {
        return MANSCDP_OFFLINE;
    }

    return MANSCDP_UNKNOWN_ON_OFF_LINE;
}

MANSCDP_result_type get_MANSCDP_statues(char* MANSCDP_string)
{
    if(0 == xmlStrncmp(MANSCDP_string, "OK", xmlStrlen("OK")))
    {
        return MANSCDP_OK;
    }
    if(0 == xmlStrncmp(MANSCDP_string, "ERROR", xmlStrlen("ERROR")))
    {
        return MANSCDP_ERROR;
    }

    return MANSCDP_UNKNOWN_RESULT_TYPE;
}

void free_MANSCDP_xml_pointer(MANSCDP_xml** pointer)
{
    if(NULL != pointer || NULL != (*pointer))
    {
        osip_free((*pointer)->DeviceID);
        osip_free((*pointer)->Result);
        osip_free((*pointer)->DeviceType);
        osip_free((*pointer)->Manufacturer);
        osip_free((*pointer)->Model);
        osip_free((*pointer)->Firmware);
        osip_free((*pointer)->p_MANSCDP_device);
        // do not free p_client_configurations
        //osip_free((*pointer)->p_client_configurations);
    }

    osip_free(*pointer);
}