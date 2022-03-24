optsetup = ${LATEXDIR}/options-setup.sty
mathsetup = ${LATEXDIR}/math-setup.tex

proposal1.pdf : proposal1.md $(mathsetup)
	pandoc -s -o proposal1.pdf proposal1.md --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

proposal2.pdf : proposal2.md $(mathsetup)
	pandoc -s -o proposal2.pdf proposal2.md --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

ilv1.pdf : ilv1.md $(mathsetup)
	pandoc -s -o ilv1.pdf ilv1.md --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

clean :
	rm *.pdf
