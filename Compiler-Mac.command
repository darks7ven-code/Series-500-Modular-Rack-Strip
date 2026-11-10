#!/bin/bash
# Double-cliquez sur ce fichier pour compiler le plugin.
export PATH="$PATH:/opt/homebrew/bin:/usr/local/bin:/Applications/CMake.app/Contents/bin"
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)" || { echo "Impossible d'ouvrir le dossier du projet."; read -n 1 -p "Appuyez sur une touche..."; exit 1; }
cd "$PROJECT_DIR" || exit 1
LOG="$HOME/Desktop/Series500ModularRack-build-log.txt"
{
  echo "Dossier : $(pwd)"
  echo "macOS : $(sw_vers -productVersion) / $(uname -m)"
  echo "CMake : $(cmake --version 2>&1 | head -1)"
  echo "Outils Xcode : $(xcode-select -p 2>&1)"
  echo "Git : $(git --version 2>&1)"
} | tee "$LOG"
fail() {
  echo ""
  echo "############ ERREUR a l'etape $1 ############"
  echo "Les dernieres lignes importantes :"
  grep -iE "error|fatal|not found|failed" "$LOG" | tail -15
  echo ""
  echo "Le journal complet est sur le Bureau : Series500ModularRack-build-log.txt"
  read -n 1 -p "Appuyez sur une touche pour fermer..."
  exit 1
}

echo "=== 1/3 Preparation (telechargement de JUCE, quelques minutes) ===" | tee -a "$LOG"
cmake -S "$PROJECT_DIR" -B "$PROJECT_DIR/build" -DCMAKE_BUILD_TYPE=Release 2>&1 | tee -a "$LOG"
[ "${PIPESTATUS[0]}" -eq 0 ] || fail 1

echo "=== 2/3 Compilation (5 a 15 minutes) ===" | tee -a "$LOG"
cmake --build "$PROJECT_DIR/build" --config Release -j2 2>&1 | tee -a "$LOG"
[ "${PIPESTATUS[0]}" -eq 0 ] || fail 2

echo "=== 3/3 Terminé ===" | tee -a "$LOG"
echo "VST3, AU et Standalone sont dans : $PROJECT_DIR/build/RackStrip_artefacts/Release"
echo "Le build ne copie aucun plugin dans les dossiers système."
echo ""
echo "Terminé. Installez le format souhaité manuellement dans votre DAW."
read -n 1 -p "Appuyez sur une touche pour fermer..."
