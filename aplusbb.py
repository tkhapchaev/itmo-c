file_in = open("aplusbb.in", "r")
file_out = open("aplusbb.out", "w")
a, b = map(int, file_in.readline().split())
print(a + b * b, file = file_out)
file_out.close()
