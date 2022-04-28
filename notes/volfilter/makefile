optsetup = ${LATEXDIR}/options-setup.sty
mathsetup = ${LATEXDIR}/math-setup.tex
inputs = vf.md vf.lv.md
options.pdf : $(inputs) $(mathsetup)
	pandoc -s -o vf.pdf $(inputs) --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

sim_under: sim.md 
	pandoc -s -o sim_under.pdf sim.md --listings -H $(optsetup) -H $(mathsetup) --pdf-engine /Library/TeX/texbin/pdflatex

clean :
	rm vf.pdf
