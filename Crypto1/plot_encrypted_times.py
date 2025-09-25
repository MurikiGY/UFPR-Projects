#!/usr/bin/env python3

#script from matplotlib doc
import matplotlib.pyplot as plt
import numpy as np

books = ("frankenstein", "dracula", "modydick")
cipher_times={
    'AES': (0.067958, 0.078510, 0.069228),
    'MURIKI': (0.026143, 2.296564, 3.351618)
    #'decrypt': (0.071294, 0.089050, 0.089050),
}

x = np.arange(len(books))  # the label locations
width = 0.25  # the width of the bars
multiplier = 0

fig, ax = plt.subplots(layout='constrained')

for attribute, measurement in cipher_times.items():
    offset = width * multiplier
    rects = ax.bar(x + offset, measurement, width, label=attribute)
    ax.bar_label(rects, padding=3)
    multiplier += 1

# Labels
ax.set_ylabel('Tempo (s)')
ax.set_title('Tempo de execucao dos algoritmos de cifra')
ax.set_xticks(x + width, books)
ax.legend(loc='upper left', ncols=3)
ax.set_ylim(0, 4) #heigth limite of the graph

plt.savefig('encrypt_times.pdf')
