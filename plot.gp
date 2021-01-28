set logscale y
filelist=system("ls plotdata/*")
plot for [filename in filelist] filename with lines title filename
