GPP=@g++
LATEX=@pdflatex -jobname
#GDD
PDFNAME=GDD
PDFPATH=./Documents/GDD
OUT=-output-directory
PDFFILES=./Documents/GDD/*.aux ./Documents/GDD/*.log ./Documents/GDD/*.out ./Documents/GDD/*.toc ./Documents/GDD/*.pdf
#C++
FLAGS=-g -Wall -Wextra -o
#Divers
WIPE=-@rm -f
FILES=*.o 2>/dev/null || true
README=@cat README.md

base: clean gdd readme

gdd:
	$(LATEX) $(PDFNAME) $(OUT) $(PDFPATH) $(PDFPATH)/main.tex >/dev/null
readme:
	$(README)
clean:
	$(WIPE) $(FILES)
	$(WIPE) $(PDFFILES)
