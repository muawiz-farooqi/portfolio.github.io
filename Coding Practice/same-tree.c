/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */
bool isSameTree(struct TreeNode* p, struct TreeNode* q)
{
    if (p == NULL && q == NULL)
    {
        return true;
    }
    if (p != NULL && q != NULL && p->val == q->val)
    {
        if ((p->left == NULL && q->left == NULL) || (p->left != NULL && q->left != NULL && isSameTree(p->left, q->left)))
        {
            if ((p->right == NULL && q->right == NULL) || (p->right != NULL && q->right != NULL && isSameTree(p->right, q->right)))
            return true;
        }
        
    }

    return false;
}