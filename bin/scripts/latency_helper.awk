BEGIN {
	not_odd = 0
	worst = 0
	sum = 0
}
{ 
	line = $0; 
		if (not_odd)
		{
			if (line >= last)	
				val = line-last;
			else
				val = (line-last+1000);
			if (val > worst)
				worst = val;
			sum = sum+val;
			latency[val]++;
			#print val;
		}	
	last = line;
	not_odd = !not_odd;	
}
END {
	for (i in latency) 
		print i, latency[i]
	printf("Worst: %d\n", worst);
	printf("Avg: %f\n", 2*sum/NR);
}
