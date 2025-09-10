# LaTeX

- github
  - vscode ext: [LaTeX-Workshop](https://github.com/James-Yu/LaTeX-Workshop)
  - formatter: [tex-fmt](https://github.com/WGUNDERWOOD/tex-fmt)

```bash
sudo apt install \
texlive texlive-science texlive-latex-extra latexmk
```

## Build PDF

```bash
cd algorithms

latexmk -pdf -interaction=nonstopmode -halt-on-error -g algorithm.tex
```
