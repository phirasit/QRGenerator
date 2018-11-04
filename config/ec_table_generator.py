for i in range(40):
    print('{', end='')
    for _ in range(4):
        inp = input()
        result = '{0, 0}'
        if inp.strip() != '':
            a, b = map(int, inp.split())
            result = '{%d, %d}' % (a, b)

        if _ < 3:
            print(result + ', ', end='')
        else:
            print(result, end='')
    print('},')
