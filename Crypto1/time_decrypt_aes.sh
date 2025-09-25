#!/usr/bin/env bash

#create folder so you can replicate it, Muriki
mkdir -p time_decrypted_books_aes

for FILE in encrypted_books_aes/*;
do
    NAME=$(basename "$FILE")
    START=$(date +%s.%6N)

    #apply the command for every book in books and saves te encrypted version in encrypted_books_aes
    #flags:
    #   -d: encrypt
    #   -in: input file
    #   -k password
    #   -out: output file
    #   -pbkdf2: aplies a transformation in the password
    #   -iter:  says how many times pbkf2 is applied i set it to 300000 cause aes is already fast
    #   -salt: aplies a "grain of salt" so that there is different outputs for equal inputs
    openssl aes-128-cbc -d -k 31122025 -pbkdf2 -iter 300000 -salt -in "$FILE"

    #couldnt find another way for it to record the time in more than 3 decimal places
    END=$(date +%s.%6N)
    DIFF=$(echo "$END" - "$START" | bc)

    #saves the time im time_encrypted_books_aes
    echo "A execucao levou $DIFF segundos" > "time_decrypted_books_aes/${NAME}.txt"
done

#some reference
#https://stackoverflow.com/questions/15015809/floating-point-results-in-bash-integer-division
