import sys

if len(sys.argv) != 2:
    print("Usage: file_stats.py <filename>\n\t       where <filename> is a *.txt file containing a space delimitted array of numbers.")
    sys.exit(1)

i = 0

arr = [[]]

with open(sys.argv[1], 'r') as f:
    lines_list = f.readlines()
    for line in lines_list:
        arr.append(line.split())

sum = [0] * len(arr[1])

for i in range(1, len(arr)):
    for j in range(0, len(arr[i])):
        sum[j] += float(arr[i][j])

mean = [0] * len(arr[1])

for j in range(0, len(sum)):
    mean[j] = sum[j] / (len(arr)-1)

std_dev = [0] * len(arr[1])

for i in range(1, len(arr)):
    for j in range(0, len(arr[i])):
        mean_diff = float(arr[i][j]) - mean[j]
        std_dev[j] += mean_diff ** 2

for j in range(0, len(std_dev)):
    std_dev[j] /= (len(arr)-2)
    print(f"Column {j}: mean = %.4f, std = %.4f" % (mean[j], std_dev[j] ** 0.5))


