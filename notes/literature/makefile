optsetup = ${LATEXDIR}/options-setup.sty
mathsetup = ${LATEXDIR}/math-setup.tex
inputs = literature.md
literature.pdf : $(inputs) $(mathsetup)
	pandoc -s -o literature.pdf $(inputs) --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

clean :
	rm literature.pdf
