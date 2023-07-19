/*
 * g_DataStructure_Lib.h
 *
 *  Created on: 2023. 3. 8.
 *      Author: Jonghun Kim
 */

#ifndef G_DATASTRUCTURE_H_
#define G_DATASTRUCTURE_H_

#include <g_Ethernet.h>
#include <g_TSN_TAS.h>
#include <hal_data.h>
#include <stdlib.h>

#define STRICT_QUEUE_MAX_SIZE     8

typedef struct Node Queue_Node;
typedef struct Head Queue_Head;
typedef enum _Queue_Dir Queue_Dir;
typedef enum _Queue_Insert Queue_Insert;

void RQ_Initial(uint8_t p, short mode);
void RQ_Insert(uint16_t p, Queue_Node *new, Queue_Node *cur, int ctrl);
void RQ_enqueue(Eth_RX_Frame *EthFrame, Queue_Insert idx);
Eth_RX_Frame *RQ_dequeue(volatile uint16_t p, int ctrl);

typedef enum _Queue_Type
{
    Queue_Strict_Pirority = -1,
    Queue_FIFO = 0,
    Queue_Static_Priority,
    Queue_Rate_Monotonic
}Queue_Type;

typedef enum _Queue_Dir
{
    front_queue = 0,
    inner_queue,
    back_queue
}Queue_Dir;

typedef enum _Queue_Insert
{
    first_in_queue = 0,
    last_in_queue
}Queue_Insert;

typedef struct Node
{
    Eth_RX_Frame         *frame;
    Queue_Node           *prev_Link;
    Queue_Node           *next_Link;
}Queue_Node;

typedef struct Head
{
    volatile uint32_t    count;
    Queue_Type           mode;
    Queue_Node           *node;
}Queue_Head;

#endif /* G_DATASTRUCTURE_H_ */
