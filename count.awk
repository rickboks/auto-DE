BEGIN{max=0}; 
/^[0-9]/{a[$0]++; if ($0 > max)max=$0} 
END{
	sum = 0
	for (i=0; i<=max; i++)
		sum += a[i]
	for (i=0; i<=max; i++)
		printf("%d: %.3g\n", i, (a[i]/sum)*100.0)
}
