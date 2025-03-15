# GRR20203933 - Muriki Gusmao Yamanaka
# GRR20205152 - Wellington de Almeida Silva
#!/bin/bash

#------
#parametros do programa
VAR_NUM=("32" "64" "128" "200" "256" "512" "600" "1000" "2000" "4000")
#VAR_NUM=("10" "20" "30")
INIT_VALUE=1
K=5
EPS="1e-3"
ITER="50"
HES_STEP=5
#------

#------
METRIC=("L3" "L2CACHE" "FLOPS_DP")
METRIC_RESULT=("L3 bandwidth \[MBytes/s\]" "L2 miss ratio" "DP \[MFLOP/s\];AVX DP \[MFLOP/s\];Runtime unhalted \[s\]")
SAIDA_EST="saida_estatisticas/"
SAIDA_PLOT="plotagens/"
FUNC_MARK=("NEWTON_MOD" "NEWTON_MOD_OPT")
PONTOS_CRITICOS="./pontosCriticos"
PLOT_SCPT="./plot.gp"
#------

mkdir -p ${SAIDA_EST}
mkdir -p ${SAIDA_PLOT}

function cancel(){
    exit 1
}


make purge
make

trap cancel INT

echo "performance" > /sys/devices/system/cpu/cpufreq/policy3/scaling_governor

TIME=$(date +%s)
OUT_PONTOS_CRIT="saida_pontosCriticos_$TIME"
for i in $( seq 0 $((${#METRIC[@]} - 1)) )
do
    rm -f "${SAIDA_EST}/${METRIC[$i]}_${F}.txt"
    echo ${METRIC[$i]}

    for NUM in ${VAR_NUM[@]}
    do
        #roda likwid
        REL="${SAIDA_EST}/${METRIC[$i]}_${NUM}.csv"
        INPUT="${NUM} ${K} ${INIT_VALUE} ${EPS} ${ITER} ${HES_STEP}"
        likwid-perfctr -o "$REL" -C 1 -g ${METRIC[$i]} -m ${PONTOS_CRITICOS} < <(echo "${INPUT}") &>/dev/null

        #obtem dados para a tabela do teste
        while read -r TEST
        do
            DATA="$NUM "
            for F in ${FUNC_MARK[@]}
            do
                DATA="${DATA} $(sed -n "/Region ${F},Group 1 Metric,${METRIC[$i]}/"',$p' "$REL" | grep "${TEST}" | head -n1 | cut -d"," -f2)"
            done

            OUTPUT_NAME="${METRIC[$i]}"
            if [ "${METRIC[${i}]}" = "FLOPS_DP" ]
            then
                TEST_NAME=$(echo ${TEST} | cut -d" " -f1)
                OUTPUT_NAME="${METRIC[$i]}_${TEST_NAME}"
            fi

            #cria tabela do teste
            OUTPUT_TABLE="${SAIDA_EST}/${OUTPUT_NAME}.txt"
            echo $DATA >> "$OUTPUT_TABLE"
        done < <(echo "${METRIC_RESULT[$i]}" | tr ";" "\n")

    done

    #plota informacoes da tabela do teste
    while read -r TEST
    do
        OUTPUT_NAME="${METRIC[$i]}"
        if [ "${METRIC[${i}]}" = "FLOPS_DP" ]
        then
            TEST_NAME=$(echo ${TEST} | cut -d" " -f1)
            OUTPUT_NAME="${METRIC[$i]}_${TEST_NAME}"
        fi
        OUTPUT_TABLE="${SAIDA_EST}/${OUTPUT_NAME}.txt"

        #plota imagens
        OUTPUT_PLOT="${SAIDA_PLOT}/${OUTPUT_NAME}.png"
        gnuplot -c "$PLOT_SCPT" "$OUTPUT_TABLE" "$(echo "${FUNC_MARK[0]}" | tr -s '_' ' ')" \
            "$(echo "${FUNC_MARK[1]}" | tr -s '_' ' ')" "$(echo "${METRIC[$i]}" | tr -s '_' ' ')" "$(echo "${TEST}" | tr -d '\\')" "$OUTPUT_PLOT"
    done < <(echo "${METRIC_RESULT[$i]}" | tr ";" "\n")

done

echo "powersave" > /sys/devices/system/cpu/cpufreq/policy3/scaling_governor
