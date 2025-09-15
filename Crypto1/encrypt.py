#!/usr/bin/env python3

"""
 Author: Muriki G. Yamanaka
 Description: Encryption algorithm
"""

import sys

def sum_digits(digit):
    return sum(int(x) for x in digit)

"""
 Processes the key as a string
"""
def vinegere(key, raw_text, crypted_text):
    key_vec = list(key)  # List because class String is immutable
    key_size = len(key)
    #print("=== Vinegere", f'Key: {key}', f'Len: {key_size} ===')

    i = 0;
    for token in raw_text:
        if i == key_size:
            i = 0
        # Mod because of the chr 0x110000 (1114112 ₁₀) input limit
        crypted_text.append(chr((ord(token)+ord(key_vec[i])) % 1114112))
        key_vec[i] = token
        i+=1;


"""
 Processes the key as a number
 The key represents the number of columns
"""
def rail_fence(key, raw_text, crypted_text):
    #print("=== Rail Fence", f'Key: {key} ===')
    
    for i in range(key):
        crypted_text += raw_text[i::key]



# === Main

# Check parameters
if len(sys.argv) != 2 or len(sys.argv[1]) != 8 or not sys.argv[1].isdigit():
    print("Usage:   ./encrypt.py <date> < input\n"
          "Example: ./encrypt.py 02022025 < input")
    sys.exit()

# Parse the values
date = sys.argv[1]
day = int(date[:2])
month = int(date[2:4])
year = int(date[4:8])
#replace_loop = day+month
replace_loop = sum_digits(date[:4])
transform_loop = sum_digits(date[4:8])

#print(f'date: {day}')
#print(f'month: {month}')
#print(f'year: {year}')
#print(f'replace: {replace_loop}')
#print(f'transform: {transform_loop}')

# Read input
raw_text = list(); crypted_text = list()

for line in sys.stdin:
    raw_text += line
#print(f'Raw: {raw_text}')

# Vigenere
#print(f'\n- Replace loops {replace_loop}')
for _ in range(replace_loop):
    vinegere(date[:2], raw_text, crypted_text)
    raw_text = crypted_text.copy()
    crypted_text.clear()
#print(f'Vinegere: {raw_text}')

# Rail Fence
#print(f'\n- Transform loops {transform_loop}')
for _ in range(transform_loop):
    rail_fence(int(date[2:4]), raw_text, crypted_text)
    raw_text = crypted_text.copy()
    crypted_text.clear()

#print(f'Rail Fence: {raw_text}')


print(''.join(raw_text))
