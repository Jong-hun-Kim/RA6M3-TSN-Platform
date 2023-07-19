/*
 * g_DataStructure_lib.c
 *
 *  Created on: 2023. 3. 8.
 *      Author: Jonghun Kim
 */

#include <g_DataStructure.h>
#include <g_TSN_TAS.h>

Queue_Node              *ptr;
volatile Queue_Head     head[STRICT_QUEUE_MAX_SIZE];

void RQ_Initial(uint8_t p, short mode)
{   // Head is empty (There isn't Node)

    if (mode == Queue_Strict_Pirority)
        for (uint8_t i = 0; i < STRICT_QUEUE_MAX_SIZE; i++)
            head[i].mode = Queue_FIFO;
    else
    {
        head[p].mode = mode;
        head[p].count = 0;
        head[p].node = NULL;
    }
}
void RQ_Insert(uint16_t p, Queue_Node *new, Queue_Node *cur, int ctrl)
{
    switch(ctrl)
    {
        case front_queue: // if you insert new node front of compare node.
            cur->prev_Link = new;
            new->next_Link = cur;
            new->prev_Link = NULL;
            head[p].node = new;
            break;
        case inner_queue: // If you insert new node between compare node and compare node's previous node.
            new->next_Link = cur;
            cur->prev_Link->next_Link = new;
            new->prev_Link = cur->prev_Link;
            new->next_Link->prev_Link = new;
            break;
        case back_queue: // If you insert new node behind of compare node.
            cur->next_Link = new;
            new->prev_Link = cur;
            new->next_Link = NULL;
            break;
    }
}
void RQ_enqueue(Eth_RX_Frame *EthFrame, Queue_Insert idx)
{
    Queue_Node *temp;
    uint16_t p = EthFrame->vlan_tag.PCP;
    EthFrame->payload[10] = (uint8_t)p;
    Queue_Node *new = (Queue_Node*)calloc(1, sizeof(Queue_Node));

    new->frame = EthFrame;

    if(head[p].node == NULL) // If queue is empty
    {
        /* If queue is empty, insert first node in empty head */
        head[p].count++;
        new->prev_Link = NULL;
        new->next_Link = NULL;

        head[p].node = new;
    }
    else if (head[p].node != NULL) // if queue isn't empty
    {
        head[p].count++;

        switch(head[p].mode) {
            case Queue_FIFO:
                temp = head[p].node;

                switch(idx)
                {
                    case first_in_queue:
                        RQ_Insert(p, new, temp, front_queue);
                        break;
                    case last_in_queue:
                        if (temp->next_Link == NULL) // One Node is only existed in Queue.
                            RQ_Insert(p, new, temp, back_queue);
                        else // Multiple Nodes are existed in Queue.
                        {
                            while(temp->next_Link != NULL)
                                temp = temp->next_Link;

                            RQ_Insert(p, new, temp, back_queue);
                        }
                        break;
                }
                break;
            case Queue_Static_Priority:
                //////////// N O T    U S E D ////////////
                break;
            case Queue_Rate_Monotonic:
                //////////// N O T    U S E D ////////////
                break;
            default:
                //////////// N O T    U S E D ////////////
                break;
        }
    }

    free(new);
}

Eth_RX_Frame *RQ_dequeue(volatile uint16_t p, int ctrl)
{
    if (head[p].count > 0)
        head[p].count--;
    ptr = head[p].node;

    //////// Dequeuing Some Node Data ////////
    switch(ctrl)
    {
        case front_queue:
            if(ptr->next_Link == NULL)
                head[p].node = NULL;
            else
            {
                head[p].node = ptr->next_Link;
                head[p].node->prev_Link = NULL;
            }
            break;
        case back_queue:
            if(ptr->next_Link == NULL)
                head[p].node = NULL;
            else
            {
                while(ptr->next_Link != NULL)
                    ptr = ptr->next_Link;
                ptr->prev_Link->next_Link = NULL;
            }
            break;
    }

    return ptr->frame;
}
