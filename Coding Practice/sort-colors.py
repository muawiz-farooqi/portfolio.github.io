class Solution:
    def sortColors(self, nums: List[int]) -> None:
        """
        Do not return anything, modify nums in-place instead.
        """
        r_count = 0
        w_count = 0
        b_count = 0
        for i in range(len(nums)):
            if nums[i] == 0:
                r_count += 1
            if nums[i] == 1:
                w_count += 1
            if nums[i] == 2:
                b_count += 1
        
        i = 0
        
        for a in range(r_count):
            nums[i] = 0
            i += 1
        for b in range(w_count):
            nums[i] = 1
            i += 1
        for c in range(b_count):
            nums[i] = 2
            i += 1