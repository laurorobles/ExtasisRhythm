import re
import sys

def main():
    with open('src/PluginEditor.cpp', 'r') as f:
        content = f.read()
    
    print("Found ExtasisRhythmEditor::resized")
    
if __name__ == "__main__":
    main()
