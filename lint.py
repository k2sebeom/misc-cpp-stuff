

with open('encode.cc', 'r') as f:
    s = f.read()

lines = s.split('\n')

with open('output.cc', 'w') as f:
    for line in lines:
        i = 1
        line = line.lstrip()
        while True:
            try:
                num = int(line[:i])
                print(num)
                
                if i >= len(line) or line[i] == ' ':
                    break
                i += 1
            except Exception:
                break
        f.write(line[i + 1:] + '\n')