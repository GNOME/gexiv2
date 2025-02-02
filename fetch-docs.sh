#!/usr/bin/bash -e

DOC_DIR=public/docs
REFS="
master
gexiv2-0.14
"

mkdir -p $DOC_DIR

IFS='
'


curl -L --output README.md "https://gitlab.gnome.org/GNOME/gexiv2/-/raw/master/README.md?ref_type=heads"
pandoc -s README.md -o public/index.html --metadata=pagetitle:"GExiv2" --css=style.css
pandoc -s versions.md -o $DOC_DIR/index.html --metadata=pagetitle:"GEXiv2 Versions" --css=style.css

for REF in $REFS; do 
    API_VERSION=${REF//gexiv2-/}
    curl -L --output "$REF.zip" "https://gitlab.gnome.org/GNOME/gexiv2/-/jobs/artifacts/$REF/download?job=docs"
    unzip -d "$REF" "$REF.zip"
    mv "$REF/reference" "$DOC_DIR/$API_VERSION"
    rm "$REF.zip"
    rm -rf "$REF"
done

ln -sr $DOC_DIR/0.14 $DOC_DIR/latest
