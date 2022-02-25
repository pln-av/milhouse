optsetup = ${LATEXDIR}/options-setup.sty
mathsetup = ${LATEXDIR}/math-setup.tex
inputs = pf.md pf-1.md pf-2.md
pf.pdf : $(inputs) $(mathsetup)
	pandoc -s -o pf.pdf $(inputs) --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

clean :
	rm pf.pdf
