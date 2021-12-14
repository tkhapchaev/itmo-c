file_in = open("quack.in", "r")
file_out = open("quack.out", "w")
commands = file_in.read().split()
command_counter = 0
REGISTERS = {}
LABELS = {}
QUEUE = []

for i in range(ord("a"), ord("z") + 1):
    REGISTERS.setdefault(chr(i), 0)

for j in range(len(commands)):
    if commands[j][0] == ":":
        LABELS.setdefault(commands[j][1:], j)

while command_counter < len(commands):
    current_command = commands[command_counter]
    is_a_number = True

    if (current_command[0] == "+"):
        is_a_number = False
        x = int(QUEUE[0])
        QUEUE.pop(0)
        y = int(QUEUE[0])
        QUEUE.pop(0)
        QUEUE.append((x + y) & 65535)

    if (current_command[0] == "-"):
        is_a_number = False
        x = int(QUEUE[0])
        QUEUE.pop(0)
        y = int(QUEUE[0])
        QUEUE.pop(0)
        QUEUE.append((x - y) & 65535)

    if (current_command[0] == "*"):
        is_a_number = False
        x = int(QUEUE[0])
        QUEUE.pop(0)
        y = int(QUEUE[0])
        QUEUE.pop(0)
        QUEUE.append((x * y) & 65535)

    if (current_command[0] == "%"):
        is_a_number = False
        x = int(QUEUE[0])
        QUEUE.pop(0)
        y = int(QUEUE[0])
        QUEUE.pop(0)

        if y == 0:
            QUEUE.append(0)
        else:
            QUEUE.append((x % y) & 65535)

    if (current_command[0] == "/"):
        is_a_number = False
        x = int(QUEUE[0])
        QUEUE.pop(0)
        y = int(QUEUE[0])
        QUEUE.pop(0)

        if y == 0:
            QUEUE.append(0)
        else:
            QUEUE.append((x // y) & 65535)

    if (current_command[0] == ">"):
        is_a_number = False
        REGISTERS[current_command[1]] = QUEUE.pop(0)

    if (current_command[0] == "<"):
        is_a_number = False
        QUEUE.append(REGISTERS[current_command[1]])

    if (current_command[0] == "P"):
        is_a_number = False
        if len(current_command) > 1:
            print(REGISTERS[current_command[1]], file = file_out)
        else:
            print(QUEUE.pop(0), file = file_out)

    if (current_command[0] == "C"):
        is_a_number = False
        if len(current_command) > 1:
            print(chr(REGISTERS[current_command[1]]), file = file_out, end = "")
        else:
            print(chr(QUEUE.pop(0) & 255), file = file_out, end = "")

    if (current_command[0] == "J"):
        is_a_number = False
        command_counter = LABELS[current_command[1:]]

    if (current_command[0] == "Z"):
        is_a_number = False
        if REGISTERS[current_command[1]] == 0:
            command_counter = LABELS[current_command[2:]]

    if (current_command[0] == "E"):
        is_a_number = False
        if REGISTERS[current_command[1]] == REGISTERS[current_command[2]]:
            command_counter = LABELS[current_command[3:]]

    if (current_command[0] == "G"):
        is_a_number = False
        if REGISTERS[current_command[1]] > REGISTERS[current_command[2]]:
            command_counter = LABELS[current_command[3:]]

    if (current_command[0] == "Q"):
        is_a_number = False
        break

    if (is_a_number and current_command[0] != ":"):
        QUEUE.append(int(current_command))

    command_counter += 1

file_out.close()