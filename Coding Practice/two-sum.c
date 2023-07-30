int* twoSum(int* nums, int numsSize, int target, int* returnSize)
{
    *returnSize = 2;

    int* indices = (int*) malloc( 2 * sizeof(int) );

    for (int i = 0; i < numsSize-1; i++)
    {
        for(int j = i+1; j < numsSize; j++)
        {
            if (nums[i] + nums[j] == target)
            {
                indices[0] = i;
                indices[1] = j;

                return indices;
            }
        }
    }

    return indices;
}