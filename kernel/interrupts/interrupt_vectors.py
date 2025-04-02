
def generate_vector(num):
    return """align 16
vector_{num}_handler:
push $0
push ${num}
jmp interrupt_stub
""".format(num = num)

print("extern interrupt_stub")
for i in range(0, 256):
    print(generate_vector(i))