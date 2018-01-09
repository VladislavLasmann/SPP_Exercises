#!/bin/bash
#
# Begriffe die in < > Zeichen geklammert sind, enthalten eine Beschreibung,
# die sie durch einen entsprechenden Wert oder Namen ersetzten müssen.
# Insbesondere sollten sie ihrem Job
#   - einen Namen geben (nach dem -J Parameter)
#   - ein Datei in die die Ausgabe von stderr erfolgen soll (nach dem -e Parameter)
#   - ein Datei in die die Ausgabe von stdout erfolgen soll (nach dem -o Parameter)
#   - Die Anzahl der Threads die sie benötigen (nach dem -c Parameter)
#
#SBATCH -J task3b
#SBATCH --mail-type=END
# Bitte achten Sie auf vollständige Pfad-Angaben:
#SBATCH -e /home/kurse/kurs00015/km89hifu/projects/Praktikum1/Abgabe/Task3/b/OUTPUT/task3b.err.%j
#SBATCH -o /home/kurse/kurs00015/km89hifu/projects/Praktikum1/Abgabe/Task3/b/OUTPUT/task3b.out.%j
#SBATCH --account=kurs00015
#SBATCH --partition=kurs00015
#SBATCH --reservation=kurs00015
#
#SBATCH -n 1       # 1 Prozess
#SBATCH -c 8       # 8 Kerne pro Prozess
#SBATCH --mem-per-cpu=1750    # Hauptspeicher in MByte pro Rechenkern
#SBATCH -t 00:05:00     # in Stunden:Minuten:Sekunden


# -------------------------------
# Anschließend schreiben Sie Ihre eigenen Befehle, wie z.B.
# Laden sie benötigte Module
module load gcc
# Wechseln sie in das Verzeichnis in dem ihr Executable liegt
cd /home/kurse/kurs00015/km89hifu/projects/Praktikum1/Abgabe/Task3/b
# Ihr Programmaufruf
make

./task3
