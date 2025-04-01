export KICADMK_DIR=kicad-makefile
mkdir -p venv
python3 -m venv venv
source venv/bin/activate
python3 -m pip install kibom
make
export BASENAME=$(basename -- "$(ls *.kicad_pcb 2>/dev/null | head -n 1)" .kicad_pcb)
export PROD_SRC_PATH="output/prod/$BASENAME-prod.zip"
export PROD_DST_PATH="output/prod/$BASENAME-prod-$(git rev-parse --short HEAD)-$(git show -s --format=%cd --date=format:%Y%m%d).zip"
mv "${PROD_SRC_PATH}" "$PROD_DST_PATH"
export DIST_SRC_PATH="output/dist/$BASENAME.zip"
export DIST_DST_PATH="output/dist/$BASENAME-$(git rev-parse --short HEAD)-$(git show -s --format=%cd --date=format:%Y%m%d).zip"
mv "${DIST_SRC_PATH}" "$DIST_DST_PATH"
