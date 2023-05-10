/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */


void helper(struct TreeNode* root, int *curr_sum)
{
    if(!root)
    {
        return;
    }

    helper(root->right, curr_sum);
    
    root->val = root->val + *curr_sum;
    *curr_sum = root->val;
    
    helper(root->left, curr_sum);
}


struct TreeNode* convertBST(struct TreeNode* root)
{  
    int curr_sum = 0;
    helper(root, &curr_sum);
    return root;
}