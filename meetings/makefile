optsetup = ${LATEXDIR}/options-setup.sty
mathsetup = ${LATEXDIR}/math-setup.tex

10032022 : 10032022.md $(mathsetup)
	pandoc -s -o 10032022.pdf 10032022.md --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

11032022 : 11032022.md $(mathsetup)
	pandoc -s -o 11032022.pdf 11032022.md --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

clean :
	rm *2022.pdf
