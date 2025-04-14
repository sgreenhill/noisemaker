stats "plot-600.csv" nooutput
f600 = 1.0/STATS_records
stats "plot-720.csv" nooutput
f720 = 1.0/STATS_records
stats "plot-816.csv" nooutput
f816 = 1.0/STATS_records
stats "plot-912.csv" nooutput
f912 = 1.0/STATS_records

bin_width = 0.1;
bin_width = 5;
set xtics 5
bw = bin_width / 5;
set boxwidth bw absolute

bin_number(x) = floor(x/bin_width)
bin(x,off) = bin_width * ( bin_number(x) + 0.5 ) + off * bw
set style fill solid 1.0 noborder

set xlabel "Audio Processor Usage (%)"
set ylabel "Frequency (fraction of 128 soundbank patches)"

set terminal png size 1024,512
set output 'perform-freq.png'

set title "Audio Processor Usage by CPU Frequency (4 voices)"

plot "plot-600.csv" using (bin($8,1)):(f600) smooth frequency with boxes title "4 voices F=600MHz", "plot-720.csv" using (bin($8,2)):(f720) smooth frequency with boxes title "4 voices F=720MHz", "plot-816.csv" using (bin($8,3)):(f816) smooth frequency with boxes title "4 voices F=816MHz", "plot-912.csv" using (bin($8,4)):(f912) smooth frequency with boxes title "4 voices F=912MHz"

set output 'perform-freq-5.png'

set title "Audio Processor Usage by CPU Frequency (5 voices)"
plot "plot-720.csv" using (bin($9,1)):(f720) smooth frequency with boxes title "5 voices F=720MHz", "plot-816.csv" using (bin($9,2)):(f816) smooth frequency with boxes title "5 voices F=816MHz", "plot-912.csv" using (bin($9,3)):(f912) smooth frequency with boxes title "5 voices F=912MHz"

set output 'perform-voices.png'

set title "Audio Processor Usage by Number of Voices (fCPU=912MHz)"
plot "plot-912.csv" using (bin($7,1)):(f912) smooth frequency with boxes title "3 voices F=912MHz", "plot-912.csv" using (bin($8,2)):(f912) smooth frequency with boxes title "4 voices F=912MHz", "plot-912.csv" using (bin($9,3)):(f912) smooth frequency with boxes title "5 voices F=912MHz", "plot-912.csv" using (bin($10,4)):(f912) smooth frequency with boxes title "6 voices F=912MHz"

