optsetup = ${LATEXDIR}/options-setup.sty
mathsetup = ${LATEXDIR}/math-setup.tex
inputs = options.md options-1.md
options.pdf : $(inputs) $(mathsetup)
	pandoc -s -o options.pdf $(inputs) --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

clean :
	rm options.pdf
