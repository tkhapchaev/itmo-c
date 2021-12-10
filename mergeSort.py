file_in = open("sort.in", "r")
N = int(file_in.readline())
input_array = list(map(int, file_in.readline().split()))

def merging(left, right):
    i = 0
    j = 0
    result = []
    left_len = len(left)
    right_len = len(right)

    while i < left_len and j < right_len:
        if left[i] <= right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            j += 1

    result = result + left[i:] + right[j:]
    return result

def merge_sort(array):
    if len(array) == 1 or len(array) == 0:
        return array
    middle = len(array) // 2
    merged_array_1 = array[:middle]
    merged_array_2 = array[middle:]
    merged_array_1 = merge_sort(merged_array_1)
    merged_array_2 = merge_sort(merged_array_2)

    return merging(merged_array_1, merged_array_2)

file_out = open("sort.out", "w")
print(*merge_sort(input_array), file = file_out)
file_out.close()
