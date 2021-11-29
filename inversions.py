filein = open("inversions.in", "r")
N = int(filein.readline())
A = list(map(int, filein.readline().split()))
inv_count = 0

def merging(left, right):
    global inv_count
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
            inv_count = inv_count + left_len - i
            j += 1

    result = result + left[i:] + right[j:]
    return result

def merge_sort(array):
    if len(array) == 1 or len(array) == 0:
        return array
    middle = len(array) // 2
    arr1 = array[:middle]
    arr2 = array[middle:]
    arr1 = merge_sort(arr1)
    arr2 = merge_sort(arr2)

    return merging(arr1, arr2)

merge_sort(A)
fileout = open("inversions.out", "w")
print(inv_count, file = fileout)

fileout.close()
