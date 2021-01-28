set logscale
filelist=system("ls data/*")
plot for [filename in filelist] filename with lines title filename
