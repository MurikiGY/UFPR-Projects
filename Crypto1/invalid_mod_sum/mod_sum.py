import unicodedata

def change_invalid(x):
    if unicodedata.category(x) == 'Cs':
        return chr(ord(x) + 0x11000)
    return x

with open('a', 'w') as f:
    f.write(''.join(change_invalid(chr(x)) for x in range(0x11000)))
