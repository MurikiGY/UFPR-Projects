#!/usr/bin/env python3

"""
Author: Muriki G. Yamanaka
Description: Encryption algorithm
"""

import sys

def sum_digits(digit):
    return sum(int(x) for x in digit)

def vinegere(key):
    key_vec = list(key)  # List because the class String is immutable
    key_size = len(key)
    print("=== Vinegere", f'Key: {key}', f'Len: {key_size} ===')

    i = 0;
    for line in sys.stdin:
        for token in line:
            if i == key_size:
                i = 0
            #print(chr(ord(token)+ord(key_vec[i])))
            key_vec[i] = token
            i+=1

def rail_fence(key):
    key_vec = list(key)  # List because the class String is immutable
    key_size = len(key)
    print("=== Rail Fence", f'Key: {key}', f'Len: {key_size} ===')


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
replace_loop = day+month
transform_loop = sum_digits(date[4:8])

#print(f'date: {day}')
#print(f'month: {month}')
#print(f'year: {year}')
#print(f'replace: {replace_loop}')
#print(f'transform: {transform_loop}')

#for _ in range(replace_loop):
print(f'- Replace loops {replace_loop}')
vinegere(date[:2])

#for _ in range(transform_loop):
print(f'- Replace loops {transform_loop}')
rail_fence(date[2:4])
