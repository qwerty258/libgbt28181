#ifndef _WORKING_THREAD_H_
#define _WORKING_THREAD_H_

void* register_working_thread(void* arg);
void* event_working_thread(void* arg);
void* keepalive_working_thread(void* arg);
void* MANSCDP_xml_message_working_thread(void* arg);
void* RTP_data_receiving_working_thread(void* arg);

#endif // !_WORKING_THREAD_H_