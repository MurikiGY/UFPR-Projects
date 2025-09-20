#!/usr/bin/env python3

"""
 Author: Eduardo Faria Kruger
 Description: Decryption algorithm
"""

import sys

modValue = 1114112

def sum_digits(digit):
    return sum(int(x) for x in digit)

"""
 Processes the key as a string
"""
def de_vinegere(key, raw_text, decrypted_text):
    key_vec = list(key)  # List because class String is immutable, please Dear Muriki, stop writing such premium english comments uhuum
    key_size = len(key)
    #print("=== Vinegere", f'Key: {key}', f'Len: {key_size} ===')

    i = 0;

    
    for token in raw_text:
        if i == key_size:
            i = 0
        original_token = chr((ord(token) - ord(key_vec[i])) % modValue)  #this only works because mod in python is non-negative
        decrypted_text.append(original_token)
        key_vec[i] = original_token
        i+=1
 
"""
 Processes the key as a number
 The key represents the number of columns
"""
def de_rail_fence(key, raw_text, decrypted_text):
    num_lines = len(raw_text) // key
    remainder = len(raw_text) % key

    # Append the complete lines
    for i in range(num_lines):
        j = i
        for _ in range(remainder):
            decrypted_text.append(raw_text[j])
            j += num_lines+1
        for _ in range(key-remainder):
            decrypted_text.append(raw_text[j])
            j += num_lines

    # Append the last incomplete line
    for i in range(remainder):
        decrypted_text.append(raw_text[num_lines+i*(num_lines+1)])


# === Main

# Check parameters
if len(sys.argv) != 2 or len(sys.argv[1]) != 8 or not sys.argv[1].isdigit():
    print("Usage:   ./encrypt.py <date> < input\n"
          "Example: ./encrypt.py 02022025 < input")
    sys.exit()

# Parse the values and print
date = sys.argv[1]
replace_loop = sum_digits(date[:4])
transform_loop = sum_digits(date[4:8])
#print(f'vin. key: {date[:2]}', file=sys.stderr)
#print(f'rail f. key: {date[2:4]}', file=sys.stderr)
#print(f'replace: {replace_loop}', file=sys.stderr)
#print(f'transform: {transform_loop}', file=sys.stderr)

# Read input
raw_text = list(); crypted_text = list()
for line in sys.stdin:
    raw_text += line #python and its cusiosities
#print(f'Raw: {raw_text}')

# === Rail Fence
#print(f'\n- Transform loops {transform_loop}')
for _ in range(transform_loop):
    de_rail_fence(int(date[2:4]), raw_text, crypted_text)
    raw_text = crypted_text.copy()
    crypted_text.clear()
#print(f'Rail Fence: {raw_text}')

# === Vigenere
#print(f'\n- Replace loops {replace_loop}')
#for _ in range(replace_loop):
#    de_vinegere(date[:2], raw_text, crypted_text)
#    raw_text = crypted_text.copy()
#    crypted_text.clear()
#print(f'Vinegere: {raw_text}')

print(''.join(raw_text), end='')
