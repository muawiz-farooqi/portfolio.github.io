//Removes Nth node from end of a Linked List
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* removeNthFromEnd(struct ListNode* head, int n)
{
    struct ListNode* temp = head;
    struct ListNode* ret_head = head;
    int sz = 1;

    while (temp->next != NULL)
    {
        temp = temp->next;
        sz++;
    }
    if (sz == 1 && n == 1)
    {
        head->val = 0;
        head = NULL;
        return head;
    }
    else
    {
        int prev_index = sz - n;
        int remove_index = prev_index + 1;
        printf("prev_index = %d\n", prev_index);
        if (remove_index == 1)
        {
            ret_head = head->next;
        }
        else
        {
            ret_head = head;
        }
        
        if (remove_index == 1)
        {
            struct ListNode* remove_ptr = head;
            remove_ptr->val = 0;
            head = head->next;
        }
        else
        {
            for (int i = 1; i < prev_index; i++)
            {
                head = head->next;
            }

            struct ListNode* remove_ptr = head->next;
            remove_ptr->val = 0;
            head->next = remove_ptr->next;
            remove_ptr->next = NULL;
        }

        return ret_head;
    }
}