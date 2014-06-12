function stdev(sumsq,average,count) {
	return sqrt((sumsq/count) - average*average);
} function max(a, b) {
	return a < b ? b : a;
} {
	if (last=="") {
		last=$1;
		min2=max2=$2;
		total2=0;
		sumsq2=0;
		count2=0;
		min3=max3=$3;
		total3=0;
		sumsq3=0;
		count3=0;
	};
	if (last!=$1) {
		ave2=total2/count2
		stdev2=stdev(sumsq2,ave2,count2)
		ave3=total3/count3
		stdev3=stdev(sumsq3,ave3,count3)
		print last, max(ave2-stdev2, 0), ave2, ave2+stdev2, min2, max2, max(ave3-stdev3, 0), ave3, ave3+stdev3, min3, max3
		last=$1;
		min2=max2=$2;
		total2=0;
		sumsq2=0;
		count2=0;
		min3=max3=$3;
		total3=0;
		sumsq3=0;
		count3=0;
	}
	if ($2>max2) {
		max2=$2
	};
	if ($2<min2) {
		min2=$2
	};
	total2+=$2;
	sumsq2+=($2*$2);
	count2+=1;
	if ($3>max3) {
		max3=$3
	};
	if ($3<min3) {
		min3=$3
	};
	total3+=$3;
	sumsq3+=($3*$3);
	count3+=1;
} END {
	if ($2>max2) {
		max2=$2
	};
	if ($2<min2) {
		min2=$2
	};
	total2+=$2;
	sumsq2+=($2*$2);
	count2+=1;
	if ($3>max3) {
		max3=$3
	};
	if ($3<min3) {
		min3=$3
	};
	total3+=$3;
	sumsq3+=($3*$3);
	count3+=1;
	ave2=total2/count2
	stdev2=stdev(sumsq2,ave2,count2)
	ave3=total3/count3
	stdev3=stdev(sumsq3,ave3,count3)
	print last, max(ave2-stdev2, 0), ave2, ave2+stdev2, min2, max2, max(ave3-stdev3, 0), ave3, ave3+stdev3, min3, max3
}

