file_in = open("sortland.in", "r")
file_out = open("sortland.out", "w")
N = int(file_in.readline())
array_input = list(map(float, file_in.readline().split()))
array_output = []

for i in range(0, len(array_input)):
    array_output.append(array_input[i])
for i in range(1, len(array_input)):
    key = array_input[i]
    j = i - 1
    while key < array_input[j] and j >= 0:
        array_input[j + 1] = array_input[j]
        j = j - 1
    array_input[j + 1] = key

richest = array_input[-1]
poorest = array_input[0]
average = array_input[len(array_input) // 2]
for i in range(0, len(array_output)):
    if array_output[i] == richest:
        richest_index = i
    if array_output[i] == poorest:
        poorest_index = i
    if array_output[i] == average:
        average_index = i

richest_index = str(richest_index + 1)
poorest_index = str(poorest_index + 1)
average_index = str(average_index + 1)
file_out.write(poorest_index + " " + average_index + " " + richest_index)
file_out.close()
